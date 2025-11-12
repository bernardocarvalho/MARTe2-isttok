/**
 * @file PSUCommunicatorTX.cpp
 * @brief Source file for class PSUCommunicatorTX
 * @date 19/01/2024
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
 * the class PSUCommunicatorTX (public, protected, and private). Be aware
 that some
 * methods, such as those inline could be defined on the header file, instead.
 *
 * https://vcis-gitlab.f4e.europa.eu/aneto/MARTe2-components/-/blob/master/Source/Components/DataSources/NI6259/NI6259DAC.cpp
 */

#define DLL_API

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/
#include <fcntl.h>

#include <math.h>
#include <unistd.h> // for close()

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/
#include "AdvancedErrorManagement.h"
#include "MemoryMapSynchronisedOutputBroker.h"
#include "PSUCommunicatorTX.h"
#include "PSUMessages.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/
namespace MARTe {
const float32 DAC_RANGE = 20.0;
//  const float32 ATCA_IOP_MAX_DAC_RANGE = 20.0;
PSUCommunicatorTX::PSUCommunicatorTX() : DataSourceI(), MessageI() {
  // boardFileDescriptor = -1;
  triggerSet = false;
  isTriggered = false;
  communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_IDLE;
  // channelsMemory = NULL_PTR(float32 *);
  currentValue = 0.0; // NULL_PTR(float32 *);
  currentStep = 0.0;
  pointOfZeroCurrent = 0.0;

  filter = ReferenceT<RegisteredMethodsMessageFilter>(
      GlobalObjectsDatabase::Instance()->GetStandardHeap());
  filter->SetDestination(this);
  ErrorManagement::ErrorType ret = MessageI::InstallMessageFilter(filter);
  if (!ret.ErrorsCleared()) {
    REPORT_ERROR(ErrorManagement::FatalError,
                 "Failed to install message filters");
  }
}

/*lint -e{1551} the destructor must guarantee that the Timer SingleThreadService
 * is stopped.*/
PSUCommunicatorTX::~PSUCommunicatorTX() {
  // REPORT_ERROR(ErrorManagement::Information, " Close Device Status Reg %d,
  // 0x%x", rc, statusReg); close(boardFileDescriptor);
  serial.Close();
  REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "Close  %s OK.",
                          portName);
}

bool PSUCommunicatorTX::AllocateMemory() { return true; }

uint32 PSUCommunicatorTX::GetNumberOfMemoryBuffers() { return 1u; }

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * The memory buffer is independent of the bufferIdx.*/
bool PSUCommunicatorTX::GetSignalMemoryBuffer(const uint32 signalIdx,
                                              const uint32 bufferIdx,
                                              void *&signalAddress) {
  bool ok = (signalIdx < (PSU_MAX_CHANNELS));
  if (ok) {
    signalAddress = &currentValue;
  }
  return ok;
}

const char8 *PSUCommunicatorTX::GetBrokerName(StructuredDataI &data,
                                              const SignalDirection direction) {
  const char8 *brokerName = NULL_PTR(const char8 *);
  if (direction == OutputSignals) {
    uint32 trigger = 0u;
    if (!data.Read("Trigger", trigger)) {
      trigger = 0u;
    }

    if (trigger == 1u) {
      brokerName = "MemoryMapSynchronisedOutputBroker";
      triggerSet = true;
    } else {
      brokerName = "MemoryMapOutputBroker";
    }
  } else {
    REPORT_ERROR(ErrorManagement::ParametersError,
                 "DataSource not compatible with InputSignals");
  }
  return brokerName;
}

bool PSUCommunicatorTX::GetInputBrokers(ReferenceContainer &inputBrokers,
                                        const char8 *const functionName,
                                        void *const gamMemPtr) {
  return false;
}

bool PSUCommunicatorTX::GetOutputBrokers(ReferenceContainer &outputBrokers,
                                         const char8 *const functionName,
                                         void *const gamMemPtr) {
  // Check if there is a Trigger signal for this function.
  uint32 functionIdx = 0u;
  uint32 nOfFunctionSignals = 0u;
  uint32 i;
  bool triggerGAM = false;
  bool ok = GetFunctionIndex(functionIdx, functionName);

  if (ok) {
    ok = GetFunctionNumberOfSignals(OutputSignals, functionIdx,
                                    nOfFunctionSignals);
  }
  uint32 trigger = 0u;
  for (i = 0u; (i < nOfFunctionSignals) && (ok) && (!triggerGAM); i++) {
    ok = GetFunctionSignalTrigger(OutputSignals, functionIdx, i, trigger);
    triggerGAM = (trigger == 1u);
  }
  if ((ok) && (triggerGAM)) {
    ReferenceT<MemoryMapSynchronisedOutputBroker> broker(
        "MemoryMapSynchronisedOutputBroker");
    ok = broker.IsValid();

    if (ok) {
      ok = broker->Init(OutputSignals, *this, functionName, gamMemPtr);
    }
    if (ok) {
      ok = outputBrokers.Insert(broker);
    }
    // Must also add the signals which are not triggering but that belong to the
    // same GAM...
    if (ok) {
      if (nOfFunctionSignals > 1u) {
        ReferenceT<MemoryMapOutputBroker> brokerNotSync(
            "MemoryMapOutputBroker");
        ok = brokerNotSync.IsValid();
        if (ok) {
          ok = brokerNotSync->Init(OutputSignals, *this, functionName,
                                   gamMemPtr);
        }
        if (ok) {
          ok = outputBrokers.Insert(brokerNotSync);
        }
      }
    }
  } else {
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

/*lint -e{715}  [MISRA C++ Rule 0-1-11], [MISRA C++ Rule 0-1-12]. Justification:
 * the counter and the timer are always reset irrespectively of the states being
 * changed.*/
bool PSUCommunicatorTX::PrepareNextState(const char8 *const currentStateName,
                                         const char8 *const nextStateName) {
  return true;
}

bool PSUCommunicatorTX::Initialise(StructuredDataI &data) {
  bool ok = DataSourceI::Initialise(data);
  // StreamString portName;
  if (ok) {
    ok = data.Read("PortName", portName);
    if (ok) {
      REPORT_ERROR_PARAMETERS(ErrorManagement::Information,
                              "The port name is set to %s", portName.Buffer());
    } else {
      REPORT_ERROR(ErrorManagement::ParametersError,
                   "The port name property shall be set");
    }
  }
  uint32 baudRate = 0u;
  if (ok) {
    ok = data.Read("BaudRate", baudRate);
    if (ok) {
      REPORT_ERROR_PARAMETERS(ErrorManagement::Information,
                              "The baud rate is set to %d", baudRate);
    } else {
      REPORT_ERROR(ErrorManagement::ParametersError,
                   "The baud rate property shall be set");
    }
  }
  if (ok) {
    ok = data.Read("CurrentStep", currentStep);
    if (ok) {
      REPORT_ERROR_PARAMETERS(ErrorManagement::Information,
                              "The CurrentStep is set to %.2f", currentStep);
    } else {
      REPORT_ERROR(ErrorManagement::ParametersError,
                   "The CurrentStep rate property shall be set");
    }
  }
  if (ok) {
    ok = data.Read("PointOfZeroCurrent", pointOfZeroCurrent);
    if (ok) {
      REPORT_ERROR_PARAMETERS(ErrorManagement::Information,
                              "The PointOfZeroCurrent is set to %.2f",
                              currentStep);
    } else {
      REPORT_ERROR(ErrorManagement::ParametersError,
                   "The PointOfZeroCurrent rate property shall be set");
    }
  }
  if (ok) {
    if (!data.Read("Timeout", timeout)) {
      timeout = 1000u;
    }
  }
  /*
     if (ok) {
     ok = data.Read("SerialTimeout", serialTimeout);
     if (ok) {
     REPORT_ERROR(ErrorManagement::Information, "The serial timeout is set to
     %d", serialTimeout);
     }
     else {
     REPORT_ERROR(ErrorManagement::ParametersError, "The serial timeout
     property shall be set");
     }
     }
     */
  if (ok) {
    ok = serial.SetSpeed(baudRate);
  }
  if (ok) {
    ok = serial.Open(portName.Buffer());
  }
  if (!ok) {
    REPORT_ERROR_PARAMETERS(ErrorManagement::ParametersError,
                            "The port %s Not opened.", portName);
  }

  return ok;
}

bool PSUCommunicatorTX::SetConfiguredDatabase(StructuredDataI &data) {
  uint32 i;
  bool ok = DataSourceI::SetConfiguredDatabase(data);

  if (ok) {
    ok = triggerSet;
  }
  if (!ok) {
    REPORT_ERROR(ErrorManagement::ParametersError,
                 "At least one Trigger signal shall be set.");
  }
  if (ok) {
    ok = (GetSignalType(0u) == Float32Bit);
    //}
    if (!ok) {
      REPORT_ERROR(ErrorManagement::ParametersError,
                   "The PSU current signal shall be of type Float32Bit");
    }
  }

  uint32 nOfFunctions = GetNumberOfFunctions();
  uint32 functionIdx;
  // Check that the number of samples for all the signals is one
  for (functionIdx = 0u; (functionIdx < nOfFunctions) && (ok); functionIdx++) {
    uint32 nOfSignals = 0u;
    ok = GetFunctionNumberOfSignals(OutputSignals, functionIdx, nOfSignals);

    for (i = 0u; (i < nOfSignals) && (ok); i++) {
      uint32 nSamples = 0u;
      ok = GetFunctionSignalSamples(OutputSignals, functionIdx, i, nSamples);
      if (ok) {
        ok = (nSamples == 1u);
      }
      if (!ok) {
        REPORT_ERROR(ErrorManagement::ParametersError,
                     "The number of samples shall be exactly one");
      }
    }
  }

  return ok;
}

// Create/Decode current packet
bool PSUCommunicatorTX::CreateCurrentPacket() {
  bool ok = true;

  // Calculate the point in the scale of the current
  int16 pointOfCurrent =
      (int16)(pointOfZeroCurrent + currentValue / currentStep);

  // Saturate current
  if (pointOfCurrent < FA_SCALE_MIN)
    pointOfCurrent = FA_SCALE_MIN;
  if (pointOfCurrent > FA_SCALE_MAX)
    pointOfCurrent = FA_SCALE_MAX;

  // Build packets
  uint16 pc = (uint16)pointOfCurrent;
  uint16 nc = ~pc;
  packet[0] = (char8)(0x0000 | ((nc & 0x03C0) >> 5) | ((pc & 0x0007) << 5));
  packet[1] = (char8)(0x0001 | ((pc & 0x03F8) >> 2));
  nc = (uint16)packet[1];
  nc <<= 8;
  nc &= 0xFF00;
  nc |= packet[0];
  REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "CurrentPacket %d 0x%x",
                          pointOfCurrent, nc); // packet[0]);
  // packet[1]);

  return ok;
}

bool PSUCommunicatorTX::SendMessage() {
  bool ok = true;
  uint16 uval;
  memcpy(&uval, packet, 2);
  REPORT_ERROR_PARAMETERS(ErrorManagement::Information, "SendMessage  0x%",
                          uval);
  //                          (uint16)packet[0], (uint16)packet[1]);
  //  packet[0], packet[1]);
  serial.Write(packet, 2);
  return ok;
}

bool PSUCommunicatorTX::CommunicatorOnline() {
  bool ok = true;
  switch (communicatorOnlineStage) {
  case FA_COMMUNICATOR_ONLINE_IDLE:
    // Log the entry on this stage
    // if(this->communicatorOnlineIdleCount++ == 0)
    // AssertErrorCondition(Information, "[FACom] COMMUNICATOR_ONLINE_IDLE");
    isTriggered = false;
    break;
  // Wait for the CODAC trigger
  case FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER:

    // Log the entry on this stage
    // if(this->communicatorOnlineWaitTriggerCount++ == 0)
    // AssertErrorCondition(Information, "[FACom]
    // COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER");

    if (isTriggered) {
      //  Send Start Operation message
      // this->SendMessage(FA_STARTOP_MESSAGE_1, FA_STARTOP_MESSAGE_2);
      packet[0] = FA_STARTOP_MESSAGE_1;
      packet[1] = FA_STARTOP_MESSAGE_2;
      SendMessage();

      // Increase attempts counter
      // this->communicatorOnlineStartOperationAttempts++;

      // Change online state
      communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_DISCHARGE;
      //	this->communicatorOnlineWaitTriggerCount = 0;
    }

    break;
  case FA_COMMUNICATOR_ONLINE_DISCHARGE:
    /*
                                if(PlasmaEnded)
                                {
                                        this->communicatorOnlineStage =
       FA_COMMUNICATOR_ONLINE_STOP_OPERATION;
                                        this->communicatorOnlineDischargeCount =
       0; break;
                                }
        */
    // CreateCurrentPacket(CurrentToSendCopy, packet1, packet2);
    CreateCurrentPacket();
    ok = SendMessage();
    // communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_STOP_OPERATION;
    break;
  case FA_COMMUNICATOR_ONLINE_STOP_OPERATION:
    packet[0] = FA_STARTOP_MESSAGE_1;
    packet[1] = FA_STARTOP_MESSAGE_2;
    ok = SendMessage();
    communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_IDLE;
    break;
  case FA_COMMUNICATOR_ONLINE_ERROR:

    // AssertErrorCondition(InitialisationError, "[FACom]::%s Power supplies
    // timeout: after %d attemps, FA_COMMUNICATOR_ONLINE_ERROR, RETURN FALSE",
    // this->Name(), this->communicatorOnlineStopOperationAttempts);

    ok = false;

    break;

  default:
    break;
  }
  return ok;
}
bool PSUCommunicatorTX::Synchronise() {
  uint32 i;
  int32 w = 24;
  bool ok = true;
  // char8 text[] = "ola";
  //  if (channelsMemory != NULL_PTR(float32 *)) {

  //                value = channelsMemory[0] / DAC_RANGE;
  CommunicatorOnline();
  // REPORT_ERROR_PARAMETERS(ErrorManagement::Information,
  //                         "Synchronise called. value: %f", currentValue);
  // char8 *data = reinterpret_cast<char8 *>(&ser_value);
  // serial.Write(data, sizeof(int32));
  // serial.Write(text, 4);
  /*

     w = dacValues[i];
     }
     */
  return ok;
}
// Messages
ErrorManagement::ErrorType PSUCommunicatorTX::GoOnlineIdle() {
  ErrorManagement::ErrorType err;
  REPORT_ERROR(ErrorManagement::Information,
               "PSUCommunicatorTX::GoOnlineIdle. Got Message!");
  communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_STOP_OPERATION;
  // communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_IDLE;

  return err;
}
ErrorManagement::ErrorType PSUCommunicatorTX::GoWaitTrigger() {
  ErrorManagement::ErrorType err;
  REPORT_ERROR(ErrorManagement::Information,
               "PSUCommunicatorTX::GoWaitTrigger. Got Message!");
  communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER;

  return err;
}
ErrorManagement::ErrorType PSUCommunicatorTX::TriggerPSU() {
  isTriggered = true;

  ErrorManagement::ErrorType err;
  REPORT_ERROR(ErrorManagement::Information,
               "PSUCommunicatorTX::TriggerPSU. Got Message!");
  // communicatorOnlineStage = FA_COMMUNICATOR_ONLINE_DISCHARGE;

  return err;
}

CLASS_REGISTER(PSUCommunicatorTX, "1.0")
CLASS_METHOD_REGISTER(PSUCommunicatorTX, GoOnlineIdle)
CLASS_METHOD_REGISTER(PSUCommunicatorTX, GoWaitTrigger)
CLASS_METHOD_REGISTER(PSUCommunicatorTX, TriggerPSU)
} // namespace MARTe
  //  vim: syntax=cpp ts=2 sw=2 sts=2 sr et
