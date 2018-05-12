//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_FEATURESMATRIXFLOAT_H
#define VOICERECOGNITION_FEATURESMATRIXFLOAT_H

#include <string>

namespace SpeechRecognition::Utility {
    /**
     * @brief Class used for storing matrix data
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class FeatureMatrix {
    private:
        float **featuresMatrix = NULL; //< matrix data

        int height = 0; //< height
        int width = 0; //< width
    public:
        int getWidth() const;

        int getHeight() const;

        /**
         * Allocates memory.
         * @param framesNum Number of frames
         * @param frameSize Size of each frame
         */
        void initialize(int height, int width);

        float **getFeaturesMatrix();

        void setFeaturesMatrixFrame(int frameNum, float *data);

        virtual ~FeatureMatrix();
    };
}


#endif //VOICERECOGNITION_FEATURESMATRIXFLOAT_H
