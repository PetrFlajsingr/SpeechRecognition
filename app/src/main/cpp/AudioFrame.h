//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef VOICERECOGNITION_AUDIOFRAME_H
#define VOICERECOGNITION_AUDIOFRAME_H


#include <stddef.h>
#include <stdio.h>

class AudioFrame {
private:
    static const size_t DATA_LENGTH = 200; //< width 200 - sample rate is 8 kHz (8000*0.025(s) = 200)
    // hamming window coeficients
    static const double ALPHA = 25.0/46;
    static const double BETA = (1 - ALPHA);
    static float hammingCoef[DATA_LENGTH];
    static bool hammingCalculated;

    static FILE* test;

    static void calcHammingCoef();
public:
    AudioFrame();

    short hammingData[DATA_LENGTH]; //< container for hammingWindow transformation

    static void openFile();

    static void closeFile();

    void applyHammingWindow(short* data);
};


#endif //VOICERECOGNITION_AUDIOFRAME_H
