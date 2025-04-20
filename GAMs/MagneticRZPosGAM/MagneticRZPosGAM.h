/**
 * @file MagneticRZPosGAM.h
 * @brief Header file for class MagneticRZPosGAM
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

 * @details This header file contains the declaration of the class MagneticRZPosGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MAGNETICRZPOSGAM_H_
#define MAGNETICRZPOSGAM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "GAM.h"
#include "MessageI.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
namespace MARTe {
    /**
     * @brief An example of a GAM which has fixed inputs and outputs.
     *
     * @details This GAM multiplies the input signal by a Gain.
     * The configuration syntax is (names and types are only given as an example):
     *
     * +GAM_MagneticRZPos = {
     *     Class = MagneticRZPosGAM
     *     Gain = 5 //Compulsory
     *     NumberOfSamplesAvg  = 4 //Compulsory
     *     ResetInEachState = 0//Compulsory. 1--> reset in each state, 0--> reset if the previous state is different from the next state

     *     InputSignals = {
     *         Signal1 = {
     *             DataSource = "DDB1"
     *             Type = uint32
     *         }
     *     }
     *     OutputSignals = {
     *         Signal1 = {
     *             DataSource = "DDB1"
     *             Type = uint32
     *         }
     *     }
     * }
     */
    class MagneticRZPosGAM : public MARTe::GAM, public MARTe::MessageI {
        public:
            CLASS_REGISTER_DECLARATION()
                /**
                 * @brief Constructor. NOOP.
                 */
                MagneticRZPosGAM();

            /**
             * @brief Destructor. NOOP.
             */
            virtual ~MagneticRZPosGAM();

            /**
             * @brief Reads the Gain from the configuration file.
             * @param[in] data see GAM::Initialise. The parameter Gain shall exist and will be read as an uint32.
             * @return true if the parameter Gain can be read.
             */
            virtual bool Initialise(MARTe::StructuredDataI & data);

            /**
             * @brief Verifies correctness of the GAM configuration.
             * @details Checks that the number of input signals is equal to the number of output signals is equal to one and that the same type is used.
             * @return true if the pre-conditions are met.
             * @pre
             *   SetConfiguredDatabase() &&
             *   GetNumberOfInputSignals() == 4 
             *   GetNumberOfInputSignals() == 
             *   GetSignalType(InputSignals, 0) == GetSignalType(OutputSignals, 0) == uint32 &&
             */
            virtual bool Setup();

            /**
             * @brief Multiplies the input signal by the Gain.
             * @return true.
             */
            virtual bool Execute();
            
            /**
             * @brief Reset the states if required.
             * @details This functions has two operations modes:
             * <ul>
             * <li> Reset the GAM states every time the state changes.
             * </li>
             * <li> Reset the GAM if it was not executed in the previous state. e.i. if the GAM goes from
             * "A" to "B" and then from "B" to "C" it will not be reset. In the other hand if the GAM goes
             * from "A" to "B" and then from "C" to "D" the GAM will be reset the states.
             * </li>
             * </ul>
             * @param[in] currentStateName indicates the current state.
             * @param[in] nextStateName indicates the next state.
             * @return true if the state vectors are not NULL.
             */
             virtual bool PrepareNextState(const char8 * const currentStateName,
                    const char8 * const nextStateName);


            /**
             * @brief CalcOffSets method.
             * @details The method is registered as a messageable function.
             * @return ErrorManagement::NoError if the pre-conditions are met, ErrorManagement::ParametersError
             * otherwise.
               */

            MARTe::ErrorManagement::ErrorType CalcOffSets();

            /**
             * @brief Export information about the component
             */
            virtual bool ExportData(MARTe::StructuredDataI & data);

        private:

            /**
             * The configured gain.
             */
            MARTe::uint32 gain;

            /**
             * The configured numberOfSamplesAvg.
             */
            uint32 numberOfSamplesAvg;

            /**
             * The input signals
             */

            uint32 *triggerSdas;
            /**
             * The input Electric Probes signals
            */
            float32 *inputSignal;

            /*
            MARTe::float32 *inputElectricTop;
            MARTe::float32 *inputElectricInner;
            MARTe::float32 *inputElectricOuter;
            MARTe::float32 *inputElectricBottom;
            */
            uint32 numberOfInputElements;

            float32 inputOffset[4];

            MARTe::float32 **lastInputs;

            /**
             * The output signals
             */
            // MARTe::float32 **outputSignals;
            float32 *outputEpR;
            float32 *outputEpZ;

            /**
             * Indicates the behaviour of the reset when MARTe changes the state
             */
            bool resetInEachState;

            /**
             * Remember the last executed state.
             */
            StreamString lastStateExecuted;

            /**
             * Flag to detect SDAS Trigger Edge.
             */
            uint32 lastTriggerSdas;

    };
}
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* MAGNETICRZPOSGAM_H_ */

//  vim: syntax=cpp ts=4 sw=4 sts=4 sr et
