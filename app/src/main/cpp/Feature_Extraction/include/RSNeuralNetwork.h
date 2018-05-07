//
// Created by Petr Flajsingr on 17/03/2018.
//

#ifndef VOICERECOGNITION_RSNEURALNETWORK_H
#define VOICERECOGNITION_RSNEURALNETWORK_H


#include <FeatureMatrix.h>
#include <NNInitInfo.h>
#include <ScriptC_RSneuralnetwork.h>
#include <RenderScript.h>
#include "RSBase.h"

namespace SpeechRecognition::Feature_Extraction {
    using namespace Utility;
    class RSNeuralNetwork : public RSBase {
        enum ACTIVATION_FUNCTIONS {
            NOTHING = 0,
            SIGMOID = 1,
            SOFTMAX = 2
        };
    private:
        ScriptC_RSneuralnetwork *neuralNetworkRSInstance; //< Renderscript neural network implementation

        // NN data
        NNInitInfo info;

        float **biases;
        float **variances;
        float **means;
        float ***weights;

        sp<Allocation> dataAllocation;

        sp<Allocation> iterationAllocation;

        int neuronIterator[500];

        unsigned int totalNeuronCount = 0;

        ACTIVATION_FUNCTIONS *layerFunction;

        void loadFromFile(std::string filepath);

        float *prepareInput(FeatureMatrix *data, int index);

        void prepareAllocations();

        unsigned int getTotalNeuronCount();

        unsigned int getTotalWeightsCount();
    public:
        RSNeuralNetwork(std::string filepath, const char *cacheDir);

        virtual ~RSNeuralNetwork();

        float *forward(float *data);

        FeatureMatrix *forwardAll(FeatureMatrix *data);
    };
}

#endif //VOICERECOGNITION_RSNEURALNETWORK_H
