//
// Created by Petr Flajsingr on 21/11/2017.
//

#ifndef VOICERECOGNITION_NEURALNETWORK_H
#define VOICERECOGNITION_NEURALNETWORK_H

#include <fstream>
#include "FeaturesMatrixFloat.h"

struct NNInitInfo{
    unsigned int layerCount;
    unsigned int neuronCounts[];
};

class NeuralNetwork {
private:
    NNInitInfo info;

    FeaturesMatrixFloat* frames;

    unsigned int frameCount = 0;

    unsigned int outputNodeCount = 0;

    const unsigned int neighborFrames = 7;

    unsigned int dataVectorLength;

    void addVectors(float* dest, float* toAdd, unsigned int arrayLength);
    void mulVectors(float* dest, float* toMul, unsigned int arrayLength);
    void vectorMulMatrix(float* vector, unsigned int vectorLength,
                                        float** matrix, unsigned int matrixWidth,
                                        float* result);

    void sigmoid(float* dest, unsigned int arrayLength);

    float* dataVector;
    float* dataVectorNext;

    float** layerBiases;
    float** layerVars;
    float** layerMeans;
    float*** layerWeights;

    unsigned int* weightCounts;
    unsigned int layerCount;

    float** layerData;
public:
    NeuralNetwork(NNInitInfo initInfo);

    NeuralNetwork(std::string filepath);

    void forward();

    virtual ~NeuralNetwork();

    void setFeatureMatrix(FeaturesMatrixFloat* frames);
};


#endif //VOICERECOGNITION_NEURALNETWORK_H
