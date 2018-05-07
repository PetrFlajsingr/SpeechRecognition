//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <constants.h>
#include <kiss_fft.h>
#include <kiss_fftr.h>

namespace SpeechRecognition::Feature_Extraction {
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

        AudioFrame();

        virtual ~AudioFrame();

        void applyHammingWindow(short *data);

        void applyFFT(kiss_fftr_cfg *cfg);

        static void calculateHammingCoefficients();
    };

}


#endif //VOICERECOGNITION_AUDIOFRAME_H
