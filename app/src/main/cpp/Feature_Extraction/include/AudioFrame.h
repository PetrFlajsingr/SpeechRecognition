//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <stddef.h>
#include <stdio.h>
#include "../../kissfft/kiss_fft.h"
#include "../../kissfft/kiss_fftr.h"
#include "constants.h"

class AudioFrame {
private:
    // hamming window coeficients
    static const double ALPHA;
    static const double BETA;
    static float hammingCoef[AUDIO_FRAME_LENGTH];

    float* hammingData; //< data after using hamming window

    kiss_fft_cpx* fftData;
public:
    float *getHammingData() const;

    kiss_fft_cpx *getFftData() const;

    AudioFrame();

    virtual ~AudioFrame();

    void applyHammingWindow(short* data);

    void applyFFT(kiss_fftr_cfg* cfg);

    static void calcHammingCoef();
};


#endif //VOICERECOGNITION_AUDIOFRAME_H
