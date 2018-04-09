//
// Created by Petr Flajsingr on 03/04/2018.
//

#ifndef VOICERECOGNITION_VOICEACTIVITYDETECTOR_H
#define VOICERECOGNITION_VOICEACTIVITYDETECTOR_H

#include <constants.h>
#include <vector>

class VoiceActivityDetector {
private:
    const unsigned int FRAMES_FOR_TRANSITION_INACTIVE = 10;
    const unsigned int FRAMES_FOR_TRANSITION_ACTIVE = 5;

    enum STATE {
        ACTIVE = 0,
        INACTIVE = 1
    };

    STATE currentState = INACTIVE;

    unsigned int counterForTransition[2] = {0, 0};

    float meanForNormalisation[MEL_BANK_FRAME_LENGTH] = {0};

    unsigned int elementCount = 0;

    std::vector<float*> buffer;
public:
    std::vector<float *> &getBuffer();

public:
    VoiceActivityDetector();

    virtual ~VoiceActivityDetector();

    void checkData(float* data);

    bool isActive();
};


#endif //VOICERECOGNITION_VOICEACTIVITYDETECTOR_H
