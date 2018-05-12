//
// Created by Petr Flajsingr on 03/04/2018.
//

#ifndef VOICERECOGNITION_VOICEACTIVITYDETECTOR_H
#define VOICERECOGNITION_VOICEACTIVITYDETECTOR_H

#include <constants.h>
#include <vector>

namespace SpeechRecognition::VoiceActivityDetection {
    /**
     * @brief Class detecting if there is voice in the signal
     *
     * This class detects activity in input signal base on Mel bank filter output.
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class VoiceActivityDetector {
    private:
        const unsigned int FRAMES_FOR_TRANSITION_INACTIVE = 25;
        const unsigned int FRAMES_FOR_TRANSITION_ACTIVE = 7;
        const float POWER_LIMIT = 0;

        /**
         * State of the VA detector
         */
        enum STATE {
            ACTIVE = 0,
            INACTIVE = 1
        };

        STATE currentState = INACTIVE;

        unsigned int transitionCounter[2] = {0, 0};

        float channelMeans[MEL_BANK_FRAME_LENGTH] = {0}; //< means for moving average

        unsigned int elementCount = 0; //< number of elements in moving average

        std::vector<float *> buffer; //< buffer for input data
    public:
        std::vector<float *> &getBuffer();

        VoiceActivityDetector();

        virtual ~VoiceActivityDetector();

        /**
         * Calculates VA state for provided data and updates objects inner state
         * @param data
         */
        void checkData(float *data);

        bool isActive();
    };
}


#endif //VOICERECOGNITION_VOICEACTIVITYDETECTOR_H
