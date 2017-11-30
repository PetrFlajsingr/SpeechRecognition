//
// Created by Petr Flajsingr on 30/11/2017.
//

#include "FeaturesMatrixFloat.h"

float **FeaturesMatrixFloat::getFeatureMatrix() {
    return featureMatrix;
}

void FeaturesMatrixFloat::init(int framesNum, int frameSize) {
    if(featureMatrix == nullptr || this->framesNum < framesNum || this->frameSize < frameSize){
        if(featureMatrix != nullptr){
            for(int i = 0; i < framesNum; ++i){
                delete[] featureMatrix[i];
            }
            delete[] featureMatrix;
        }

        featureMatrix = new float*[framesNum];
        for(int i = 0; i < framesNum; ++i){
            featureMatrix[i] = new float[frameSize];
        }
    }
}

void FeaturesMatrixFloat::setFeatureMatrix(float **featureMatrix) {
    this->featureMatrix = featureMatrix;
    if(this->featureMatrix == nullptr){
        this->frameSize = 0;
        this->framesNum = 0;
    }
}
