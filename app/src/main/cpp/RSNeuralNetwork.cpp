//
// Created by Petr Flajsingr on 17/03/2018.
//


#include <fstream>
#include "RSNeuralNetwork.h"

RSNeuralNetwork::RSNeuralNetwork(std::string filepath, const char *cacheDir) {
    this->loadFromFile(filepath);

    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->neuralNetworkRSInstance = new ScriptC_RSneuralnetwork(this->renderScriptObject);

    this->prepareAllocations();
}

void RSNeuralNetwork::loadFromFile(std::string filepath) {
    std::ifstream file;
    file.open(filepath.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        this->layerVars = new float*[1];
        this->layerMeans = new float*[1];

        //layer count
        uint8_t layerCount;
        file.read((char*)&layerCount, 1);
        info.layerCount = layerCount;

        this->layerBiases = new float*[info.layerCount];
        this->layerWeights = new float**[info.layerCount];

        info.neuronCounts = new unsigned int[info.layerCount];

        //variance and mean length
        uint16_t var_meanLength;
        file.read((char*)&var_meanLength, 2);

        this->layerVars[0] = new float[var_meanLength];
        this->layerMeans[0] = new float[var_meanLength];

        //mean data
        float tempFloat = 0;
        for(uint16_t i = 0; i < var_meanLength; ++i){
            file.read((char*)&tempFloat, sizeof(float));
            this->layerMeans[0][i] = tempFloat;
        }

        //variance data
        for(uint16_t i = 0; i < var_meanLength; ++i){
            file.read((char*)&tempFloat, sizeof(float));
            this->layerVars[0][i] = tempFloat;
        }

        uint8_t temp_8b;
        for(uint16_t layerIterator = 0; layerIterator < layerCount; layerIterator++){
            file.read((char*)&temp_8b, 1);

            uint16_t biasSize;
            file.read((char*)&biasSize, 2);
            info.neuronCounts[layerIterator] = biasSize;

            this->layerBiases[layerIterator] = new float[biasSize];

            //bias data
            for(uint16_t i = 0; i < biasSize; ++i){
                file.read((char*)&tempFloat, sizeof(float));
                this->layerBiases[layerIterator][i] = tempFloat;
            }

            uint16_t weightSizeX;
            file.read((char*)&weightSizeX, 2);
            uint16_t weightSizeY;
            file.read((char*)&weightSizeY, 2);

            this->layerWeights[layerIterator] = new float*[weightSizeX];
            //weights data
            for(uint16_t i = 0; i < weightSizeX; ++i){
                this->layerWeights[layerIterator][i] = new float[weightSizeY];
                for(uint16_t j = 0; j < weightSizeY; ++j) {
                    file.read((char *) &tempFloat, sizeof(float));
                    this->layerWeights[layerIterator][i][j] = tempFloat;
                }
            }

            file.read((char*)&temp_8b, 1);
            switch(temp_8b){
                case 0:
                    this->layerFunction[layerIterator] = NOTHING;
                    break;
                case 1:
                    this->layerFunction[layerIterator] = SIGMOID;
                    break;
                case 2:
                    this->layerFunction[layerIterator] = SOFTMAX;
                    break;
                default:break;
            }
        }
        file.close();
    }
}

void RSNeuralNetwork::prepareInput(FeatureMatrix *data, int index, float *out) {
    const int ROLLING_WINDOW_SIZE = 7;
    int frameNum = 0;
    for(int i = 0; i < ROLLING_WINDOW_SIZE * 2 + 1; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = index + i - ROLLING_WINDOW_SIZE;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > data->getFramesNum() - 1){
                frameNum = data->getFramesNum() - 1;
            }
            out[i + (ROLLING_WINDOW_SIZE * 2 + 1) * j] = data->getFeaturesMatrix()[frameNum][j];
        }
    }
}

RSNeuralNetwork::~RSNeuralNetwork() {
    for(unsigned int i = 0; i < info.layerCount; ++i){
        delete[] this->layerBiases[i];
        for(unsigned int j = 0; j < info.neuronCounts[i + 1]; ++j) {
            delete[] this->layerWeights[i][j];
        }
        delete[] this->layerWeights[i];
    }
    delete[] this->layerVars[0];
    delete[] this->layerMeans[0];

    delete[] this->layerBiases;
    delete[] this->layerVars;
    delete[] this->layerMeans;
    delete[] this->layerWeights;
}

float *RSNeuralNetwork::forward(float *data) {
    sp<Allocation> dataAllocation = Allocation::createSized(this->renderScriptObject,
                                                            Element::F32(this->renderScriptObject),
                                                            info.neuronCounts[0]);

    dataAllocation->copy1DFrom(data);

    sp<Allocation> iterAlloc;

    int neuronIterator[info.neuronCounts[0]];
    for(int i = 0; i < info.neuronCounts[0]; i++) {
        neuronIterator[i] = i;
    }


    uint32_t biasOffset = 0, weightOffset = 0;
    for(uint32_t layerIterator = 0; layerIterator < info.layerCount; layerIterator++){
        int neuronIterator[info.neuronCounts[layerIterator]];
        for(int i = 0; i < info.neuronCounts[layerIterator]; i++) {
            neuronIterator[i] = i;
        }

        if(layerIterator == 0){
            iterAlloc = Allocation::createSized(this->renderScriptObject,
                                                Element::U32(this->renderScriptObject), info.neuronCounts[0]);
            iterAlloc->copy1DFrom(neuronIterator);

            this->neuralNetworkRSInstance->bind_data(dataAllocation);

            //globals
            this->neuralNetworkRSInstance->forEach_globalMeansVars(iterAlloc);

            renderScriptObject->finish();
        }

        this->neuralNetworkRSInstance->set_layerNumber(layerIterator);
        this->neuralNetworkRSInstance->set_biasOffset(biasOffset);
        this->neuralNetworkRSInstance->set_weightOffset(weightOffset);
        this->neuralNetworkRSInstance->forEach_forwardWeights(iterAlloc);
        this->renderScriptObject->finish();


        iterAlloc = Allocation::createSized(this->renderScriptObject,
                                            Element::U32(this->renderScriptObject), info.neuronCounts[layerIterator +  1]);
        iterAlloc->copy1DFrom(neuronIterator);
        this->neuralNetworkRSInstance->forEach_forwardBias(iterAlloc);
        renderScriptObject->finish();

        biasOffset += info.neuronCounts[layerIterator];
        weightOffset += info.neuronCounts[layerIterator] * info.neuronCounts[layerIterator + 1];

    }

    neuralNetworkRSInstance->invoke_calculateSoftmax(info.neuronCounts[info.layerCount - 1]);

    float* result = new float[info.neuronCounts[info.layerCount - 1]];
    dataAllocation->copy1DRangeTo(0, info.neuronCounts[info.layerCount - 1], result);

    return result;
}

FeatureMatrix *RSNeuralNetwork::forwardAll(FeatureMatrix *data) {
    FeatureMatrix* result = new FeatureMatrix();
    result->init(data->getFramesNum(), info.neuronCounts[info.layerCount - 1]);

    float* inputData = new float[info.neuronCounts[0]];
    for(int frameNum = 0; frameNum < data->getFramesNum(); frameNum++){
        prepareInput(data, frameNum, inputData);
        result->getFeaturesMatrix()[frameNum] = this->forward(inputData);
    }

    return result;
}

void RSNeuralNetwork::prepareAllocations() {
    // Allocation for means - Renderscript
    sp<Allocation> meansAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                             info.neuronCounts[0]);
    meansAllocation->copy1DFrom(this->layerMeans[0]);
    neuralNetworkRSInstance->set_means(meansAllocation);
    //\
    //Alocation for variance - Renderscript
    sp<Allocation> varsAllocation = Allocation::createSized(this->renderScriptObject,
                                                            Element::F32(this->renderScriptObject),
                                                            info.neuronCounts[0]);
    varsAllocation->copy1DFrom(this->layerVars[0]);
    neuralNetworkRSInstance->set_vars(varsAllocation);
    //\
    //Allocation for biases - Renderscript
    sp<Allocation> biasesAllocation = Allocation::createSized(this->renderScriptObject,
                                                              Element::F32(this->renderScriptObject),
                                                              getTotalNeuronCount());
    unsigned int offset = 0;
    for(int i = 0; i < info.layerCount; ++i) {
        biasesAllocation->copy1DRangeFrom(offset,
                                          info.neuronCounts[i],
                                          this->layerBiases[i]);
        offset += info.neuronCounts[i];
    }

    neuralNetworkRSInstance->set_biases(biasesAllocation);
    //\
    //Allocation for weights - Renderscript
    sp<Allocation> weightsAllocation = Allocation::createSized(this->renderScriptObject,
                                                               Element::F32(this->renderScriptObject),
                                                               getTotalWeightsCount());

    offset = 0;
    int lastLayer;
    for(int i = 0; i < info.layerCount; ++i){
        if(i == 0)
            lastLayer = 1;
        else
            lastLayer = info.neuronCounts[i - 1];
        for(int j = 0; j < info.neuronCounts[i]; ++j){
            weightsAllocation->copy1DRangeFrom(offset + j * lastLayer,
                                               lastLayer,
                                               this->layerWeights[i][j]);
        }
        offset += lastLayer * info.neuronCounts[i];
    }

    neuralNetworkRSInstance->set_weights(weightsAllocation);
    //\
    //Allocation for neuron counters - Renderscript
    sp<Allocation> neuronCountAllocation = Allocation::createSized(this->renderScriptObject,
                                                                   Element::U32(this->renderScriptObject),
                                                                   info.layerCount);

    neuronCountAllocation->copy1DRangeFrom(0, info.layerCount, info.neuronCounts);
    neuralNetworkRSInstance->set_neuronCounts(neuronCountAllocation);
    //\
    //Allocation for neuron counters - Renderscript
    sp<Allocation> layerFunctionAllocation = Allocation::createSized(this->renderScriptObject,
                                                                   Element::U8(this->renderScriptObject),
                                                                   info.layerCount);

    neuronCountAllocation->copy1DRangeFrom(0, info.layerCount, layerFunction);
    neuralNetworkRSInstance->set_functions(neuronCountAllocation);
}

unsigned int RSNeuralNetwork::getTotalNeuronCount() {
    if(totalNeuronCount == 0) {
        for(int i = 0; i < info.layerCount; ++i) {
            this->totalNeuronCount += info.neuronCounts[i];
        }
    }
    return totalNeuronCount;
}

unsigned int RSNeuralNetwork::getTotalWeightsCount() {
    unsigned int total = info.neuronCounts[0];
    for(int i = 0; i < info.layerCount - 1; ++i) {
        total += info.neuronCounts[i] * info.neuronCounts[i + 1];
    }
    return total;
}

