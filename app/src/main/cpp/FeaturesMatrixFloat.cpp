//
// Created by Petr Flajsingr on 30/11/2017.
//

#include "FeaturesMatrixFloat.h"
#include "constants.h"

float **FeaturesMatrixFloat::getFeaturesMatrix() {
    return featuresMatrix;
}

void FeaturesMatrixFloat::init(int framesNum, int frameSize) {
    if(featuresMatrix == nullptr || this->framesNum < framesNum || this->frameSize < frameSize){
        if(featuresMatrix != nullptr){
            for(int i = 0; i < framesNum; ++i){
                delete[] featuresMatrix[i];
            }
            delete[] featuresMatrix;
        }

        featuresMatrix = new float*[framesNum];
        for(int i = 0; i < framesNum; ++i){
            featuresMatrix[i] = new float[frameSize];
        }
    }
}

void FeaturesMatrixFloat::setFeatureMatrix(float **featureMatrix) {
    this->featuresMatrix = featureMatrix;
    if(this->featuresMatrix == nullptr){
        this->frameSize = 0;
        this->framesNum = 0;
    }
}

void FeaturesMatrixFloat::setFeatureMatrixFrame(int frameNum, float *data) {
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i) {
        this->featuresMatrix[frameNum][i] = data[i];
    }
}
