//
// Created by Petr Flajsingr on 21/11/2017.
//

#ifndef VOICERECOGNITION_NEURALNETWORK_H
#define VOICERECOGNITION_NEURALNETWORK_H

struct NNInitInfo{
    unsigned int layerCount;
    unsigned int neuronCounts[];
};

class NeuralNetwork {
private:
    NNInitInfo info;
public:
    float** layer_biases;
    float** layer_vars;
    float*** layer_weights;

    NeuralNetwork(NNInitInfo initInfo);

    virtual ~NeuralNetwork();
};


#endif //VOICERECOGNITION_NEURALNETWORK_H
