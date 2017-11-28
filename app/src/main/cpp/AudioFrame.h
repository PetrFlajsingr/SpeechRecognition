//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <stddef.h>
#include <stdio.h>
#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftr.h"

class AudioFrame {
private:
    static const int DATA_LENGTH = 200; //< width 200 - sample rate is 8 kHz (8000*0.025(s) = 200)
    // hamming window coeficients
    static const double ALPHA = 25.0/46;
    static const double BETA = (1 - ALPHA);
    static float hammingCoef[DATA_LENGTH];
    static bool hammingCalculated;

    static void calcHammingCoef();
public:
    AudioFrame();

    virtual ~AudioFrame();

    float* hammingData; //< container for hammingWindow transformation

    kiss_fft_cpx* fftData;

    void applyHammingWindow(short* data);

    void applyFFT(kiss_fftr_cfg* cfg);
};


#endif //VOICERECOGNITION_AUDIOFRAME_H
