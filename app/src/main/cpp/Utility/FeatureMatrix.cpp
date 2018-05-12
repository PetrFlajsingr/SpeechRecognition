//
// Created by Petr Flajsingr on 30/11/2017.
//


#include <FeatureMatrix.h>
#include <constants.h>
#include <string>
#include <fstream>
#include <sstream>
#include <string>

float **SpeechRecognition::Utility::FeatureMatrix::getFeaturesMatrix() {
    return featuresMatrix;
}


void SpeechRecognition::Utility::FeatureMatrix::initialize(int height, int width) {
    if(featuresMatrix == NULL || this->height < height || this->width < width){
        if(featuresMatrix != NULL){
            for(int i = 0; i < height; ++i){
                delete[] featuresMatrix[i];
            }
            delete[] featuresMatrix;
        }

        featuresMatrix = new float*[height];
        for(int i = 0; i < height; ++i){
            featuresMatrix[i] = new float[width]();
        }
    }
    this->height = height;
    this->width = width;
}

void SpeechRecognition::Utility::FeatureMatrix::setFeaturesMatrixFrame(int frameNum, float *data) {
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i) {
        this->featuresMatrix[frameNum][i] = data[i];
    }
}

SpeechRecognition::Utility::FeatureMatrix::~FeatureMatrix() {
    for(int i = 0; i < height; ++i){
        delete[] featuresMatrix[i];
    }
}

int SpeechRecognition::Utility::FeatureMatrix::getHeight() const {
    return height;
}

int SpeechRecognition::Utility::FeatureMatrix::getWidth() const {
    return width;
}