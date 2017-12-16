//
// Created by Petr Flajsingr on 16/12/2017.
//

#ifndef VOICERECOGNITION_RSNEURALNETWORK_H
#define VOICERECOGNITION_RSNEURALNETWORK_H

#include "ScriptC_RSneuralnetwork.h"
#include <RenderScript.h>

using namespace android::RSC;
struct NNInitInfo{
    unsigned int layerCount;
    unsigned int* neuronCounts;
};

class RSNeuralNetwork {
private:
    sp<RS> renderScriptObject;

    ScriptC_RSneuralnetwork* neuralNetworkRSInstance;

    void prepareAllocations();

    unsigned int outputNodeCount = 0;

    float** layerBiases;
    float** layerVars;
    float** layerMeans;
    float*** layerWeights;

    unsigned int* weightCounts;
    unsigned int layerCount;

    void allocateMemory(NNInitInfo initInfo);

    void loadFromFile(std::string filepath);

public:
    RSNeuralNetwork(std::string filepath);

    virtual ~RSNeuralNetwork();
};


#endif //VOICERECOGNITION_RSNEURALNETWORK_H
