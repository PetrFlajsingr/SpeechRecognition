//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_MELFILTERBANK_H
#define VOICERECOGNITION_MELFILTERBANK_H

#include "constants.h"
#include "FeaturesMatrixFloat.h"
#include "kissfft/kiss_fft.h"

class MelFilterBank {
private:
    static float* melFBin = nullptr;

    static float* melCBin = nullptr;

    static int* melCinD = nullptr;

    static float* initLinSpace(float min, float max, int n);

    static void melVect(float* x, int length);

    static float melPoint(float x);

    static float melInvPoint(float x);

    static FeaturesMatrixFloat mfb;

public:
    MelFilterBank();

    static void initStatic();

    void calculateMelBanks(int frameCount, kiss_fft_cpx** fftFrames);
};


#endif //VOICERECOGNITION_MELFILTERBANK_H