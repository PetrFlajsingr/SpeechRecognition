//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H


class FeaturesMatrixFloat {
private:
    float** featuresMatrix = nullptr; //< matrix data

    int framesNum = 0; //< height
    int frameSize = 0; //< width
public:
    void init(int framesNum, int frameSize);

    float **getFeaturesMatrix();

    void setFeatureMatrix(float **featureMatrix);

    void setFeatureMatrixFrame(int frameNum, float* data);

    virtual ~FeaturesMatrixFloat();
};


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
