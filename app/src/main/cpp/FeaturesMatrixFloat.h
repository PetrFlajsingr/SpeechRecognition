//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H


class FeaturesMatrixFloat {
private:
    float** featureMatrix = nullptr;
public:
    void setFeatureMatrix(float **featureMatrix);

private:
    int framesNum = 0;
    int frameSize = 0;

public:
    void init(int framesNum, int frameSize);

    float **getFeatureMatrix();
};


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
