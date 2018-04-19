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

/**
 * Allocates memory.
 * @param framesNum Number of frames
 * @param frameSize Size of each frame
 */
void SpeechRecognition::Utility::FeatureMatrix::init(int framesNum, int frameSize) {
    if(featuresMatrix == NULL || this->framesNum < framesNum || this->frameSize < frameSize){
        if(featuresMatrix != NULL){
            for(int i = 0; i < framesNum; ++i){
                delete[] featuresMatrix[i];
            }
            delete[] featuresMatrix;
        }

        featuresMatrix = new float*[framesNum];
        for(int i = 0; i < framesNum; ++i){
            featuresMatrix[i] = new float[frameSize]();
        }
    }
    this->framesNum = framesNum;
    this->frameSize = frameSize;
}

void SpeechRecognition::Utility::FeatureMatrix::setFeatureMatrix(float **featureMatrix) {
    this->featuresMatrix = featureMatrix;
    if(this->featuresMatrix == NULL){
        this->frameSize = 0;
        this->framesNum = 0;
    }
}

void SpeechRecognition::Utility::FeatureMatrix::setFeatureMatrixFrame(int frameNum, float *data) {
    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i) {
        this->featuresMatrix[frameNum][i] = data[i];
    }
}

SpeechRecognition::Utility::FeatureMatrix::~FeatureMatrix() {
    for(int i = 0; i < framesNum; ++i){
        delete[] featuresMatrix[i];
    }
}

int SpeechRecognition::Utility::FeatureMatrix::getFramesNum() const {
    return framesNum;
}

int SpeechRecognition::Utility::FeatureMatrix::getFrameSize() const {
    return frameSize;
}

void SpeechRecognition::Utility::FeatureMatrix::dumpResultToFile(std::string path) {
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < this->getFramesNum(); ++i){
        for(int j = 0; j < this->getFrameSize(); ++j){
            std::stringstream ss;
            ss << this->getFeaturesMatrix()[i][j];
            out.write((ss.str() + ",").c_str(), ss.str().size()+1);
        }
        out.write("\n", 1);
    }
    out.close();
}

void SpeechRecognition::Utility::FeatureMatrix::setFramesNum(int framesNum) {
    FeatureMatrix::framesNum = framesNum;
}