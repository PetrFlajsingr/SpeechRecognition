//
// Created by Petr Flajsingr on 03/04/2018.
//

#include <VoiceActivityDetector.h>

VoiceActivityDetector::VoiceActivityDetector() {}

VoiceActivityDetector::~VoiceActivityDetector() {}

void VoiceActivityDetector::checkData(float* melBankData) {
    buffer.push_back(melBankData);
    if(buffer.size() > FRAMES_FOR_TRANSITION_ACTIVE) {
        delete[] buffer.at(0);
        buffer.erase(buffer.begin());
    }
    float sum = 0;
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; i++){
        meanForNormalisation[i] = (meanForNormalisation[i] * elementCount + melBankData[i]) / (elementCount + 1);
        sum += melBankData[i] - meanForNormalisation[i];
    }
    elementCount++;

    if(currentState == ACTIVE && sum <= POWER_LIMIT) {
        counterForTransition[ACTIVE] = 0;
        counterForTransition[INACTIVE]++;
        if(counterForTransition[INACTIVE] == FRAMES_FOR_TRANSITION_INACTIVE){
            currentState = INACTIVE;
        }
    } else if(sum > POWER_LIMIT) {
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

std::vector<float *> &VoiceActivityDetector::getBuffer() {
    return buffer;
}
