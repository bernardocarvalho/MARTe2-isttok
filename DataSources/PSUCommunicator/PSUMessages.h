/**
 * @file PSUMessages.h
 * @brief Header file
 * @date 25/10/2025
 * @author Bernardo Carvalho
 *
 *
 * @copyright Copyright 2025 European Joint Undertaking for ITER and
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

 * @details This header file contains the declaration of the class AtcaIopADC
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */
#define FA_CHARGE_MESSAGE_1 0x6C
#define FA_CHARGE_MESSAGE_2 0x6D
#define FA_SHUTDOWN_MESSAGE_1 0x92
#define FA_SHUTDOWN_MESSAGE_2 0x93
#define FA_STARTOP_MESSAGE_1 0xFE
#define FA_STARTOP_MESSAGE_2 0xFF
#define FA_STOPOP_MESSAGE_1 0x00
#define FA_STOPOP_MESSAGE_2 0x01

#define FA_STARTED_MESSAGE_1 0xFE
#define FA_STARTED_MESSAGE_2 0xFF
#define FA_STOPPED_MESSAGE_1 0x00
#define FA_STOPPED_MESSAGE_2 0x01
#define FA_STOP_ERROR_MESSAGE_1 0x24
#define FA_STOP_ERROR_MESSAGE_2 0x25
#define FA_COMM_ERROR_MESSAGE_1 0xDA
#define FA_COMM_ERROR_MESSAGE_2 0xDB

#define FA_COMMUNICATION_MAX_PACKETS 4
#define FA_FRAMING_BIT_MASK 0x01

#define FA_SCALE_MIN 0
#define FA_SCALE_MAX 1023

#define LOG_CHARGE 1
#define LOG_SHUTDOWN 2
#define LOG_STARTOP 3
#define LOG_STOPOP 4
#define LOG_TEMPERATURE_FAULT 5
#define LOG_24V_FAULT 6
#define LOG_CHARGED 7
#define LOG_NOT_CHARGED 8
#define LOG_STARTED 9
#define LOG_STOPPED 10
#define LOG_STOP_FAULT 11
#define LOG_COMMUNICATION_FAULT 12
#define LOG_CURRENT_VALUE 13

// Logging #defines
// #define __FA_COM_LOG_RECEIVED_MESSAGES
// #define ___FA_COM_LOG_SENT_MESSAGES
#define __FA_COM_LOG_LEVEL InitialisationError

// Communicator Online Stages
#define FA_COMMUNICATOR_ONLINE_IDLE 0
#define FA_COMMUNICATOR_ONLINE_WAIT_CODAC_TRIGGER 1
#define FA_COMMUNICATOR_ONLINE_DISCHARGE 2
#define FA_COMMUNICATOR_ONLINE_STOP_OPERATION 3
#define FA_COMMUNICATOR_ONLINE_ERROR 4

#define FA_COMMUNICATOR_MAXIMUM_ATTEMPTS 5
