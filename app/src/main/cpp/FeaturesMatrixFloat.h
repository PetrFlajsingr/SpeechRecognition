//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H


class FeaturesMatrixFloat {
private:
    float** featuresMatrix = nullptr;
public:
    void setFeatureMatrix(float **featureMatrix);

    void setFeatureMatrixFrame(int frameNum, float* data);

private:
    int framesNum = 0;
    int frameSize = 0;

public:
    void init(int framesNum, int frameSize);

    float **getFeaturesMatrix();
};


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
