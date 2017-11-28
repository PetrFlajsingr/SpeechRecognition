//
// Created by Petr Flajsingr on 21/11/2017.
//

#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork(NNInitInfo initInfo) {
    //layers init
    this->layer_biases = new float*[initInfo.layerCount];
    this->layer_vars = new float*[initInfo.layerCount];
    this->layer_weights = new float**[initInfo.layerCount];
    //\layers

    //neuron init
    for(unsigned int i = 0; i < initInfo.layerCount; ++i){
        this->layer_biases[i] = new float[initInfo.neuronCounts[i]];
        this->layer_vars[i] = new float[initInfo.neuronCounts[i]];
        this->layer_weights[i] = new float*[initInfo.neuronCounts[i]];
        //weight allocation for dense NN
        if(i < initInfo.layerCount - 1) {
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layer_weights[i][j] = new float[initInfo.neuronCounts[i + 1]];
            }
        }else{
            // 1 for output neurons
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layer_weights[i][j] = new float[1];
            }
        }
    }
    //\neuron init
    this->info = initInfo;
}

NeuralNetwork::~NeuralNetwork() {
    for(unsigned int i = 0; i < this->info.layerCount; ++i){
        delete[] this->layer_biases[i];
        delete[] this->layer_vars[i];
        for(unsigned int j = 0; j < this->info.neuronCounts[i]; ++j) {
            delete[] this->layer_weights[i][j];
        }
        delete[] this->layer_weights[i];
    }
}
