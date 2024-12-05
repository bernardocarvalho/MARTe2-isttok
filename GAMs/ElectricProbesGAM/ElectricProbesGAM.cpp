/**
 * @file ElectricProbesGAM.cpp
 * @brief Source file for class ElectricProbesGAM
 * @date 06/04/2018
 * @author Andre Neto
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class ElectricProbesGAM (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "CLASSMETHODREGISTER.h"

#include "ElectricProbesGAM.h"
#include "RegisteredMethodsMessageFilter.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

//namespace MARTeIsttok {
namespace MARTe {
/**
 * The number of signals
 */
    const  uint32 EP_NUM_INPUTS  = 4u;
    const  uint32 EP_NUM_OUTPUTS = 2u;

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
//namespace MARTeIsttok {
    ElectricProbesGAM::ElectricProbesGAM() : 
                GAM(),
                MessageI() {
        gain = 0u;
        numberOfSamplesAvg = 1u;
        numberOfInputElements = 0u;

        //outputSignals = NULL_PTR(MARTe::float32 **);
/*
        inputElectricTop    = NULL_PTR(MARTe::float32 *);
        inputElectricInner  = NULL_PTR(MARTe::float32 *);
        inputElectricOuter  = NULL_PTR(MARTe::float32 *);
        inputElectricBottom = NULL_PTR(MARTe::float32 *);
*/
        triggerSdas = NULL_PTR(MARTe::uint32 *);
        inputSignal = NULL; // NULL_PTR(MARTe::float32*);

        outputEpR = NULL_PTR(MARTe::float32 *);
        outputEpZ = NULL_PTR(MARTe::float32 *);
        resetInEachState = false;

        lastInputs = NULL_PTR(MARTe::float32**);
        lastTriggerSdas = 0u;
    }

    ElectricProbesGAM::~ElectricProbesGAM() {
        //if (inputSignal != NULL_PTR(MARTe::float32 **)) {
       //     delete[] inputSignal;
        //}
        inputSignal =  NULL; //NULL_PTR(MARTe::float32*);
        /*if (outputSignals != NULL_PTR(MARTe::float32 **)) {
            delete[] outputSignals;
        }
        */
        if (lastInputs != NULL_PTR(MARTe::float32**)) {
            MARTe::uint32 k;
            for (k=0u; k < EP_NUM_INPUTS; k++) {
                if (lastInputs[k] != NULL_PTR(MARTe::float32*)) {
                    delete[] lastInputs[k];
                }
            }
            delete[] lastInputs;
        }
    }

    bool ElectricProbesGAM::Initialise(MARTe::StructuredDataI & data) {
        using namespace MARTe;
        bool ok = GAM::Initialise(data);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Could not Initialise the GAM");
        }
        if (ok) {
            ok = data.Read("Gain", gain);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "The parameter Gain shall be set");
            }
        }
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "Parameter Gain set to %d", gain);
        }
        if (ok) {
            ok = data.Read("NumberOfSamplesAvg", numberOfSamplesAvg);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, 
                        "The parameter NumberOfSamplesAvg shall be set");
            }
        }
        if (ok) {
            REPORT_ERROR(ErrorManagement::Information, "Parameter NumberOfSamplesAvg set to %d",
                    numberOfSamplesAvg);
        }
        if (ok) {
            uint32 auxResetInEachState = 0u;
            ok = data.Read("ResetInEachState", auxResetInEachState);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "Error reading ResetInEachState");
            }
            else {
                if (auxResetInEachState == 1u) {
                    resetInEachState = true;
                }
                else if (auxResetInEachState == 0u) {
                    resetInEachState = false;
                }
                else {
                    ok = false;
                    REPORT_ERROR(ErrorManagement::InitialisationError, "Wrong value for ResetInEachState. Possible values 0 (false) or 1 (true)");
                }
            }
        }

        return ok;
    }

    bool ElectricProbesGAM::Setup() {
        using namespace MARTe;
        uint32 numberOfInputSignals = GetNumberOfInputSignals();
        bool ok = (numberOfInputSignals == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be equal to 2. numberOfInputSignals = %d ", numberOfInputSignals);
        }
        if (ok) {

            StreamString inputSignalName;
            ok = GetSignalName(InputSignals, 0u, inputSignalName);
            TypeDescriptor inputSignalType = GetSignalType(InputSignals, 0u);
            ok = (inputSignalType == UnsignedInteger32Bit);
            if (!ok) {
                const char8 * const inputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(inputSignalType);
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The type of the input signals shall be uint32. inputSignalType = %s", inputSignalTypeStr);
            }
            uint32 numberOfInputSamples = 0u;
            if (ok) {
                ok = GetSignalNumberOfSamples(InputSignals, 0u, numberOfInputSamples);
            }

            if (ok) {
                ok = (numberOfInputSamples == 1u);
            }
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The number of input signals samples shall be equal to 1. numberOfInputSamples = %d", numberOfInputSamples);
            }
            uint32 numberOfInputDimensions = 0u;
            if (ok) {
                ok = GetSignalNumberOfDimensions(InputSignals, 0u, numberOfInputDimensions);
            }

            if (ok) {
                ok = (numberOfInputDimensions == 0u);
                if (!ok) {
                    REPORT_ERROR(
                            ErrorManagement::ParametersError,
                            "The number of input signals dimensions shall be equal to 0. numberOfInputDimensions(%s) = %d", inputSignalName.Buffer(), numberOfInputDimensions);
                }
            }
            if (ok) {
                ok = GetSignalNumberOfElements(InputSignals, 0u, numberOfInputElements);
            }
            if (ok) {
                ok = (numberOfInputElements == 1u);
            }
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The number of input signal elements shall be equal to 1. numberOfInputElements(%s) = %d", inputSignalName.Buffer(), numberOfInputElements);
            }
            ok = GetSignalName(InputSignals, 1u, inputSignalName);
            inputSignalType = GetSignalType(InputSignals, 1u);
            ok = (inputSignalType == Float32Bit);
            if (!ok) {
                const char8 * const inputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(inputSignalType);
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The type of the input signals shall be float32. inputSignalType = %s", inputSignalTypeStr);
            }
            numberOfInputSamples = 0u;
            if (ok) {
                ok = GetSignalNumberOfSamples(InputSignals, 1u, numberOfInputSamples);
            }

            if (ok) {
                ok = (numberOfInputSamples == 1u);
            }
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The number of input signals samples shall be equal to 1. numberOfInputSamples = %d", numberOfInputSamples);
            }
            numberOfInputDimensions = 0u;
            if (ok) {
                ok = GetSignalNumberOfDimensions(InputSignals, 1u, numberOfInputDimensions);
            }

            if (ok) {
                ok = (numberOfInputDimensions == 0u);
                if (!ok) {
                    REPORT_ERROR(
                            ErrorManagement::ParametersError,
                            "The number of input signals dimensions shall be equal to 0. numberOfInputDimensions(%s) = %d", inputSignalName.Buffer(), numberOfInputDimensions);
                }
            }
            if (ok) {
                ok = GetSignalNumberOfElements(InputSignals, 1u, numberOfInputElements);
            }
            if (ok) {
                ok = (numberOfInputElements == 4u);
            }
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The number of input signal elements shall be equal to 4. numberOfInputElements(%s) = %d", inputSignalName.Buffer(), numberOfInputElements);
            }


        }
        if (ok) {
            lastInputs = new float32*[numberOfInputElements];
            uint32 n;
            for (n = 0u; n < numberOfInputElements ; n++) {
                if (numberOfSamplesAvg > 1u) {
                    lastInputs[n] = new float32[numberOfSamplesAvg - 1u];
                    if (lastInputs[n] != NULL_PTR(MARTe::float32*)) {
                        uint32 i;
                        for (i = 0u; i < (numberOfSamplesAvg - 1u); i++) {
                            lastInputs[n][i] = 0.0F;
                        }
                    }
                }
            }
        }
        if (ok) {
            triggerSdas = reinterpret_cast<uint32 *>(GetInputSignalMemory(0u));
            inputSignal   = reinterpret_cast<float32 *>(GetInputSignalMemory(1u));
/*
            inputElectricTop    = reinterpret_cast<float32 *>(GetInputSignalMemory(0u));
            inputElectricInner  = reinterpret_cast<float32 *>(GetInputSignalMemory(1u));
            inputElectricOuter  = reinterpret_cast<float32 *>(GetInputSignalMemory(2u));
            inputElectricBottom = reinterpret_cast<float32 *>(GetInputSignalMemory(3u));
*/
            REPORT_ERROR(ErrorManagement::Information, "InputSignals reinterpret_cast OK");
        }


        // OutputSignals
        uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
        ok = (numberOfOutputSignals == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of output signals shall be equal to 2. numberOfOutputSignals = %d ", numberOfOutputSignals);
        }
        if (ok) {
            uint32 n;
            for (n = 0u; (n < numberOfOutputSignals) && (ok); n++) {
                StreamString outputSignalName;
                ok = GetSignalName(OutputSignals, n, outputSignalName);
                TypeDescriptor outputSignalType = GetSignalType(OutputSignals, n);
                ok = (outputSignalType == Float32Bit);
                if (!ok) {
                    const char8 * const outputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(outputSignalType);
                    REPORT_ERROR(ErrorManagement::ParametersError,
                            "The type of the output signals shall be float32. outputSignalType = %s", outputSignalTypeStr);
                }
                uint32 numberOfOutputSamples = 0u;
                if (ok) {
                    ok = GetSignalNumberOfSamples(OutputSignals, n, numberOfOutputSamples);
                }

                if (ok) {
                    ok = (numberOfOutputSamples == 1u);
                }
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError,
                            "The number of output signals samples shall be equal to 1. numberOfOutputSamples = %d", numberOfOutputSamples);
                }
                uint32 numberOfOutputDimensions = 0u;
                if (ok) {
                    ok = GetSignalNumberOfDimensions(OutputSignals, n, numberOfOutputDimensions);
                }

                if (ok) {
                    ok = (numberOfOutputDimensions == 0u);
                    if (!ok) {
                        REPORT_ERROR(ErrorManagement::ParametersError,
                                "The number of output signals dimensions shall be equal to 0.  numberOfOutputDimensions (%s) = %d", outputSignalName.Buffer(), numberOfOutputDimensions);
                    }
                }
                uint32 numberOfOutputElements = 0u;
                if (ok) {
                    ok = GetSignalNumberOfElements(OutputSignals, n, numberOfOutputElements);
                }
                if (ok) {
                    ok = (numberOfOutputElements == 1u);
                }
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError,
                                 "The number of output signals elements shall be equal to 1. (%s) numberOfOutputElements = %d", outputSignalName.Buffer(), numberOfOutputElements);
                }

            }
            if (ok) {
                outputEpR = reinterpret_cast<float32 *>(GetOutputSignalMemory(0u));
                outputEpZ = reinterpret_cast<float32 *>(GetOutputSignalMemory(1u));
            }
        }

        // Install message filter
        ReferenceT<RegisteredMethodsMessageFilter> registeredMethodsMessageFilter("RegisteredMethodsMessageFilter");

        if (ok) {
            ok = registeredMethodsMessageFilter.IsValid();
        }

        if (ok) {
            registeredMethodsMessageFilter->SetDestination(this);
            ok = InstallMessageFilter(registeredMethodsMessageFilter);
        }

        return ok;

    }

    bool ElectricProbesGAM::Execute() {
        /* inputElectricOuter - inputElectricInner */
        *outputEpR =  (inputSignal[2] - inputOffset[2]) - 
            (inputSignal[1] - inputOffset[1]);
        *outputEpZ =  inputSignal[0] - inputOffset[0];
        //*outputEpZ =  inputSignal[0] - inputSignal[3];

        /* update the last value arrays */
        for (MARTe::uint32 i = 0u; i < numberOfInputElements; i++) {

            if (numberOfSamplesAvg > 2u) {
                for (MARTe::uint32 k = (numberOfSamplesAvg - 1u); k > 0u; k--) {
                    lastInputs[i][k] = lastInputs[i][k - 1];
                }
            }
            if (numberOfSamplesAvg > 1u) {
                lastInputs[i][0] = inputSignal[i];
            }
        }
        /*
        if (numberOfSamplesAvg > 1u) {
            lastInputs[0][0] = *inputElectricTop;
            lastInputs[1][0] = *inputElectricInner;
            lastInputs[2][0] = *inputElectricOuter;
            lastInputs[3][0] = *inputElectricBottom;
                                         
        lastInputs[i][0] = input[i][numberOfSamples - 1u];
        }
*/

        /* Should use a MARTe2 Message. 
         * Here for Sdas recorded signel with Trigger pseudo-signal
         * */
        if ((lastTriggerSdas == 0u) && (*triggerSdas == 1u)) {
            CalcOffSets();
        }
        lastTriggerSdas = *triggerSdas;
        return true;
    }

    bool ElectricProbesGAM::PrepareNextState(const char8 * const currentStateName,
            const char8 * const nextStateName) {
        bool ret = true;

        if (resetInEachState) {
            lastStateExecuted = nextStateName;
        }
/*
            bool cond1 = (stateVector.GetDataPointer() != NULL_PTR(float64 **));
            bool cond2 = (derivativeStateVector.GetDataPointer() != NULL_PTR(float64 **));
            if (cond1 && cond2) {
                for (uint32 i = 0u; i < sizeStateVector; i++) {
                    stateVector(i, 0u) = 0.0;
                    derivativeStateVector(i, 0u) = 0.0;
                }
            }
            else {
                REPORT_ERROR(ErrorManagement::ParametersError, "stateVector or derivativeStateVector = NULL ");
                ret = false;
            }

        }
        else {
            //If the currentStateName and lastStateExecuted are different-> rest values
            if (lastStateExecuted != currentStateName) {
                bool cond1 = (stateVector.GetDataPointer() != NULL_PTR(float64 **));
                bool cond2 = (derivativeStateVector.GetDataPointer() != NULL_PTR(float64 **));
                if (cond1 && cond2) {
                    for (uint32 i = 0u; i < sizeStateVector; i++) {
                        stateVector(i, 0u) = 0.0;
                        derivativeStateVector(i, 0u) = 0.0;
                    }
                }
                else {
                    REPORT_ERROR(ErrorManagement::ParametersError, "stateVector or derivativeStateVector = NULL ");
                    ret = false;
                }
            }
            lastStateExecuted = nextStateName;
        }
*/
        return ret;
    }


    bool ElectricProbesGAM::ExportData(MARTe::StructuredDataI & data) {
        using namespace MARTe;
        bool ok = GAM::ExportData(data);
        if (ok) {
            ok = data.CreateRelative("Parameters");
        }
        if (ok) {
            ok = data.Write("Gain", gain);
        }
        if (ok) {
            ok = data.MoveToAncestor(1u);
        }
        return ok;
    }

    ErrorManagement::ErrorType ElectricProbesGAM::CalcOffSets() {

        ErrorManagement::ErrorType ret = MARTe::ErrorManagement::NoError;
        REPORT_ERROR(ErrorManagement::Information, 
                        "CalcOffSets. Inputs:%f, %f, %f, %f.",
                        inputSignal[0],
                        inputSignal[1],
                        inputSignal[2],
                        inputSignal[3]);
        if (numberOfSamplesAvg > 1u) {
            for (uint32 i = 0u; i < EP_NUM_INPUTS; i++) {
                inputOffset[i] = 0.0f;
                for (uint32 k =  0 ; k < numberOfSamplesAvg; k++) {
                    inputOffset[i] += lastInputs[i][k];
                }
                inputOffset[i] /= numberOfSamplesAvg;
            }
            REPORT_ERROR(ErrorManagement::Information, 
                        "CalcOffSets. Offset:%f, %f, %f, %f.",
                        inputOffset[0],
                        inputOffset[1],
                        inputOffset[2],
                        inputOffset[3]);
        }

        return ret;
    }


    CLASS_REGISTER(ElectricProbesGAM, "1.1")

    CLASS_METHOD_REGISTER(ElectricProbesGAM, CalcOffSets)

} /* namespace MARTeIsttok */

//  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
