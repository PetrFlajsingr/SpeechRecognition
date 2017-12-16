//
// Created by Petr Flajsingr on 16/12/2017.
//

#include <string>
#include <fstream>
#include "RSNeuralNetwork.h"
#include "constants.h"

RSNeuralNetwork::RSNeuralNetwork(std::string filepath, const char* cacheDir) {
    NNInitInfo initInfo;
    initInfo.layerCount = 3;
    initInfo.neuronCounts = new unsigned int[initInfo.layerCount];
    initInfo.neuronCounts[0] = 360;
    initInfo.neuronCounts[1] = 300;
    initInfo.neuronCounts[2] = 300;
    this->outputNodeCount = 30;

    this->allocateMemory(initInfo);

    this->loadFromFile(filepath);

    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->neuralNetworkRSInstance = new ScriptC_RSneuralnetwork(this->renderScriptObject);

    this->prepareAllocations();
}

void RSNeuralNetwork::allocateMemory(NNInitInfo initInfo) {
    //layers init
    this->layerBiases = new float*[initInfo.layerCount];
    this->layerVars = new float*[1];
    this->layerMeans = new float*[1];
    this->layerWeights = new float**[initInfo.layerCount];
    //\layers
    this->weightCounts = new unsigned int[initInfo.layerCount];
    this->layerCount = initInfo.layerCount;

    //neuron init

    this->layerVars[0] = new float[initInfo.neuronCounts[0]];
    this->layerMeans[0] = new float[initInfo.neuronCounts[0]];
    for(unsigned int i = 0; i < initInfo.layerCount; ++i){
        this->layerBiases[i] = new float[initInfo.neuronCounts[i]];
        this->layerWeights[i] = new float*[initInfo.neuronCounts[i]];
        //weight allocation for dense NN
        if(i < initInfo.layerCount - 1) {
            this->weightCounts[i] = initInfo.neuronCounts[i + 1];
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[initInfo.neuronCounts[i + 1]];
            }
        }else{
            // 1 for output neurons
            this->weightCounts[i] = 30;
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[30];
            }
        }
    }
}

RSNeuralNetwork::~RSNeuralNetwork() {
    for(unsigned int i = 0; i < this->layerCount; ++i){
        delete[] this->layerBiases[i];
        for(unsigned int j = 0; j < this->weightCounts[i]; ++j) {
            delete[] this->layerWeights[i][j];
        }
        delete[] this->layerWeights[i];
    }
    delete[] this->layerVars[0];
    delete[] this->layerMeans[0];

    delete[] this->weightCounts;
    delete[] this->layerBiases;
    delete[] this->layerVars;
    delete[] this->layerMeans;
    delete[] this->layerWeights;
}

void RSNeuralNetwork::loadFromFile(std::string filepath) {
    std::ifstream file;
    file.open(filepath.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        //header
        uint8_t version;
        file.read((char*)&version, 1);
        uint8_t subversion;
        file.read((char*)&subversion, 1);
        uint8_t dataComponentCount;
        file.read((char*)&dataComponentCount, 1);
        file.ignore(7);
        //\header
        for(uint8_t i = 0; i < dataComponentCount; ++i){
            uint8_t recordType;
            file.read((char*)&recordType, 1);
            uint16_t firstDim;
            file.read((char*)&firstDim, 2);
            uint16_t secondDim;
            file.read((char*)&secondDim, 2);
            uint8_t recordNNtype;
            file.read((char*)&recordNNtype, 1);
            uint8_t layerNum;
            file.read((char*)&layerNum, 1);
            file.ignore(13);

            if(recordNNtype == 1 && layerNum == 1 && recordType == 1){
                float means;
                for(unsigned int j = 0; j < firstDim; ++j) {
                    file.read((char*)&means, sizeof(float));
                    this->layerMeans[0][j] = means;
                }
                continue;
            }

            if(recordNNtype == 2 && layerNum == 1 && recordType == 1){
                float vars;
                for(unsigned int j = 0; j < firstDim; ++j) {
                    file.read((char*)&vars, sizeof(float));
                    this->layerVars[0][j] = vars;
                }
                continue;
            }

            if(recordNNtype == 11 && layerNum == 1 && recordType == 2){
                float weight;
                for(unsigned int j = 0; j < firstDim; ++j){
                    for(unsigned int k = 0; k < secondDim; ++k){
                        file.read((char*)&weight, sizeof(float));
                        this->layerWeights[0][j][k] = weight;
                    }
                }
                continue;
            }

            if(recordNNtype == 12 && layerNum == 1 && recordType == 1){
                float bias;
                for(unsigned int j = 0; j < firstDim; ++j) {
                    file.read((char*)&bias, sizeof(float));
                    this->layerBiases[0][j] = bias;
                }
                continue;
            }

            if(recordNNtype == 11 && layerNum == 2 && recordType == 2){
                float weight;
                for(unsigned int j = 0; j < firstDim; ++j){
                    for(unsigned int k = 0; k < secondDim; ++k){
                        file.read((char*)&weight, sizeof(float));
                        this->layerWeights[1][j][k] = weight;
                    }
                }
                continue;
            }

            if(recordNNtype == 12 && layerNum == 2 && recordType == 1){
                float bias;
                for(unsigned int j = 0; j < firstDim; ++j) {
                    file.read((char*)&bias, sizeof(float));
                    this->layerBiases[1][j] = bias;
                }
                continue;
            }


            if(recordNNtype == 11 && layerNum == 3 && recordType == 2){
                float weight;
                for(unsigned int j = 0; j < firstDim; ++j){
                    for(unsigned int k = 0; k < secondDim; ++k){
                        file.read((char*)&weight, sizeof(float));
                        this->layerWeights[2][j][k] = weight;
                    }
                }
                continue;
            }

            if(recordNNtype == 12 && layerNum == 3 && recordType == 1){
                float bias;
                for(unsigned int j = 0; j < firstDim; ++j) {
                    file.read((char*)&bias, sizeof(float));
                    this->layerBiases[2][j] = bias;
                }
                continue;
            }
        }
    }
}

void RSNeuralNetwork::prepareAllocations() {

}
