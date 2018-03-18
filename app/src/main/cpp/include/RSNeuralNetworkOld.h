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

class RSNeuralNetworkOld {
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

    void prepareInput(FeatureMatrix*data, int centerFrame, float* out);

public:
    RSNeuralNetworkOld(std::string filepath, const char* cacheDir);

    virtual ~RSNeuralNetworkOld();

    float* forward(float* data);

    FeatureMatrix* forwardAll(FeatureMatrix* data);
};


#endif //VOICERECOGNITION_RSNEURALNETWORK_H
