//
// Created by Petr Flajsingr on 17/03/2018.
//

#ifndef VOICERECOGNITION_RSNEURALNETWORK_H
#define VOICERECOGNITION_RSNEURALNETWORK_H

#include "ScriptC_RSneuralnetwork.h"
#include "constants.h"
#include "NNInitInfo.h"
#include "../../Miscelanous/include/FeatureMatrix.h"
#include <RenderScript.h>
#include <vector>
#include <string>

class RSNeuralNetwork {
    enum ACTIVATION_FUNCTIONS{
        NOTHING = 0,
        SIGMOID = 1,
        SOFTMAX = 2
    };
private:
    sp<RS> renderScriptObject;

    ScriptC_RSneuralnetwork* neuralNetworkRSInstance; //< Renderscript neural network implementation

    // NN data
    NNInitInfo info;

    float** layerBiases;
    float** layerVars;
    float** layerMeans;
    float*** layerWeights;

    unsigned int totalNeuronCount = 0;

    ACTIVATION_FUNCTIONS* layerFunction;

    void loadFromFile(std::string filepath);

    float* prepareInput(FeatureMatrix*data, int index);

    void prepareAllocations();
public:
    RSNeuralNetwork(std::string filepath, const char* cacheDir);

    virtual ~RSNeuralNetwork();

    float* forward(float* data);

    FeatureMatrix* forwardAll(FeatureMatrix* data);

    unsigned int getTotalNeuronCount();

    unsigned int getTotalWeightsCount();
};


#endif //VOICERECOGNITION_RSNEURALNETWORK_H
