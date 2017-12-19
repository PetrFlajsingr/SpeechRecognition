//
// Created by Petr Flajsingr on 16/12/2017.
//

#ifndef VOICERECOGNITION_RSNEURALNETWORK_H
#define VOICERECOGNITION_RSNEURALNETWORK_H

#include "ScriptC_RSneuralnetwork.h"
#include "NeuralNetwork.h"
#include <RenderScript.h>
#include <vector>

using namespace android::RSC;

class RSNeuralNetwork {
private:
    sp<RS> renderScriptObject;

    ScriptC_RSneuralnetwork* neuralNetworkRSInstance;

    void prepareAllocations();

    NNInitInfo info;

    unsigned int outputNodeCount = 0;

    float** layerBiases;
    float** layerVars;
    float** layerMeans;
    float*** layerWeights;

    unsigned int layerCount;

    void allocateMemory();

    void loadFromFile(std::string filepath);

    void prepareInput(FeaturesMatrixFloat*data, int centerFrame, float* out);

public:
    RSNeuralNetwork(std::string filepath, const char* cacheDir);

    virtual ~RSNeuralNetwork();

    float* forward(float* data);

    FeaturesMatrixFloat* forwardAll(FeaturesMatrixFloat* data);
};


#endif //VOICERECOGNITION_RSNEURALNETWORK_H
