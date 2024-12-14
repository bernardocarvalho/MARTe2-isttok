/**
 * @file AtcaIopADC.cpp
 * @brief Source file for class AtcaIopADC
 * @date 19/09/2020
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
 * the class AtcaIopADC (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

//#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
//#include <math.h>
#include <fcntl.h>
#include <sys/mman.h>  // For mmap()

#include <unistd.h> // for close()

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "MemoryMapSynchronisedInputBroker.h"
#include "AtcaIopADC.h"
#include "atca-v6-iop-ioctl.h"

#define DEBUG_POLL 32768
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {

    const uint32 POLL_EXTRA_WAIT = 50000u; // 50 us 
    //const float64 ADC_SIMULATOR_PI = 3.14159265359;
    const uint32 IOP_ADC_OFFSET = 2u; // in DMA Data packet in 32b. First 2 are counter.
    const uint32 IOP_ADC_INTEG_OFFSET = 16u;  // in 64 bit words

    const uint32 RT_PCKT_SIZE = 1024u;
    const uint32 RT_PCKT64_SIZE = 512u; // In 64 bit words

    /* 256 + 3840 = 4096 B (PAGE_SIZE) data packet
       typedef struct _DMA_CH1_PCKT {
       uint32 head_time_cnt;
       uint32 header; // h5431BACD
       int32 channel[60]; // 24 56
       uint32 foot_time_cnt;
       uint32 footer; // h9876ABDC
       uint8 page_fill[3840];
       } DMA_CH1_PCKT;
       */
#define DMA_RT_PCKT_SIZE 256
    /* 256 Bytes RT  data packet */ 
    typedef struct _DMA_CH1_PCKT {
        volatile uint32_t head_time_cnt;
        volatile uint32_t header; // h5431BACD
        volatile int32_t adc_decim_data[ADC_CHANNELS];
        volatile uint64_t _fill64_0;  // 00
        volatile int64_t adc_integ_data[ADC_CHANNELS];
        volatile int64_t _fill64_1[4]; // 00
        volatile uint32_t sample_cnt;
        volatile uint32_t _fill32; // hAAAABBBB
        volatile uint32_t foot_time_cnt;
        volatile uint32_t footer; // h9876ABDC
    } DMA_CH1_PCKT;

    struct atca_eo_config {
        int32_t offset[ATCA_IOP_N_ADCs];
    };

    struct atca_wo_config {
        int32_t offset[ATCA_IOP_N_ADCs];
    };

    //}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
AtcaIopADC::AtcaIopADC() :
    DataSourceI(), MessageI(), EmbeddedServiceMethodBinderI(), executor(*this) {
        deviceName = "";
        boardId = 2u;
        //deviceDmaName = "";
        boardFileDescriptor = -1;
        boardDmaDescriptor = -1;
        mappedDmaBase = NULL;
        mappedDmaSize = 0u;
        isMaster = 0u;
        oldestBufferIdx = 0u;
        lastTimeTicks = 0u;
        sleepTimeTicks = 0u;
        timerPeriodUsecTime = 0u;
        counterAndTimer[0] = 0u;
        counterAndTimer[1] = 0u;
        timeoutCount = 0u;
        timeoutMax = 0u;
        sleepNature = Busy;
        sleepPercentage = 0u;
        //execCounter = 0u;
        //pollTimout = 0u;
        adcPeriod = 0.;
        uint32 k;
        adcValues = NULL_PTR(int32 *);
        for (k=0u; k < ATCA_IOP_N_ADCs; k++) {
            electricalOffsets[k] = 0u;
            wiringOffsets[k] = 0.0;
        }
        for (k=0u; k < ATCA_IOP_N_INTEGRALS; k++) {
            adcIntegralValues[k] = 0u;
        }

        if (!synchSem.Create()) {
            REPORT_ERROR(ErrorManagement::FatalError, "Could not create EventSem.");
        }
    }

/*lint -e{1551} the destructor must guarantee that the Timer SingleThreadService is stopped.*/
AtcaIopADC::~AtcaIopADC() {
    if (boardFileDescriptor != -1) {
        // Synchronize DMA before accessing board registers
        oldestBufferIdx = GetOldestBufferIdx();
        PollDmaBuff(2000000u); //  wait max 2ms
        ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_STREAM_DISABLE);
        ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_DMA_DISABLE);
        uint32 statusReg = 0;
        ioctl(boardFileDescriptor, ATCA_PCIE_IOPG_STATUS, &statusReg);
        //rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_DMA_RESET);
        close(boardFileDescriptor);
        REPORT_ERROR(ErrorManagement::Information, "Close device %d OK. Status Reg 0x%x,", boardFileDescriptor, statusReg);
    }
    if (!synchSem.Post()) {
        REPORT_ERROR(ErrorManagement::Warning, "Could not post EventSem.");
    }
    if (!executor.Stop()) {
        if (!executor.Stop()) {
            REPORT_ERROR(ErrorManagement::Warning, "Could not stop SingleThreadService.");
        }
    }
    // some waiting..
    //float32 sleepTime = static_cast<float32>(static_cast<float64>(1000) * HighResolutionTimer::Period());
    //Sleep::NoMore(sleepTime);
    if (boardDmaDescriptor != -1) {
        close(boardDmaDescriptor);
        REPORT_ERROR(ErrorManagement::Information, "Close device %d OK", boardDmaDescriptor);
    }
    adcValues = NULL_PTR(int32 *);
    //uint32 k;
}

bool AtcaIopADC::AllocateMemory() {
    return true;
}

bool AtcaIopADC::Initialise(StructuredDataI& data) {
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
    numberOfChannels = 0u;
    if (ok) {
        ok = data.Read("NumberOfChannels", numberOfChannels);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The NumberOfChannels shall be specified");
        }
        if (numberOfChannels > ATCA_IOP_N_ADCs ) {
            numberOfChannels = ATCA_IOP_N_ADCs;
        }
    }
    if (ok) {
        ok = data.Read("RTDecimation", rtDecimation);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The RTDecimation shall be specified");
        }
        if (rtDecimation > 2000000u) {
            rtDecimation = 2000000u;
        }
    }
    StreamString sleepNatureStr;
    if (!data.Read("SleepNature", sleepNatureStr)) {
        REPORT_ERROR(ErrorManagement::Information, "SleepNature was not set. Using Default.");
        sleepNatureStr = "Default";
    }
    if (sleepNatureStr == "Default") {
        sleepNature = Default;
    }
    else if (sleepNatureStr == "Busy") {
        sleepNature = Busy;
        if (!data.Read("SleepPercentage", sleepPercentage)) {
            sleepPercentage = 0u;
            REPORT_ERROR(ErrorManagement::Information, "SleepPercentage was not set. Using Default %d.", sleepPercentage);
        }
        if (sleepPercentage > 100u) {
            sleepPercentage = 100u;
        }
    }
    else {
        REPORT_ERROR(ErrorManagement::ParametersError, "Unsupported SleepNature.");
        ok = false;
    }


    AnyType arrayDescription = data.GetType("ElectricalOffsets");
    uint32 numberOfElements = 0u;
    if (ok) {
        ok = arrayDescription.GetDataPointer() != NULL_PTR(void *);
    }
    if (ok) {
        numberOfElements = arrayDescription.GetNumberOfElements(0u);
        ok = (numberOfElements == numberOfChannels);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Exactly %d elements shall be defined in the array ElectricalOffsets", numberOfChannels);
        }
    }
    if (ok) {
        Vector<int32> readVector(&electricalOffsets[0u], numberOfElements);
        ok = data.Read("ElectricalOffsets", readVector);
    }

    arrayDescription = data.GetType("WiringOffsets");
    numberOfElements = 0u;
    if (ok) {
        ok = arrayDescription.GetDataPointer() != NULL_PTR(void *);
    }
    if (ok) {
        numberOfElements = arrayDescription.GetNumberOfElements(0u);
        ok = (numberOfElements == numberOfChannels);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "Exactly %d elements shall be defined in the array WiringOffsets", numberOfChannels);
        }
    }
    if (ok) {
        Vector<float32> readVector(&wiringOffsets[0u], numberOfElements);
        ok = data.Read("WiringOffsets", readVector);
    }

    chopperPeriod = 1000;
    if (!data.Read("ChopperPeriod", chopperPeriod)) {
        REPORT_ERROR(ErrorManagement::Warning, "ChopperPeriod not specified. Using default: %d", chopperPeriod);
    }

    if (!data.Read("IsMaster", isMaster)) {
        REPORT_ERROR(ErrorManagement::Warning, "IsMaster not specified. Using default: %d", isMaster);
    }

    adcFrequency = 2e5;
    if (!data.Read("ADCFrequency", adcFrequency)) {
        REPORT_ERROR(ErrorManagement::Warning, "ADCFrequency not specified. Using default: %d", adcFrequency);
    }
    adcPeriod = 1./adcFrequency;

    if (ok) {
        uint32 cpuMaskIn;
        if (!data.Read("CPUMask", cpuMaskIn)) {
            cpuMaskIn = 0xFFu;
            REPORT_ERROR(ErrorManagement::Warning, "CPUMask not specified using: %d", cpuMaskIn);
        }
        cpuMask = cpuMaskIn;
    }
    if (ok) {
        if (!data.Read("StackSize", stackSize)) {
            stackSize = THREADS_DEFAULT_STACKSIZE;
            REPORT_ERROR(ErrorManagement::Warning, "StackSize not specified using: %d", stackSize);
        }
    }
    if (ok) {
        ok = (stackSize > 0u);

        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "StackSize shall be > 0u");
        }
    }
    if (ok) {
        executor.SetCPUMask(cpuMask);
        executor.SetStackSize(stackSize);
    }
    return ok;
}

bool AtcaIopADC::SetConfiguredDatabase(StructuredDataI& data) {
    bool ok = DataSourceI::SetConfiguredDatabase(data);
    struct atca_eo_config eo_conf;
    struct atca_wo_config wo_conf;
    //The DataSource shall have N signals. 
    //
    if (ok) {
        ok = (GetNumberOfSignals() == ATCA_IOP_N_SIGNALS);
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "Exactly %d signals shall be configured", ATCA_IOP_N_SIGNALS);
    }
    uint32 i, startPos, endPos;
    startPos = 0u;
    endPos = ATCA_IOP_N_TIMCNT;
    for (i=startPos; (i < endPos) && (ok); i++) {
        ok = (GetSignalType(i).type == UnsignedInteger);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The signal %d shall be of type UnsignedInteger", i);
        }
        ok = (GetSignalType(i).numberOfBits == 32u);
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "The signal in position %d shall have 32 bits and %d were specified", i, uint16(GetSignalType(i).numberOfBits));
        }
    }
    ok = (GetSignalType(4u).type == SignedInteger);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The signal 4 shall be of type SignedInteger");
    }
    ok = (GetSignalType(4u).numberOfBits == 32u);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The signal in position %d shall have 32 bits and %d were specified", i, uint16(GetSignalType(i).numberOfBits));
    }
    uint8 nDimensions = 0u;
    uint32 nElements = 0u;
    ok = GetSignalNumberOfDimensions(4u, nDimensions);
    if (ok) {
        ok = GetSignalNumberOfElements(4u, nElements);
        REPORT_ERROR(ErrorManagement::Information, "The signal 4 shall has %d Elements", nElements);
    }
    startPos = endPos;
    endPos = startPos + ATCA_IOP_N_ADCs;
    if (ok) {
        ok = (GetSignalType(5u).type == SignedInteger);
    }
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The signal 4 shall be of type SignedInteger");
    }
    ok = (GetSignalType(5u).numberOfBits == 64u);
    if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError, "The signal in position 5 shall have 64 bits and %d were specified", uint16(GetSignalType(i).numberOfBits));
    }
    nDimensions = 0u;
    nElements = 0u;
    if (!ok) {
        ok = GetSignalNumberOfDimensions(5u, nDimensions);
    }
    if (ok) {
        ok = GetSignalNumberOfElements(5u, nElements);
    }
    StreamString fullDeviceName;
    //Configure the board
    if (ok) {
        ok = fullDeviceName.Printf("%s_%d", deviceName.Buffer(), boardId);
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
    ok = fullDeviceName.Seek(0LLU);
    if (ok) {
        ok = fullDeviceName.Printf("%s_dmart_%d", deviceName.Buffer(), boardId);
    }
    if (ok) {
        ok = fullDeviceName.Seek(0LLU);
    }
    if (ok) {
        boardDmaDescriptor = open(fullDeviceName.Buffer(), O_RDWR);
        ok = (boardDmaDescriptor > -1);
        if (!ok) {
            REPORT_ERROR_PARAMETERS(ErrorManagement::ParametersError, "Could not open DMA device %s", fullDeviceName);
        }
        else
            REPORT_ERROR(ErrorManagement::Information, "Open DMA device %s OK", fullDeviceName);
    }

    //mappedDmaBase = (int32 *) mmap(0,  getpagesize() * NUMBER_OF_BUFFERS,
    mappedDmaSize = getpagesize();
    mappedDmaBase =  mmap(0,  mappedDmaSize, //4096u,  // * NUMBER_OF_BUFFERS,
            PROT_READ, MAP_SHARED, boardDmaDescriptor, 0);
    if (mappedDmaBase == MAP_FAILED){
        ok = false;
        REPORT_ERROR(ErrorManagement::FatalError, "Error Mapping DMA Memory Device %s", fullDeviceName);
    }

    uint32 statusReg = 0;
    int rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_DMA_RESET);
    //rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_STREAM_DISABLE);
    if (chopperPeriod > 0)
    {
        uint32 chopCounter = (chopperPeriod) << 16;
        chopCounter &= 0xFFFF0000;
        chopCounter |= chopperPeriod / 2;

        rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPS_CHOP_COUNTERS, &chopCounter);
        //read chopper
        chopCounter = 0;
        rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPG_CHOP_COUNTERS, &chopCounter);

        rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_CHOP_ON);
        REPORT_ERROR(ErrorManagement::Information, "Chop ON 0x%x", chopCounter);
    }
    else
        rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_CHOP_OFF);

    for (i=0u; i < ATCA_IOP_N_ADCs ; i++) {
        eo_conf.offset[i] = 0u;
        wo_conf.offset[i] = 0.0;
    }
    for (i=0u; i < ATCA_IOP_N_ADCs ; i++) {
        eo_conf.offset[i] = electricalOffsets[i];
        wo_conf.offset[i] = static_cast<int32>(wiringOffsets[i] * 65536);
       // REPORT_ERROR(ErrorManagement::Information, "WiringOffset %d: %d", i, wo_conf.offset[i]);
    }
    rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPS_EO_OFFSETS, &eo_conf);
    rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPS_WO_OFFSETS, &wo_conf);

    rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPG_STATUS, &statusReg);
    if (rc) {
        ok = false;
        REPORT_ERROR(ErrorManagement::FatalError, "Device Status Reg %d, 0x%x", rc, statusReg);
    }
    // rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_ACQ_ENABLE);
    //rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_DMA_DISABLE);
    //Sleep::Busy(0.001); // in Sec
    //rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_ACQ_DISABLE);

    rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_IRQ_DISABLE);
    rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPT_STREAM_ENABLE);
    //rc = ioctl(boardFileDescriptor, ATCA_PCIE_IOPG_STATUS, &statusReg);
    Sleep::Busy(0.0011); // in Sec
    int32 currentDMA = 0u;// = CurrentBufferIndex(200);
    for (i = 0u; i < 1; i++) {
        oldestBufferIdx = GetOldestBufferIdx();
        currentDMA = PollDmaBuff(2000000u); //  wait max 2ms
        REPORT_ERROR(ErrorManagement::Information, "AtcaIopADC::CurrentBufferIndex: %d, Idx: %d", currentDMA, oldestBufferIdx);
    }
    REPORT_ERROR(ErrorManagement::Information, "AtcaIopADC::CurrentBufferIndex: %d", currentDMA);
    //REPORT_ERROR(ErrorManagement::Warning, "SleepTime %d, count:0x%x", sleepTime, pdma[3].head_time_cnt);
    uint32 nOfFunctions = GetNumberOfFunctions();
    float32 cycleFrequency = -1.0F;
    bool frequencyFound = false;
    uint32 functionIdx;

    //How many GAMs (functions) are interacting with this DataSource?
    for (functionIdx = 0u; (functionIdx < nOfFunctions) && (ok); functionIdx++) {
        uint32 nOfSignals = 0u;
        ok = GetFunctionNumberOfSignals(InputSignals, functionIdx, nOfSignals);
        uint32 i;
        for (i = 0u; (i < nOfSignals) && (ok); i++) {
            if (!frequencyFound) {
                ok = GetFunctionSignalReadFrequency(InputSignals, functionIdx, i, cycleFrequency);
                //Found a GAM that wants to synchronise on this DataSourceI. We need to have one and exactly one GAM asking to synchronise in this DataSource.
                frequencyFound = (cycleFrequency > 0.F);
            }
            bool isCounter = false;
            bool isTime = false;
            bool isAdc = false;
            //            bool isAdcDecim = false;
            //bool isAdcSim = false;
            uint32 signalIdx = 0u;
            uint32 nSamples = 0u;
            uint32 nElements = 0u;

            ok = GetFunctionSignalSamples(InputSignals, functionIdx, i, nSamples);

            //Is the counter or the time signal?
            StreamString signalAlias;
            if (ok) {
                ok = GetFunctionSignalAlias(InputSignals, functionIdx, i, signalAlias);
            }
            if (ok) {
                ok = GetSignalIndex(signalIdx, signalAlias.Buffer());
            }
            if (ok) {
                isCounter = (signalIdx == 0u);
                isTime = (signalIdx == 1u);
                isAdc = (signalIdx < ATCA_IOP_N_TIMCNT + ATCA_IOP_N_ADCs + ATCA_IOP_N_INTEGRALS);
                if (isCounter) {
                    if (nSamples > 1u) {
                        ok = false;
                        REPORT_ERROR(ErrorManagement::ParametersError, "The first signal (counter) shall have one and only one sample");
                    }
                }
                else if (isTime) {
                    if (nSamples > 1u) {
                        ok = false;
                        REPORT_ERROR(ErrorManagement::ParametersError, "The second signal (time) shall have one and only one sample");
                    }
                }
                /*
                   else if (isAdcDecim) {
                   ok = GetSignalNumberOfElements(signalIdx, nElements);
                   REPORT_ERROR(ErrorManagement::Information, "The ADC decim Signal Elements %d", nElements);
                // if (nSamples > 1u) {
                //    ok = false;
                //    REPORT_ERROR(ErrorManagement::ParametersError, "The second signal (time) shall have one and only one sample");
                //}
                }
                */
                else if (isAdc) {
                    if (nSamples > 1u) {
                        ok = false;
                        REPORT_ERROR(ErrorManagement::ParametersError, "The ATCA-IOP signals shall have one and only one sample");
                    }
                }
                else {}
            }
        }
    }
    if (ok) {
        ok = frequencyFound;
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError, "No frequency > 0 was set (i.e. no signal synchronises on this AtcaIopADC).");
        }
    }
    if (ok) {
        REPORT_ERROR(ErrorManagement::Information, "The timer will be set using a cycle frequency of %f Hz", cycleFrequency);

        float64 periodUsec = (1e6 / cycleFrequency);
        timerPeriodUsecTime = static_cast<uint32>(periodUsec);
        float64 sleepTimeT = (static_cast<float64>(HighResolutionTimer::Frequency()) / cycleFrequency);
        sleepTimeTicks = static_cast<uint64>(sleepTimeT);
        /*
         * [Information - AtcaIopADC.cpp:548]: The timer will be set using a cycle frequency of 10000.000000 Hz
         * [Information - AtcaIopADC.cpp:554]: The timer will be set using a sleepTimeTicks of 100000 (ns)
         */
        REPORT_ERROR(ErrorManagement::Information,
                "The timer will be set using a sleepTimeTicks of %d (ns)", sleepTimeTicks);
    }
    if (ok) {
        //float32 totalNumberOfSamplesPerSecond = (static_cast<float32>(adcSamplesPerCycle) * cycleFrequency);
        float32 totalNumberOfSamplesPerSecond = (static_cast<float32>(rtDecimation) * cycleFrequency);
        ok = (adcFrequency == static_cast<uint32>(totalNumberOfSamplesPerSecond));
        if (!ok) {
            REPORT_ERROR(ErrorManagement::ParametersError,
                    "The adcSamplesPerCycle * cycleFrequency (%u) shall be equal to the ADCs acquisition frequency (%u)",
                    totalNumberOfSamplesPerSecond, adcFrequency);
        }
    }
    if (ok) {
        adcValues = new int32[numberOfChannels];
    }
    return ok;
}

uint32 AtcaIopADC::GetNumberOfMemoryBuffers() {
    return 1u;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: The memory buffer is independent of the bufferIdx.*/
bool AtcaIopADC::GetSignalMemoryBuffer(const uint32 signalIdx, const uint32 bufferIdx, void*& signalAddress) {
    bool ok = true;
    if (signalIdx == 0u) {
        signalAddress = &counterAndTimer[0];
    }
    else if (signalIdx == 1u) {
        signalAddress = &counterAndTimer[1];
    }
    else if (signalIdx == 2u) {
        signalAddress = &timeoutCount;
    }
    else if (signalIdx == 3u) {
        signalAddress = &timeoutMax;
    }
    else if (signalIdx == 4u) {
        signalAddress = &adcValues[0]; //signalIdx - ATCA_IOP_N_TIMCNT]cwtimeoutMax;
    }
    else if (signalIdx == 5u) {
        signalAddress = &adcIntegralValues[0]; //signalIdx - ATCA_IOP_N_TIMCNT]cwtimeoutMax;
    }
    /*
    else if (signalIdx < ATCA_IOP_N_TIMCNT + ATCA_IOP_N_ADCs ) {
        signalAddress = &adcValues[signalIdx - ATCA_IOP_N_TIMCNT];
    }
    else if (signalIdx < ATCA_IOP_N_TIMCNT + ATCA_IOP_N_ADCs + ATCA_IOP_N_INTEGRALS) {
        signalAddress = &adcIntegralValues[signalIdx -
            (ATCA_IOP_N_TIMCNT + ATCA_IOP_N_ADCs)];
    }
    */
    else {
        ok = false;
    }
    return ok;
}

const char8* AtcaIopADC::GetBrokerName(StructuredDataI& data, const SignalDirection direction) {
    const char8 *brokerName = NULL_PTR(const char8 *);
    if (direction == InputSignals) {
        float32 frequency = 0.F;
        if (!data.Read("Frequency", frequency)) {
            frequency = -1.F;
        }
        if (frequency > 0.F) {
            brokerName = "MemoryMapSynchronisedInputBroker";
        }
        else {
            brokerName = "MemoryMapInputBroker";
        }
    }
    else {
        REPORT_ERROR(ErrorManagement::ParametersError, "DataSource not compatible with OutputSignals");
    }
    return brokerName;
}

bool AtcaIopADC::Synchronise() {
    ErrorManagement::ErrorType err;
    err = synchSem.ResetWait(TTInfiniteWait);
    return err.ErrorsCleared();
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: the counter and the timer are always reset irrespectively of the states being changed.*/
bool AtcaIopADC::PrepareNextState(const char8* const currentStateName, const char8* const nextStateName) {
    bool ok = true;
    if (executor.GetStatus() == EmbeddedThreadI::OffState) {
        ok = executor.Start();
        REPORT_ERROR(ErrorManagement::Information, "Executor Start");
    }
    /*
    if (executor.GetStatus() == EmbeddedThreadI::RunningState) {
        REPORT_ERROR(ErrorManagement::Information, " ADC currentStateName   %s, nextStateName %s", currentStateName, nextStateName);
    }
    */
    counterAndTimer[0] = 0u;
    //counterAndTimer[1] = 0u;
    return ok;
}

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification: the method sleeps for the given period irrespectively of the input info.*/
ErrorManagement::ErrorType AtcaIopADC::Execute(ExecutionInfo& info) {
    DMA_CH1_PCKT *pdma = (DMA_CH1_PCKT *) mappedDmaBase;
    if (lastTimeTicks == 0u) {
        lastTimeTicks = HighResolutionTimer::Counter();
    }

    uint64 startTicks = HighResolutionTimer::Counter();
    //If we lose cycle, rephase to a multiple of the period.
    uint32 nCycles = 0u;
    while (lastTimeTicks < startTicks) {
        lastTimeTicks += sleepTimeTicks;
        nCycles++;
    }
    lastTimeTicks -= sleepTimeTicks;

    //Sleep until the next period. Cannot be < 0 due to while(lastTimeTicks < startTicks) above
    uint64 sleepTicksCorrection = (startTicks - lastTimeTicks);
    //uint64 deltaTicks = sleepTimeTicks - (startTicks - lastTimeTicks);
    uint64 deltaTicks = sleepTimeTicks - sleepTicksCorrection;

    timeoutMax = deltaTicks; // debug
    //volatile int32 currentDMA = 0u;
    oldestBufferIdx = GetOldestBufferIdx();
    float32 totalSleepTime = static_cast<float32>(static_cast<float64>(deltaTicks) * HighResolutionTimer::Period());

    if (sleepNature == Busy) {
        if (sleepPercentage > 0u) {
            //float32 sleepTime = totalSleepTime * 0.5;
            float32 sleepTime = totalSleepTime * (static_cast<float32>(sleepPercentage) / 100.F);
            Sleep::NoMore(sleepTime);
            //if(PollDmaBuff(startTicks + deltaTicks + 100000u) < 0)
        }
        if(PollDmaBuff(deltaTicks + POLL_EXTRA_WAIT) < 0){
            //pollTimout++; // TODO check max wait
            timeoutCount++;             //
        }
        /*
           else {
           float32 totalSleepTime = static_cast<float32>(static_cast<float64>(deltaTicks) * HighResolutionTimer::Period());
           uint32 busyPercentage = (100u - sleepPercentage);
           float32 busyTime = totalSleepTime * (static_cast<float32>(busyPercentage) / 100.F);
           Sleep::SemiBusy(totalSleepTime, busyTime);
           }
           */
    }
    else {
        float32 sleepTime = static_cast<float32>(static_cast<float64>(deltaTicks) * HighResolutionTimer::Period());
        Sleep::NoMore(sleepTime);
    }
    lastTimeTicks = HighResolutionTimer::Counter();

    ErrorManagement::ErrorType err = synchSem.Post();
    counterAndTimer[0] += nCycles;
    //counterAndTimer[1] = mappedDmaBase[oldestBufferIdx * RT_PCKT_SIZE] * timerPeriodUsecTime;
    counterAndTimer[1] = pdma[oldestBufferIdx].head_time_cnt * timerPeriodUsecTime;
    // Get adc data from DMA packet
    uint32 k;
    uint32 s;
    for (k=0u; k < ATCA_IOP_N_ADCs ; k++) {
        //adcValues[k] = (mappedDmaBase[oldestBufferIdx * RT_PCKT_SIZE +
        //        IOP_ADC_OFFSET + k] ) / (1<<14);
        adcValues[k] = pdma[oldestBufferIdx].adc_decim_data[k] / (1<<14);
    }
    //int64 * mappedDmaBase64 = (int64 *) mappedDmaBase;
    for (k=0u; k < ATCA_IOP_N_INTEGRALS ; k++) {
        adcIntegralValues[k] = pdma[oldestBufferIdx].adc_integ_data[k];
        //mappedDmaBase64[oldestBufferIdx * RT_PCKT64_SIZE + IOP_ADC_INTEG_OFFSET + k];
    }

    float64 t = counterAndTimer[1];
    t /= 1e6;
    // Compute simulated Sinus Signals
    return err;
}

const ProcessorType& AtcaIopADC::GetCPUMask() const {
    return cpuMask;
}

uint32 AtcaIopADC::GetStackSize() const {
    return stackSize;
}

uint32 AtcaIopADC::GetSleepPercentage() const {
    return sleepPercentage;
}
/*
 * waitLimitTicks in ns
 * */

int32 AtcaIopADC::PollDmaBuff(uint64 maxWaitTicks) const {

    DMA_CH1_PCKT *pdma = (DMA_CH1_PCKT *) mappedDmaBase;
    uint32  oldBufferFooter = pdma[oldestBufferIdx].foot_time_cnt;
    volatile uint32  freshBufferFooter = oldBufferFooter;
    uint64 actualTime = HighResolutionTimer::Counter();
    uint64 waitLimitTicks = actualTime + maxWaitTicks;
    while (freshBufferFooter == oldBufferFooter) {
        if(actualTime > waitLimitTicks) {
            return -1; // Timeout
        }
        actualTime = HighResolutionTimer::Counter();
        freshBufferFooter = pdma[oldestBufferIdx].foot_time_cnt;
    }
    //uint32 headTimeMark = mappedDmaBase[buffIdx * RT_PCKT_SIZE];
    uint32 headTimeMark = pdma[oldestBufferIdx].head_time_cnt ;
    if(headTimeMark != freshBufferFooter)
    {
        return -2; // Error in Head/Foot data
    }
    return 0; //oldestBufferIdx;
}
uint32 AtcaIopADC::GetOldestBufferIdx() const {
    DMA_CH1_PCKT *pdma = (DMA_CH1_PCKT *) mappedDmaBase;
    uint32 buffIdx = 0u;

    volatile uint32 header = pdma[buffIdx].head_time_cnt;
    volatile uint32 oldestBufferHeader = header;
    for (uint32 dmaIndex = 1u; dmaIndex < NUMBER_OF_BUFFERS; dmaIndex++) {
        header = pdma[dmaIndex].head_time_cnt;
        if (header < oldestBufferHeader) {
            oldestBufferHeader = header;
            buffIdx  = dmaIndex;
        }
    }
    return buffIdx;
}

CLASS_REGISTER(AtcaIopADC, "1.0")

}

//  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
