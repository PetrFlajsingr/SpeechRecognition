//
// Created by Petr Flajsingr on 21/11/2017.
//

#ifndef VOICERECOGNITION_NEURALNETWORK_H
#define VOICERECOGNITION_NEURALNETWORK_H


#include <NNInitInfo.h>
#include <FeatureMatrix.h>

class NeuralNetwork {
private:
    NNInitInfo info;

    FeatureMatrix* frames;

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

    void allocateMemory(NNInitInfo initInfo);
public:
    NeuralNetwork(std::string filepath);

    void forward();

    virtual ~NeuralNetwork();

    void setFeatureMatrix(FeatureMatrix* frames);
};


#endif //VOICERECOGNITION_NEURALNETWORK_H
