//
// Created by Petr Flajsingr on 16/12/2017.
//

#include <string>
#include <fstream>
#include <RenderScript.h>
#include <sstream>
#include "RSNeuralNetworkOld.h"
#include "constants.h"

/**
 * Requires cache directory of the application for Renderscript.
 * @param filepath path to neural network file
 */
RSNeuralNetwork::RSNeuralNetwork(std::string filepath, const char* cacheDir) {
    NNInitInfo initInfo;
    initInfo.layerCount = 3;
    initInfo.neuronCounts = new unsigned int[initInfo.layerCount];
    initInfo.neuronCounts[0] = 360;
    initInfo.neuronCounts[1] = 300;
    initInfo.neuronCounts[2] = 300;
    initInfo.neuronCounts[3] = 30;
    this->info = initInfo;
    this->outputNodeCount = 30;

    this->allocateMemory();

    this->loadFromFile(filepath);

    this->renderScriptObject = new RS();
    this->renderScriptObject->init(cacheDir);
    this->neuralNetworkRSInstance = new ScriptC_RSneuralnetwork(this->renderScriptObject);

    this->prepareAllocations();
}

/**
 * Memory allocation for neural network
 */
void RSNeuralNetwork::allocateMemory() {
    //layers init
    this->layerBiases = new float*[info.layerCount];
    this->layerVars = new float*[1];
    this->layerMeans = new float*[1];
    this->layerWeights = new float**[info.layerCount];
    //\layers
    this->layerCount = info.layerCount;

    this->layerVars[0] = new float[info.neuronCounts[0]];
    this->layerMeans[0] = new float[info.neuronCounts[0]];

    for(unsigned int i = 0; i < info.layerCount; ++i){
        this->layerBiases[i] = new float[info.neuronCounts[i + 1]];
        this->layerWeights[i] = new float*[info.neuronCounts[i + 1]];
        //weight allocation for dense NN
        for(unsigned int j = 0; j < info.neuronCounts[i + 1]; ++j) {
            this->layerWeights[i][j] = new float[info.neuronCounts[i]];
        }
    }
}

RSNeuralNetwork::~RSNeuralNetwork() {
    for(unsigned int i = 0; i < this->layerCount; ++i){
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

/**
 * Loading of NN from file
 * @param filepath path to file
 */
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

/**
 * Memory allocation for RenderScript
 */
void RSNeuralNetwork::prepareAllocations() {
    sp<Allocation> meansAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                             info.neuronCounts[0]);
    meansAllocation->copy1DFrom(this->layerMeans[0]);
    neuralNetworkRSInstance->set_means(meansAllocation);

    sp<Allocation> varsAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                            info.neuronCounts[0]);
    varsAllocation->copy1DFrom(this->layerVars[0]);
    neuralNetworkRSInstance->set_vars(varsAllocation);

    sp<Allocation> biasesAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                              info.neuronCounts[1]
                                                              + info.neuronCounts[2]
                                                              + this->outputNodeCount);
    biasesAllocation->copy1DRangeFrom(0,
                                      info.neuronCounts[1],
                                      this->layerBiases[0]);
    biasesAllocation->copy1DRangeFrom(info.neuronCounts[1],
                                      info.neuronCounts[2],
                                      this->layerBiases[1]);
    biasesAllocation->copy1DRangeFrom(info.neuronCounts[1] + info.neuronCounts[2],
                                      this->outputNodeCount,
                                      this->layerBiases[2]);

    neuralNetworkRSInstance->set_biases(biasesAllocation);

    sp<Allocation> weightsAllocation = Allocation::createSized(this->renderScriptObject,
                                                             Element::F32(this->renderScriptObject),
                                                               info.neuronCounts[0] * info.neuronCounts[1]
                                                             + info.neuronCounts[1] * info.neuronCounts[2]
                                                             + info.neuronCounts[2] * this->outputNodeCount);
    int offset = 0;
    for(int i = 0; i < this->layerCount; ++i){
        for(int j = 0; j < info.neuronCounts[i + 1]; ++j){
            weightsAllocation->copy1DRangeFrom(offset + j * info.neuronCounts[i],
                                               info.neuronCounts[i],
                                               this->layerWeights[i][j]);
        }
        offset += info.neuronCounts[i] * info.neuronCounts[i + 1];
    }

    neuralNetworkRSInstance->set_weights(weightsAllocation);

    sp<Allocation> neuronCountAllocation = Allocation::createSized(this->renderScriptObject,
                                                                   Element::U32(this->renderScriptObject),
                                                                   3);
    neuronCountAllocation->copy1DRangeFrom(0, 3, info.neuronCounts);
    neuralNetworkRSInstance->set_neuronCounts(neuronCountAllocation);
}

void dumpArray(std::string path, float* arr, int size){
    std::ofstream out;
    out.open(path);
    for(int i = 0; i < size; ++i){
        std::stringstream ss;
        ss << arr[i];
        out.write((ss.str() + ",").c_str(), ss.str().size()+1);
    }
    out.close();
}

/**
 * Forward data through NN.
 * @param data input data
 * @return NN output
 */
float* RSNeuralNetwork::forward(float* data) {
    sp<Allocation> dataAllocation = Allocation::createSized(this->renderScriptObject,
                                                            Element::F32(this->renderScriptObject),
                                                            info.neuronCounts[0]);

    dataAllocation->copy1DFrom(data);

    int neuronIterator[360];
    for(int i = 0; i < 360; i++) {
        neuronIterator[i] = i;
    }

    sp<Allocation> iterAlloc = Allocation::createSized(this->renderScriptObject,
                                                       Element::U32(this->renderScriptObject), 360);
    iterAlloc->copy1DFrom(neuronIterator);

    this->neuralNetworkRSInstance->bind_data(dataAllocation);

    //globals
    this->neuralNetworkRSInstance->forEach_globalMeansVars(iterAlloc);
    //this->neuralNetworkRSInstance->forEach_forwardInputLayer(iterAlloc);

    renderScriptObject->finish();

    // first layer
    this->neuralNetworkRSInstance->set_layerNumber(0);
    this->neuralNetworkRSInstance->set_biasOffset(0);
    this->neuralNetworkRSInstance->set_weightOffset(0);
    this->neuralNetworkRSInstance->forEach_forwardWeights(iterAlloc);
    this->renderScriptObject->finish();

    iterAlloc = Allocation::createSized(this->renderScriptObject,
                                        Element::U32(this->renderScriptObject), 300);
    iterAlloc->copy1DFrom(neuronIterator);
    this->neuralNetworkRSInstance->forEach_forwardBias(iterAlloc);
    renderScriptObject->finish();


    //2nd layer
    this->neuralNetworkRSInstance->set_layerNumber(1);
    this->neuralNetworkRSInstance->set_biasOffset(this->info.neuronCounts[1]);
    this->neuralNetworkRSInstance->set_weightOffset(this->info.neuronCounts[0] * this->info.neuronCounts[1]);
    this->neuralNetworkRSInstance->forEach_forwardWeights(iterAlloc);
    this->renderScriptObject->finish();

    this->neuralNetworkRSInstance->forEach_forwardBias(iterAlloc);
    renderScriptObject->finish();

    iterAlloc = Allocation::createSized(this->renderScriptObject,
                                        Element::U32(this->renderScriptObject), 30);
    iterAlloc->copy1DFrom(neuronIterator);

    // 3rd layer
    this->neuralNetworkRSInstance->set_layerNumber(2);
    this->neuralNetworkRSInstance->set_biasOffset(this->info.neuronCounts[1] + this->info.neuronCounts[2]);
    this->neuralNetworkRSInstance->set_weightOffset(this->info.neuronCounts[0] * this->info.neuronCounts[1]
                                                    + this->info.neuronCounts[1] * this->info.neuronCounts[2]);
    this->neuralNetworkRSInstance->forEach_forwardWeights(iterAlloc);
    this->renderScriptObject->finish();

    this->neuralNetworkRSInstance->forEach_forwardBias(iterAlloc);
    renderScriptObject->finish();

    float* result = new float[this->outputNodeCount];
    dataAllocation->copy1DRangeTo(0, 30, result);

    return result;
}

FeatureMatrix *RSNeuralNetwork::forwardAll(FeatureMatrix *data) {
    FeatureMatrix* result = new FeatureMatrix();
    result->init(data->getFramesNum(), 30);

    float* inputData = new float[360];
    for(int frameNum = 0; frameNum < data->getFramesNum(); frameNum++){
        prepareInput(data, frameNum, inputData);
        result->getFeaturesMatrix()[frameNum] = this->forward(inputData);
    }

    return result;
}

/**
 * Data preparation for input
 * @param data
 * @param centerFrame
 * @param out
 */
void RSNeuralNetwork::prepareInput(FeatureMatrix *data, int centerFrame, float* out) {
    const int neig = 7;
    int frameNum = 0;
    for(int i = 0; i < neig * 2 + 1; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = centerFrame + i - neig;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > data->getFramesNum() - 1){
                frameNum = data->getFramesNum() - 1;
            }
            out[i + (neig * 2 + 1) * j] = data->getFeaturesMatrix()[frameNum][j];
        }
    }
}
