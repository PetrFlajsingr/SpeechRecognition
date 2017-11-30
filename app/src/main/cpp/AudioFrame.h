//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <stddef.h>
#include <stdio.h>
#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftr.h"
#include "constants.h"

class AudioFrame {
private:
    // hamming window coeficients
    static const double ALPHA = 0.54;
    static const double BETA = (1 - ALPHA);
    static float hammingCoef[AUDIO_FRAME_LENGTH];

public:
    AudioFrame();

    virtual ~AudioFrame();

    float* hammingData; //< container for hammingWindow transformation

    kiss_fft_cpx* fftData;

    void applyHammingWindow(short* data);

    void applyFFT(kiss_fftr_cfg* cfg);

    static void calcHammingCoef();
};


#endif //VOICERECOGNITION_AUDIOFRAME_H
