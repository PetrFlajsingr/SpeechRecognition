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

    /**
     * @brief Class handling neural network using Renderscript
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class RSNeuralNetwork : public RSBase {
        /**
         * Types of activation functions
         */
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

        /**
         * Loads neural network from file. Requires format defined in header.
         * Allocates memory.
         * @param filepath Path to NN binary file
         */
        void loadFromFile(std::string filepath);

        /**
         * Prepares input for NN.
         * @param data input data - mel bank results
         * @param index index of middle frame
         * @param out
         */
        float *prepareInput(FeatureMatrix *data, int index);

        /**
         * Prepares Renderscript allocations and copies data to them.
         */
        void prepareAllocations();

        /**
         * Total count of neurons in network. For memory allocation and offset calculation.
         */
        unsigned int getTotalNeuronCount();

        /**
         * Total count of weights in network. For memory allocation and offset calculation.
         */
        unsigned int getTotalWeightsCount();
    public:
        /**
         * Creates objects needed for Renderscript. Loads the neural network from a file given by the first
         * parameter.
         * @param filepath Path to neural network file
         * @param cacheDir cache directory for renderscript
         */
        RSNeuralNetwork(std::string filepath, const char *cacheDir);

        virtual ~RSNeuralNetwork();

        /**
         * Pass data through network.
         * @param data input data
         * @return clearOutputNode of neural network
         */
        float *forward(float *data);

        /**
         * Forwards entire feature matrix.
         * @param data clearOutputNode of mel bank filters
         * @return matrix of neural network outputs
         */
        FeatureMatrix *forwardAll(FeatureMatrix *data);
    };
}

#endif //VOICERECOGNITION_RSNEURALNETWORK_H
