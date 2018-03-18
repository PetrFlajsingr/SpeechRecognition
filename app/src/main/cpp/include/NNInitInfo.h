//
// Created by Petr Flajsingr on 17/03/2018.
//

#ifndef VOICERECOGNITION_NNINITINFO_H
#define VOICERECOGNITION_NNINITINFO_H

#include <string>

struct NNInitInfo{
    unsigned int layerCount;
    unsigned int* neuronCounts;

    virtual ~NNInitInfo() {
        delete[](neuronCounts);
    }
};

#endif //VOICERECOGNITION_NNINITINFO_H