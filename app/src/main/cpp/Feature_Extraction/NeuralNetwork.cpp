//
// Created by Petr Flajsingr on 21/11/2017.
//


#include <string>
#include <NeuralNetwork.h>
#include <fstream>
#include <constants.h>
#include <cmath>

NeuralNetwork::NeuralNetwork(std::string filepath) {
    NNInitInfo initInfo;
    initInfo.layerCount = 3;
    initInfo.neuronCounts = new unsigned int[initInfo.layerCount];
    initInfo.neuronCounts[0] = 360;
    initInfo.neuronCounts[1] = 300;
    initInfo.neuronCounts[2] = 300;
    this->outputNodeCount = 30;

    this->allocateMemory(initInfo);

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

NeuralNetwork::~NeuralNetwork() {
    for(unsigned int i = 0; i < this->info.layerCount; ++i){
        delete[] this->layerBiases[i];
        if(i < this->info.layerCount - 1)
            delete[] this->layerData[i];
        for(unsigned int j = 0; j < this->weightCounts[i]; ++j) {
            delete[] this->layerWeights[i][j];
        }
        delete[] this->layerWeights[i];
    }
    delete[] this->layerVars[0];
    delete[] this->layerMeans[0];

    delete[] this->weightCounts;
    delete[] this->dataVector;
    delete[] this->dataVectorNext;
    delete[] this->layerBiases;
    delete[] this->layerVars;
    delete[] this->layerMeans;
    delete[] this->layerData;
    delete[] this->layerWeights;
}

void NeuralNetwork::setFeatureMatrix(FeatureMatrix *frames) {
    this->frames = frames;
    this->frameCount = frames->getFramesNum();
}

void NeuralNetwork::forward() {
    FeatureMatrix* bottleneckMatrix = new FeatureMatrix();
    bottleneckMatrix->init(this->frameCount, this->outputNodeCount);

    int frameNum = 0;

    for(int i = 0; i < this->neighborFrames * 2 + 1; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = i - this->neighborFrames;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > this->frameCount - 1){
                frameNum = this->frameCount - 1;
            }
            this->dataVectorNext[i + (this->neighborFrames * 2 + 1) * j] = this->frames->getFeaturesMatrix()[frameNum][j];
        }
    }
    memcpy(this->dataVector, this->dataVectorNext, this->dataVectorLength * sizeof(float));

    for(int currentFrame = 0; currentFrame < frameCount; ++currentFrame){
        //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN: curFrame: %d", currentFrame);
        //input layer
        addVectors(this->dataVector, this->layerMeans[0], this->dataVectorLength);
        mulVectors(this->dataVector, this->layerVars[0], this->dataVectorLength);
        vectorMulMatrix(this->dataVector, this->dataVectorLength,
                        this->layerWeights[0], this->weightCounts[0], this->layerData[0]);
        addVectors(this->dataVector, this->layerBiases[0], this->dataVectorLength);
        //inner layers
        unsigned int layerIterator;
        for(layerIterator = 1; layerIterator < this->layerCount - 1; ++layerIterator) {
            sigmoid(this->layerData[layerIterator - 1], this->dataVectorLength);
            vectorMulMatrix(this->layerData[layerIterator - 1], this->dataVectorLength,
                            this->layerWeights[layerIterator], this->weightCounts[layerIterator], this->layerData[layerIterator]);
            addVectors(this->layerData[layerIterator - 1], this->layerBiases[layerIterator], this->dataVectorLength);
        }

        //clearOutputNode layer
        sigmoid(this->layerData[layerIterator - 1], this->dataVectorLength);
        vectorMulMatrix(this->layerData[layerIterator - 1], this->dataVectorLength,
                        this->layerWeights[layerIterator], this->weightCounts[layerIterator],
                        bottleneckMatrix->getFeaturesMatrix()[currentFrame]);
        addVectors(bottleneckMatrix->getFeaturesMatrix()[currentFrame], this->layerBiases[layerIterator], bottleneckMatrix->getFrameSize());

        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; i++){
            memcpy(this->dataVector + i * ((this->neighborFrames * 2 + 1) + 1),
                   this->dataVectorNext + i * ((this->neighborFrames * 2 + 1) + 1),
                   this->dataVectorLength - 1);
            frameNum = currentFrame + this->neighborFrames + 1;
            if(frameNum > (frameCount - 1)){
                frameNum = frameCount - 1;
            }
            this->dataVector[(i + 1) * (this->neighborFrames * 2 + 1) - 1] = this->frames->getFeaturesMatrix()[frameNum][i];
        }
        memcpy(this->dataVectorNext, this->dataVector, this->dataVectorLength);
    }
    //bottleneckMatrix->dumpResultToFile("/sdcard/AAAAnnoutput.txt");
}

void NeuralNetwork::addVectors(float *dest, float *toAdd, unsigned int arrayLength) {
    float temp;
    for(unsigned int i = 0; i < arrayLength; ++i){
        temp = dest[i];
        temp = toAdd[i];
        dest[i] += toAdd[i];
        temp = dest[i];
    }
}

void NeuralNetwork::mulVectors(float *dest, float *toMul, unsigned int arrayLength) {
    float temp;
    for(unsigned int i = 0; i < arrayLength; ++i){
        temp = dest[i];
        temp = toMul[i];
        dest[i] *= toMul[i];
        temp = dest[i];
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
    float temp;
    for(int j = 0; j < matrixWidth; ++j){
        result[j] = 0;
        for(int i = 0; i < vectorLength; ++i){
            temp = result[j];
            temp = vector[i];
            temp = matrix[j][i];
            result[j] = result[j] + vector[i] * matrix[j][i];
            temp = result[j];
        }
    }
}

void NeuralNetwork::allocateMemory(NNInitInfo initInfo) {
//layers init
    this->layerBiases = new float*[initInfo.layerCount];
    this->layerVars = new float*[1];
    this->layerMeans = new float*[1];
    this->layerWeights = new float**[initInfo.layerCount];
    this->layerData = new float*[initInfo.layerCount - 1];
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
            this->layerData[i] = new float[initInfo.neuronCounts[i + 1]];
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[initInfo.neuronCounts[i + 1]];
            }
        }else{
            // 1 for clearOutputNode neurons
            this->weightCounts[i] = 30;
            for(unsigned int j = 0; j < initInfo.neuronCounts[i]; ++j) {
                this->layerWeights[i][j] = new float[30];
            }
        }
    }

    this->dataVectorLength = (this->neighborFrames * 2 + 1) * MEL_BANK_FRAME_LENGTH;

    this->dataVector = new float[this->dataVectorLength];
    this->dataVectorNext = new float[this->dataVectorLength];
    //\neuron init
    this->info = initInfo;
}


