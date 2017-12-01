//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H


#include <cstdio>
#include <string>

class FeaturesMatrixFloat {
private:
    float** featuresMatrix = NULL; //< matrix data

    int framesNum = 0; //< height
    int frameSize = 0; //< width
public:
    int getFrameSize() const;


    int getFramesNum() const;

    void init(int framesNum, int frameSize);

    float **getFeaturesMatrix();

    void setFeatureMatrix(float **featureMatrix);

    void setFeatureMatrixFrame(int frameNum, float* data);

    virtual ~FeaturesMatrixFloat();

    void dumpResultToFile(std::string path);
};


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
