/**
 * @file AtcaIopConfig.cpp
 * @brief Source file for class AtcaIopConfig
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
 * the class AtcaIopConfig (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 *
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <fcntl.h>

#include <unistd.h> // for close()
//#include <math.h>

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "MemoryMapSynchronisedOutputBroker.h"

#include "AtcaIopConfig.h"
#include "atca-v6-iop-ioctl.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {
    const float32 DAC_RANGE = 20.0;
    const float32 ATCA_IOP_MAX_DAC_RANGE = 20.0;
    AtcaIopConfig::AtcaIopConfig() :
        DataSourceI(),
        MessageI() {
            devFileDescriptor = -1;
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
            eoWriteFlag = false;

            filter = ReferenceT<RegisteredMethodsMessageFilter>(GlobalObjectsDatabase::Instance()->GetStandardHeap());
            filter->SetDestination(this);
            ErrorManagement::ErrorType ret = MessageI::InstallMessageFilter(filter);
            if (!ret.ErrorsCleared()) {
                REPORT_ERROR(ErrorManagement::FatalError, "Failed to install message filters");
            }
        }

    /*lint -e{1551} the destructor must guarantee that the Timer SingleThreadService is stopped.*/
    AtcaIopConfig::~AtcaIopConfig() {
        if (devFileDescriptor != -1) {
            uint32 statusReg = 0;
            close(devFileDescriptor);
            REPORT_ERROR(ErrorManagement::Information, "Close device %d OK. Status Reg 0x%x,", devFileDescriptor, statusReg);
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

    bool AtcaIopConfig::AllocateMemory() {
        return true;
    }

    uint32 AtcaIopConfig::GetNumberOfMemoryBuffers() {
        return 1u;
    }

    /*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: The memory buffer is independent of the bufferIdx.*/
    bool AtcaIopConfig::GetSignalMemoryBuffer(const uint32 signalIdx, const uint32 bufferIdx, void*& signalAddress) {
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

    const char8* AtcaIopConfig::GetBrokerName(StructuredDataI& data, const SignalDirection direction) {
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

    bool AtcaIopConfig::GetInputBrokers(ReferenceContainer& inputBrokers, const char8* const functionName, void* const gamMemPtr) {
        return false;
    }

    bool AtcaIopConfig::GetOutputBrokers(ReferenceContainer& outputBrokers, const char8* const functionName, void* const gamMemPtr) {
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
    bool AtcaIopConfig::PrepareNextState(const char8* const currentStateName, const char8* const nextStateName) {
        return true;
    }

    bool AtcaIopConfig::Initialise(StructuredDataI& data) {
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


        return ok;
    }

    bool AtcaIopConfig::SetConfiguredDatabase(StructuredDataI& data) {
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
                //In member function ‘virtual bool MARTe::AtcaIopConfig::SetConfiguredDatabase(MARTe::StructuredDataI&)’:
                //AtcaIopConfig.cpp:311:49: error: no match for ‘operator==’ (operand types are ‘MARTe::BitRange<short unsigned int, 4, 2>’ and ‘const MARTe::TypeDescriptor’)
                ok = (GetSignalType(1) == Float32Bit);
                if (!ok) {
                    REPORT_ERROR(ErrorManagement::ParametersError, "WO signal shall be of type Float32Bit");
                }
            }
        }
        uint32 nOfFunctions = GetNumberOfFunctions();
        uint32 functionIdx;
        StreamString fullDeviceName;

        //Configure the board
        if (ok) {
            ok = fullDeviceName.Printf("%s_eo_%d", deviceName.Buffer(), boardId);
        }
        if (ok) {
            ok = fullDeviceName.Seek(0LLU);
        }
        if (ok) {
            devFileDescriptor = open(fullDeviceName.Buffer(), O_RDWR);
            ok = (devFileDescriptor > -1);
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


    bool AtcaIopConfig::Synchronise() {
        bool ok = true;
/*
#ifdef DEBUG_POLL
            if((synchCounter++)%4096 == 0) {
                //i = (synchCounter/4096) & 0xF;
                REPORT_ERROR(ErrorManagement::Information, "Synchronise eo0:%d wo0%0.3f", eoValues[0], woValues[0]);
            }
#endif
*/
        if (eoWriteFlag) {
            eoWriteFlag = false;
            ok =  IoWriteEoWo();
            // REPORT_ERROR(ErrorManagement::Information, "AtcaIopConfig::Synchronise. Do IoWriteEoWO!");
        }
        return ok;
    }

    ErrorManagement::ErrorType AtcaIopConfig::WriteEoWo() {
        ErrorManagement::ErrorType err;
        REPORT_ERROR(ErrorManagement::Information, "AtcaIopConfig::WriteEoWo. Got Message!");

        //ret = err.ErrorsCleared();
        eoWriteFlag = true;
        return err;
    }

    bool AtcaIopConfig::IoWriteEoWo() {
        bool ok = true;
        int rv;
        uint32 i;
        struct atca_eo_config eo_conf;
        struct atca_wo_config wo_conf;

        for (i=0u; i < ATCA_IOP_MAX_ADC_CHANNELS; i++) {
            eo_conf.offset[i] = eoValues[i];
            wo_conf.offset[i] = static_cast<int32>(woValues[i] * 65536);
        }
        rv = ioctl(devFileDescriptor, ATCA_PCIE_IOPS_EO_OFFSETS, &eo_conf);
        if (rv ) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Fail Write  eo0:%d", eoValues[0]);
            ok = false;
        }
        rv = ioctl(devFileDescriptor, ATCA_PCIE_IOPS_WO_OFFSETS, &wo_conf);
        if (rv ) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Fail Write  wo0:%6.3f", woValues[0]);
            ok = false;
        }
        rv = ioctl(devFileDescriptor, ATCA_PCIE_IOPT_RST_INTEG);
        if (rv ) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Fail reset Integrators");
            ok = false;
        }
        return ok;
    }

    CLASS_REGISTER(AtcaIopConfig, "1.0")
    CLASS_METHOD_REGISTER(AtcaIopConfig, WriteEoWo)

}
//  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
