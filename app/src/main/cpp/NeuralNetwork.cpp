//
// Created by Petr Flajsingr on 21/11/2017.
//

#include <string.h>
#include <math.h>
#include "NeuralNetwork.h"
#include "constants.h"

NeuralNetwork::NeuralNetwork(std::string filepath) {
    std::ifstream file;
    file.open(filepath.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        //header
        char version;
        file.read(&version, 1);
        char subversion;
        file.read(&subversion, 1);
        char dataComponentCount;
        file.read(&dataComponentCount, 1);
        file.ignore(7);
        //\header
        for(unsigned char i = 0; i < (unsigned char)dataComponentCount; ++i){
            char recordType;
            file.read(&recordType, 1);
            char firstDim;
            file.read(&firstDim, 1);
            char secondDim;
            file.read(&secondDim, 1);
            char recordNNtype;
            file.read(&recordNNtype, 1);

        }
    }
}

NeuralNetwork::NeuralNetwork(NNInitInfo initInfo) {
    //layers init
    this->layerBiases = new float*[initInfo.layerCount];
    this->layerVars = new float*[initInfo.layerCount];
    this->layerMeans = new float*[initInfo.layerCount];
    this->layerWeights = new float**[initInfo.layerCount];
    //\layers
    this->weightCounts = new unsigned int[initInfo.layerCount];
    this->layerCount = initInfo.layerCount;

    //neuron init
    for(unsigned int i = 0; i < initInfo.layerCount; ++i){
        this->layerBiases[i] = new float[initInfo.neuronCounts[i]];
        this->layerVars[i] = new float[initInfo.neuronCounts[i]];
        this->layerMeans[i] = new float[initInfo.neuronCounts[i]];
        this->layerWeights[i] = new float*[initInfo.neuronCounts[i]];
        //weight allocation for dense NN
        if(i < initInfo.layerCount - 1) {
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[initInfo.neuronCounts[i + 1]];
            }
        }else{
            // 1 for output neurons
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[1];
            }
        }
    }

    this->dataVectorLength = (this->outputNodeCount * 2 + 1) * MEL_BANK_FRAME_LENGTH;

    this->dataVector = new float[this->dataVectorLength];
    this->dataVectorNext = new float[this->dataVectorLength];
    //\neuron init
    this->info = initInfo;
}

NeuralNetwork::~NeuralNetwork() {
    for(unsigned int i = 0; i < this->info.layerCount; ++i){
        delete[] this->layerBiases[i];
        delete[] this->layerVars[i];
        delete[] this->layerMeans[i];
        delete[] this->layerData[i];
        for(unsigned int j = 0; j < this->info.neuronCounts[i]; ++j) {
            delete[] this->layerWeights[i][j];
        }
        delete[] this->layerWeights[i];
    }
    delete[] this->weightCounts;
    delete[] this->dataVector;
    delete[] this->dataVectorNext;
    delete[] this->layerBiases;
    delete[] this->layerVars;
    delete[] this->layerMeans;
    delete[] this->layerData;
    delete[] this->layerWeights;
}

void NeuralNetwork::setFeatureMatrix(FeaturesMatrixFloat *frames) {
    this->frames = frames;
    this->frameCount = frames->getFramesNum();
}

void NeuralNetwork::forward() {
    FeaturesMatrixFloat* bottleneckMatrix = new FeaturesMatrixFloat();
    bottleneckMatrix->init(this->frameCount, this->outputNodeCount);

    int frameNum = 0;

    for(int i = 0; i < this->neighborFrames * 2 + 1; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = i - this->neighborFrames;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > this->outputNodeCount - 1){
                frameNum = this->outputNodeCount - 1;
            }
            this->dataVectorNext[i + (this->neighborFrames*2+1)*j] = this->frames->getFeaturesMatrix()[frameNum][j];
        }
    }
    memcpy(this->dataVector, this->dataVectorNext, this->dataVectorLength * sizeof(float));

    for(int currentFrame = 0; currentFrame < frameCount; ++currentFrame){

        //input layer
        addVectors(this->dataVector, this->layerMeans[0], this->dataVectorLength);
        mulVectors(this->dataVector, this->layerVars[0], this->dataVectorLength);
        vectorMulMatrix(this->dataVector, this->dataVectorLength,
                        this->layerWeights[0], this->weightCounts[0], this->layerData[0]);
        //inner layers
        unsigned int layerIterator;
        for(layerIterator = 1; layerIterator < this->layerCount - 1; ++layerIterator) {
            addVectors(this->layerData[layerIterator - 1], this->layerBiases[layerIterator], this->dataVectorLength);
            sigmoid(this->layerData[layerIterator - 1], this->dataVectorLength);
            vectorMulMatrix(this->layerData[layerIterator - 1], this->dataVectorLength,
                            this->layerWeights[layerIterator], this->weightCounts[layerIterator], this->layerData[layerIterator]);
        }

        //output layer
        addVectors(this->layerData[layerIterator - 1], this->layerBiases[layerIterator - 1], this->dataVectorLength);
        sigmoid(this->layerData[layerIterator - 1], this->dataVectorLength);
        vectorMulMatrix(this->layerData[layerIterator - 1], this->dataVectorLength,
                        this->layerWeights[layerIterator], this->weightCounts[layerIterator],
                        bottleneckMatrix->getFeaturesMatrix()[currentFrame]);
        addVectors(bottleneckMatrix->getFeaturesMatrix()[currentFrame], this->layerBiases[layerIterator], this->dataVectorLength);

        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
            memcpy(this->dataVector + i * (this->dataVectorLength + 1),
                   this->dataVectorNext + i * (this->dataVectorLength + 1),
                   this->dataVectorLength - 1);
            frameNum = currentFrame + this->neighborFrames + 1;
            if(frameNum > (frameCount - 1)){
                frameNum = frameCount - 1;
            }
            this->dataVector[(i + 1) * this->dataVectorLength - 1] = this->frames->getFeaturesMatrix()[frameNum][i];
        }
        memcpy(this->dataVectorNext, this->dataVector, this->dataVectorLength);
    }
}

void NeuralNetwork::addVectors(float *dest, float *toAdd, unsigned int arrayLength) {
    for(unsigned int i = 0; i < arrayLength; ++i){
        dest[i] += toAdd[i];
    }
}

void NeuralNetwork::mulVectors(float *dest, float *toMul, unsigned int arrayLength) {
    for(unsigned int i = 0; i < arrayLength; ++i){
        dest[i] *= toMul[i];
    }
}

void NeuralNetwork::sigmoid(float *dest, unsigned int arrayLength) {
    for(unsigned int i = 0; i < arrayLength; ++i){
        dest[i] = (float)(1.0/(1+exp(-dest[i])));
    }
}

void NeuralNetwork::vectorMulMatrix(float* vector, unsigned int vectorLength,
                     float** matrix, unsigned int matrixWidth,
                     float* result){
    for(int j = 0; j < matrixWidth; ++j){
        result[j] = 0;
        for(int i = 0; i < vectorLength; ++i){
            result[j] = result[j] + vector[i] * matrix[j][i];
        }
    }
}


