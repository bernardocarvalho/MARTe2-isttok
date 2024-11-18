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
#include "ElectricProbesGAM.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTeIsttok {
    ElectricProbesGAM::ElectricProbesGAM() : 
                GAM(),
                MessageI() {
        gain = 0u;
        inputSignals = NULL_PTR(MARTe::float32 **);
        outputSignals = NULL_PTR(MARTe::float32 **);
        outputSignal1 = NULL;
    }

    ElectricProbesGAM::~ElectricProbesGAM() {
        if (inputSignals != NULL_PTR(MARTe::float32 **)) {
            delete[] inputSignals;
        }
        if (outputSignals != NULL_PTR(MARTe::float32 **)) {
            delete[] outputSignals;
        }
        outputSignal1 = NULL;
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
        return ok;
    }

    bool ElectricProbesGAM::Setup() {
        using namespace MARTe;
        uint32 numberOfInputSignals = GetNumberOfInputSignals();
        uint32 numberOfOutputSignals = GetNumberOfOutputSignals();
        bool ok = (numberOfInputSignals == 4u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of input signals shall be equal to 4. numberOfInputSignals = %d ", numberOfInputSignals);
        }
        if (ok) {
            inputSignals = new float32*[numberOfInputSignals];
        }
        if (ok) {
            ok = (numberOfOutputSignals == 1u);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError,
                        "The number of output signals shall be equal to 1. numberOfOutputSignals = %d", numberOfOutputSignals);
            }
        }

        if (ok) {
            uint32 n;
            for (n = 0u; (n < numberOfInputSignals) && (ok); n++) {
                StreamString inputSignalName;
                ok = GetSignalName(InputSignals, n, inputSignalName);
                TypeDescriptor inputSignalType = GetSignalType(InputSignals, n);
                ok = (inputSignalType == Float32Bit);
                if (!ok) {
                    const char8 * const inputSignalTypeStr = TypeDescriptor::GetTypeNameFromTypeDescriptor(inputSignalType);
                    REPORT_ERROR(ErrorManagement::ParametersError,
                            "The type of the input signals shall be float32. inputSignalType = %s", inputSignalTypeStr);
                }
                uint32 numberOfInputSamples = 0u;
                if (ok) {
                    ok = GetSignalNumberOfSamples(InputSignals, n, numberOfInputSamples);
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
                    ok = GetSignalNumberOfDimensions(InputSignals, n, numberOfInputDimensions);
                }

                if (ok) {
                    ok = (numberOfInputDimensions == 0u);
                    if (!ok) {
                        REPORT_ERROR(
                                ErrorManagement::ParametersError,
                                "The number of input signals dimensions shall be equal to 0. numberOfInputDimensions(%s) = %d", inputSignalName.Buffer(), numberOfInputDimensions);
                    }
                }
                uint32 numberOfInputElements = 0u;
                if (ok) {
                    ok = GetSignalNumberOfElements(InputSignals, n, numberOfInputElements);
                }
                if (ok) {
                    ok = (numberOfInputElements == 1u);
                }
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError,
                            "The number of input signal elements shall be equal to 1. numberOfInputElements(%s) = %d", inputSignalName.Buffer(), numberOfInputElements);
                }

                if (ok) {
                     inputSignals[n] = reinterpret_cast<float32 *>(GetInputSignalMemory(n));
                }


            }
        }
        ok = (numberOfOutputSignals == 2u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The number of output signals shall be equal to 2. numberOfOutputSignals = %d ", numberOfOutputSignals);
        }
        if (ok) {
            outputSignals = new float32*[numberOfOutputSignals];
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
                        REPORT_ERROR(
                                ErrorManagement::ParametersError,
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
                outputSignals[0] = reinterpret_cast<float32 *>(GetOutputSignalMemory(0));
                outputSignal1 = reinterpret_cast<float32 *>(GetOutputSignalMemory(1));
            }
        }
        return ok;

    }

    bool ElectricProbesGAM::Execute() {
        //*outputSignal = *inputSignal;
        //*outputSignal1 = *inputSignals[0] - *inputSignals[1];
         return true;
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

    CLASS_REGISTER(ElectricProbesGAM, "1.0")
    //CLASS_METHOD_REGISTER(AtcaIopConfig, WriteEoWo)

}

//  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
