//
// Created by Petr Flajsingr on 17/03/2018.
//

#ifndef VOICERECOGNITION_NNINITINFO_H
#define VOICERECOGNITION_NNINITINFO_H

namespace SpeechRecognition::Feature_Extraction {
    struct NNInitInfo {
        unsigned int layerCount;
        unsigned int *neuronCounts;
        unsigned int inputSize;

        virtual ~NNInitInfo() {
            delete[] neuronCounts;
        }
    };
}

#endif //VOICERECOGNITION_NNINITINFO_H