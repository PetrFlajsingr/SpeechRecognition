//
// Created by Petr Flajsingr on 03/04/2018.
//

#include "include/VoiceActivityDetector.h"

VoiceActivityDetector::VoiceActivityDetector() {}

VoiceActivityDetector::~VoiceActivityDetector() {}

void VoiceActivityDetector::checkData(float* melBankData) {
    float sum = 0;
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; i++){
        meanForNormalisation[i] = (meanForNormalisation[i] * elementCount + melBankData[i]) / (elementCount + 1);
        sum += melBankData[i] - meanForNormalisation[i];
    }
    elementCount++;

    if(currentState == ACTIVE && sum <= 0) {
        counterForTransition[ACTIVE] = 0;
        counterForTransition[INACTIVE]++;
        if(counterForTransition[INACTIVE] == FRAMES_FOR_TRANSITION_INACTIVE){
            currentState = INACTIVE;
        }
    } else if(sum > 0) {
        counterForTransition[INACTIVE] = 0;
        counterForTransition[ACTIVE]++;
        if(counterForTransition[ACTIVE] == FRAMES_FOR_TRANSITION_ACTIVE){
            currentState = ACTIVE;
        }
    }
}

bool VoiceActivityDetector::isActive() {
    return currentState == ACTIVE;
}
