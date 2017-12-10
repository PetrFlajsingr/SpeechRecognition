//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_MELFILTERBANK_H
#define VOICERECOGNITION_MELFILTERBANK_H

#include <string>
#include "constants.h"
#include "FeaturesMatrixFloat.h"
#include "kissfft/kiss_fft.h"

class MelFilterBank {
private:
    static float* melFBin;

    static float* melCBin;

    static int* melCinD;

    static float* initLinSpace(float min, float max, int n);

    static void melVect(float* x, int length);

    static float melPoint(float x);

    static float melInvPoint(float x);

    static FeaturesMatrixFloat mfb;

    FeaturesMatrixFloat* melBankFrames;
public:
    FeaturesMatrixFloat *getMelBankFrames() const;

public:
    MelFilterBank();

    virtual ~MelFilterBank();

    static void initStatic();

    static void deleteStatic();

    void calculateMelBanks(int frameCount, kiss_fft_cpx** fftFrames);

    void dumpResultToFile(std::string path);
};


#endif //VOICERECOGNITION_MELFILTERBANK_H
