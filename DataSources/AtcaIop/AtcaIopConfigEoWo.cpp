/**
 * @file AtcaIopConfigEoWo.cpp
 * @brief Source file for class AtcaIopConfigEoWo
 * @date 19/04/2024
 * @author Andre Neto / Bernardo Carvalho
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
 * the class AtcaIopConfigEoWo (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 *
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <fcntl.h>

#include <unistd.h> // for close()
#include <math.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "MemoryMapSynchronisedOutputBroker.h"
#include "AtcaIopConfigEoWo.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {
    const float32 DAC_RANGE = 20.0;
    const float32 ATCA_IOP_MAX_DAC_RANGE = 20.0;
    AtcaIopConfigEoWo::AtcaIopConfigEoWo() :
        DataSourceI(),
        MessageI() {
            boardFileDescriptor = -1;
            //numberOfDACsEnabled = 0u;
            //isMaster = 0u;
            deviceName = "";
            boardId = 0u;
            triggerSet = false;
            uint32 n;
            synchCounter = 0u;

            //channelsMemory = NULL_PTR(float32 *);
            eoValues = NULL_PTR(int32 *);
            woValues = NULL_PTR(float32 *);

            filter = ReferenceT<RegisteredMethodsMessageFilter>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
            filter->SetDestination(this);
            ErrorManagement::ErrorType ret = MessageI::InstallMessageFilter(filter);
            if (!ret.ErrorsCleared()) {
                REPORT_ERROR(ErrorManagement::FatalError, "Failed to install message filters");
            }
        }

    /*lint -e{1551} the destructor must guarantee that the Timer SingleThreadService is stopped.*/
    AtcaIopConfigEoWo::~AtcaIopConfigEoWo() {
        if (boardFileDescriptor != -1) {
            uint32 statusReg = 0;
            close(boardFileDescriptor);
            REPORT_ERROR(ErrorManagement::Information, "Close device %d OK. Status Reg 0x%x,", boardFileDescriptor, statusReg);
        }
        if (eoValues != NULL_PTR(int32 *)) {
            delete[] eoValues;
        }
        if (woValues != NULL_PTR(float32 *)) {
            delete[] woValues;
        }
        //if (channelsMemory != NULL_PTR(float32 *)) {
            //delete[] channelsMemory;
        //}
    }

    bool AtcaIopConfigEoWo::AllocateMemory() {
        return true;
    }

    uint32 AtcaIopConfigEoWo::GetNumberOfMemoryBuffers() {
        return 1u;
    }

    /*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: The memory buffer is independent of the bufferIdx.*/
    bool AtcaIopConfigEoWo::GetSignalMemoryBuffer(const uint32 signalIdx, const uint32 bufferIdx, void*& signalAddress) {
        //bool ok = (signalIdx < (ATCA_IOP_EOWO_N_SIGNALS));
        bool ok = true;
        if (signalIdx == 0u) {
            signalAddress = &eoValues[0];
        }
        else if (signalIdx == 1u) {
            signalAddress = woValues; //&counterAndTimer[1];
        }
        else {
            ok = false;
        }
        /*
        bool ok = (signalIdx < (ATCA_IOP_MAX_DAC_CHANNELS));
        if (ok) {
            if (channelsMemory != NULL_PTR(float32 *)) {
                signalAddress = &(channelsMemory[signalIdx]);
            }
        }
        */
        return ok;
    }

    const char8* AtcaIopConfigEoWo::GetBrokerName(StructuredDataI& data, const SignalDirection direction) {
        const char8 *brokerName = NULL_PTR(const char8 *);
        if (direction == OutputSignals) {
            uint32 trigger = 0u;
            if (!data.Read("Trigger", trigger)) {
                trigger = 0u;
            }

            if (trigger == 1u) {
                brokerName = "MemoryMapSynchronisedOutputBroker";
                triggerSet = true;
            }
            else {
                brokerName = "MemoryMapOutputBroker";
            }
        }
        else {
            REPORT_ERROR(ErrorManagement::ParametersError, "DataSource not compatible with InputSignals");
        }
        return brokerName;
    }

    bool AtcaIopConfigEoWo::GetInputBrokers(ReferenceContainer& inputBrokers, const char8* const functionName, void* const gamMemPtr) {
        return false;
    }

    bool AtcaIopConfigEoWo::GetOutputBrokers(ReferenceContainer& outputBrokers, const char8* const functionName, void* const gamMemPtr) {
        //Check if there is a Trigger signal for this function.
        uint32 functionIdx = 0u;
        uint32 nOfFunctionSignals = 0u;
        uint32 i;
        bool triggerGAM = false;
        bool ok = GetFunctionIndex(functionIdx, functionName);

        if (ok) {
            ok = GetFunctionNumberOfSignals(OutputSignals, functionIdx, nOfFunctionSignals);
        }
        uint32 trigger = 0u;
        for (i = 0u; (i < nOfFunctionSignals) && (ok) && (!triggerGAM); i++) {
            ok = GetFunctionSignalTrigger(OutputSignals, functionIdx, i, trigger);
            triggerGAM = (trigger == 1u);
        }
        if ((ok) && (triggerGAM)) {
            ReferenceT<MemoryMapSynchronisedOutputBroker> broker("MemoryMapSynchronisedOutputBroker");
            ok = broker.IsValid();

            if (ok) {
                ok = broker->Init(OutputSignals, *this, functionName, gamMemPtr);
            }
            if (ok) {
                ok = outputBrokers.Insert(broker);
            }
            //Must also add the signals which are not triggering but that belong to the same GAM...
            if (ok) {
                if (nOfFunctionSignals > 1u) {
                    ReferenceT<MemoryMapOutputBroker> brokerNotSync("MemoryMapOutputBroker");
                    ok = brokerNotSync.IsValid();
                    if (ok) {
                        ok = brokerNotSync->Init(OutputSignals, *this, functionName, gamMemPtr);
                    }
                    if (ok) {
                        ok = outputBrokers.Insert(brokerNotSync);
                    }
                }
            }
        }
        else {
            ReferenceT<MemoryMapOutputBroker> brokerNotSync("MemoryMapOutputBroker");
            ok = brokerNotSync.IsValid();
            if (ok) {
                ok = brokerNotSync->Init(OutputSignals, *this, functionName, gamMemPtr);
            }
            if (ok) {
                ok = outputBrokers.Insert(brokerNotSync);
            }
        }
        return ok;
    }

    /*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: the counter and the timer are always reset irrespectively of the states being changed.*/
    bool AtcaIopConfigEoWo::PrepareNextState(const char8* const currentStateName, const char8* const nextStateName) {
        REPORT_ERROR(ErrorManagement::Information, " EoWo currentStateName   %s, nextStateName %s", currentStateName, nextStateName);
        //if (currentStateName == "Idle") {
        //    REPORT_ERROR(ErrorManagement::Information, "OffState");
        //}
        return true;
    }

    bool AtcaIopConfigEoWo::Initialise(StructuredDataI& data) {
        bool ok = DataSourceI::Initialise(data);
        if (ok) {
            ok = data.Read("DeviceName", deviceName);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "The DeviceName shall be specified");
            }
        }
        if (ok) {
            ok = data.Read("BoardId", boardId);
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "The BoardId shall be specified");
            }
        }


        //Get individual signal parameters
        //uint32 i = 0u;
        /*
        if (ok) {
            ok = data.MoveRelative("Signals");
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "Could not move to the Signals section");
            }
            //Do not allow to add signals in run-time
            if (ok) {
                ok = signalsDatabase.MoveRelative("Signals");
            }
            if (ok) {
                ok = signalsDatabase.Write("Locked", 1u);
            }
            if (ok) {
                ok = signalsDatabase.MoveToAncestor(1u);
            }
            while ((i < ATCA_IOP_MAX_DAC_CHANNELS) && (ok)) {
                if (data.MoveRelative(data.GetChildName(i))) {
                    //uint32 channelId;
                    float32 range;
                    ok = data.Read("OutputRange", range);
                    if (ok) {
                        //if (data.Read("OutputRange", range)) {
                        ok = (range > 0.0) && (range <= ATCA_IOP_MAX_DAC_RANGE);
                        if (!ok) {
                            REPORT_ERROR(ErrorManagement::ParametersError, "Invalid OutputRange specified.");
                        }
                        if (ok) {
                            outputRange[i] = range;
                REPORT_ERROR(ErrorManagement::Information, " Parameter DAC %d  Output Range %f", i, range);
                            //dacEnabled[i] = true;
                            numberOfDACsEnabled++;
                        }
                    }
                        else {
                            REPORT_ERROR(ErrorManagement::ParametersError, "The OutputRange shall be specified.");
                        }
                        if (ok) {
                            ok = data.MoveToAncestor(1u);
                        }
                        i++;
                    }
                    else {
                        break;
                    }
                }
            }
//            REPORT_ERROR(ErrorManagement::Information, "numberOfDACsEnabled %d", numberOfDACsEnabled);
//
*/

            return ok;
        }

        bool AtcaIopConfigEoWo::SetConfiguredDatabase(StructuredDataI& data) {
            uint32 i;
            bool ok = DataSourceI::SetConfiguredDatabase(data);

            if (ok) {
                ok = triggerSet;
            }
            if (!ok) {
                REPORT_ERROR(ErrorManagement::ParametersError, "At least one Trigger signal shall be set.");
            }

            // Check the signal index 
            uint32 nOfSignals = GetNumberOfSignals();
            if (ok) {
                ok = (nOfSignals > 0u);
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError, "At least one signal shall be defined");
                }
            }
            if (ok) {
                ok = (GetSignalType(0).type == SignedInteger);
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError, "EO signal shall be of type SignedInteger");
                }
                else {
                    //In member function ‘virtual bool MARTe::AtcaIopConfigEoWo::SetConfiguredDatabase(MARTe::StructuredDataI&)’:
//AtcaIopConfigEoWo.cpp:311:49: error: no match for ‘operator==’ (operand types are ‘MARTe::BitRange<short unsigned int, 4, 2>’ and ‘const MARTe::TypeDescriptor’)
                    ok = (GetSignalType(1) == Float32Bit);
                    if (!ok) {
                        REPORT_ERROR(ErrorManagement::ParametersError, "WO signal shall be of type Float32Bit");
                    }
                }
            }
            uint32 nOfFunctions = GetNumberOfFunctions();
            uint32 functionIdx;
            /*
            //Check that the number of samples for all the signals is one
            for (functionIdx = 0u; (functionIdx < nOfFunctions) && (ok); functionIdx++) {

                for (i = 0u; (i < nOfSignals) && (ok); i++) {
                    uint32 nSamples = 0u;
                    ok = GetFunctionSignalSamples(OutputSignals, functionIdx, i, nSamples);
                    if (ok) {
                        ok = (nSamples == 1u);
                    }
                    if (!ok) {
                        REPORT_ERROR(ErrorManagement::ParametersError, "The number of samples shall be exactly one");
                    }
                }
            }
*/
            StreamString fullDeviceName;
            //Configure the board
            if (ok) {
                ok = fullDeviceName.Printf("%s_eo_%d", deviceName.Buffer(), boardId);
            }
            if (ok) {
                ok = fullDeviceName.Seek(0LLU);
            }
            if (ok) {
                boardFileDescriptor = open(fullDeviceName.Buffer(), O_RDWR);
                ok = (boardFileDescriptor > -1);
                if (!ok) {
                    REPORT_ERROR_PARAMETERS(ErrorManagement::ParametersError, "Could not open device %s", fullDeviceName);
                }
                else
                    REPORT_ERROR(ErrorManagement::Information, "Open device %s OK", fullDeviceName);
            }
            if (ok) {
                //Allocate memory
                eoValues = new int32[ATCA_IOP_MAX_ADC_CHANNELS];
                woValues = new float32[ATCA_IOP_MAX_ADC_CHANNELS];
            }

            return ok;
        }


        bool AtcaIopConfigEoWo::Synchronise() {
            uint32 i;
            int32 w = 1;
            bool ok = true;
#ifdef DEBUG_POLL
            if((synchCounter++)%4096 == 0) {
                //i = (synchCounter/4096) & 0xF;
                REPORT_ERROR(ErrorManagement::Information, "Synchronise eo0:%d wo0%0.3f", eoValues[0], woValues[0]);
            }
#endif
            //write(boardFileDescriptor,  &w, 4);
            /*
            if (channelsMemory != NULL_PTR(float32 *)) {

//                value = channelsMemory[0] / DAC_RANGE;
                for (i = 0u; (i < 2u) && (ok); i++) {
                    //for (i = 0u; (i < numberOfDACsEnabled ) && (ok); i++) {
                    float32 value = channelsMemory[i] / outputRange[i];
                    w = SetDacReg(i, value);
                    write(boardFileDescriptor,  &w, 4);
                    //                value = channelsMemory[1] / DAC_RANGE;
                    //value = channelsMemory[1] / DAC_RANGE * pow(2,17);
                    //                w = SetDacReg(1, value);
                    //w = 0x000FFFFF & static_cast<uint32>(value);
                    //              write(boardFileDescriptor,  &w, 4);
                    //REPORT_ERROR(ErrorManagement::Information, " Writing DAC 0 0x%x", w);
                }
            }

               w = dacValues[i];
               }
               */
            return ok;
        }

        int32 AtcaIopConfigEoWo::SetDacReg(uint32 channel, float32 val) const {
            if (val > 1.0)
                val = 1.0;
            if (val < -1.0)
                val = -1.0;
            int32 dacReg = static_cast<int32>(val * pow(2,17));
            if (dacReg > 0x1FFFF)  // 131071
                dacReg = 0x1FFFF;
            if (dacReg < -131072)  // -0x20000
                dacReg = -131072;
            dacReg &= 0x0003FFFF; // keep 18 lsb
            dacReg |= (0xF & channel) << 28;
            return dacReg;
        }

        CLASS_REGISTER(AtcaIopConfigEoWo, "1.0")
    }
    //  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
