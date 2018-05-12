//
// Created by Petr Flajsingr on 03/04/2018.
//

#include <VoiceActivityDetector.h>

SpeechRecognition::VoiceActivityDetection::VoiceActivityDetector::VoiceActivityDetector() {}

SpeechRecognition::VoiceActivityDetection::VoiceActivityDetector::~VoiceActivityDetector() {}

void SpeechRecognition::VoiceActivityDetection::VoiceActivityDetector::checkData(float* melBankData) {
    buffer.push_back(melBankData);

    // delete old invalid data
    if(buffer.size() > FRAMES_FOR_TRANSITION_ACTIVE) {
        delete[] buffer.at(0);
        buffer.erase(buffer.begin());
    }

    // update means
    float sum = 0;
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; i++){
        channelMeans[i] = (channelMeans[i] * elementCount + melBankData[i]) / (elementCount + 1);
        sum += melBankData[i] - channelMeans[i];
    }
    elementCount++;

    // detect state
    if(currentState == ACTIVE && sum <= POWER_LIMIT) {
        transitionCounter[ACTIVE] = 0;
        transitionCounter[INACTIVE]++;
        if(transitionCounter[INACTIVE] == FRAMES_FOR_TRANSITION_INACTIVE){
            currentState = INACTIVE;
        }
    } else if(sum > POWER_LIMIT) {
        transitionCounter[INACTIVE] = 0;
        transitionCounter[ACTIVE]++;
        if(transitionCounter[ACTIVE] == FRAMES_FOR_TRANSITION_ACTIVE){
            currentState = ACTIVE;
        }
    }
}

bool SpeechRecognition::VoiceActivityDetection::VoiceActivityDetector::isActive() {
    return currentState == ACTIVE;
}

std::vector<float *> &SpeechRecognition::VoiceActivityDetection::VoiceActivityDetector::getBuffer() {
    return buffer;
}
