//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <constants.h>
#include <kiss_fft.h>
#include <kiss_fftr.h>

namespace SpeechRecognition::Feature_Extraction {
    /**
     * @brief Class providing operations for segmented frames
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class AudioFrame {
    private:
        // hamming window coeficients
        static const double ALPHA;
        static const double BETA;
        static float hammingCoefficients[AUDIO_FRAME_LENGTH];

        float *hammingData; //< data after using hamming window

        kiss_fft_cpx *fftData;
    public:
        float *getHammingData() const;

        kiss_fft_cpx *getFftData() const;

        /**
         * Allocates memory for first operation (hamming window).
         */
        AudioFrame();

        virtual ~AudioFrame();

        /**
         * Applies hamming window to the given data. Length of data is defined by DATA_LENGTH.
         * Saves the data in hammingData array.
         * @param data input data
         */
        void applyHammingWindow(short *data);

        /**
         * Allocates memory for results of FFT. Calculates only first half of the data due to FFT symetry.
         * Deletes no longer necessary audio data.
         * @param cfg configuration for kiss_fftr function
         */
        void applyFFT(kiss_fftr_cfg *cfg);

        /**
         * Calculates hamming window coefficients.
         * coef(n) = alpha - beta * cos((2*pi*n)/(N - 1))
         */
        static void calculateHammingCoefficients();
    };

}


#endif //VOICERECOGNITION_AUDIOFRAME_H
