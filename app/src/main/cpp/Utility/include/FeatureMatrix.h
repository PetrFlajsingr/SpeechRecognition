//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H

#include <string>

namespace SpeechRecognition::Utility {
    class FeatureMatrix {
    private:
        float **featuresMatrix = NULL; //< matrix data

        int height = 0; //< height
        int width = 0; //< width
    public:
        int getWidth() const;

        int getHeight() const;

        void initialize(int height, int width);

        float **getFeaturesMatrix();

        void setFeaturesMatrixFrame(int frameNum, float *data);

        virtual ~FeatureMatrix();
    };
}


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
