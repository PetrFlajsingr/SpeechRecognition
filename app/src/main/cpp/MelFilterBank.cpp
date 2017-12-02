//
// Created by Petr Flajsingr on 30/11/2017.
//

#include "MelFilterBank.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <android/log.h>

//STATIC
float* MelFilterBank::melFBin = NULL;

float* MelFilterBank::melCBin = NULL;

int* MelFilterBank::melCinD = NULL;

FeaturesMatrixFloat MelFilterBank::mfb;

void dumpFloatArray(float* array, std::string path, int length){
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < length; ++i){
        std::stringstream ss;
        ss << array[i];
        out.write((ss.str() + ",").c_str(), ss.str().size()+1);
        out.write("\n", 1);
    }
    out.close();
}

void dumpIntArray(int* array, std::string path, int length){
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < length; ++i){
        std::stringstream ss;
        ss << array[i];
        out.write((ss.str() + ",").c_str(), ss.str().size()+1);
        out.write("\n", 1);
    }
    out.close();
}

void MelFilterBank::initStatic() {
    melFBin = initLinSpace(0, TARGET_SAMPLING_RATE / 2, FFT_FRAME_LENGTH / 2 + 1);
    dumpFloatArray(melFBin, "/sdcard/pokus/melfbin.txt", FFT_FRAME_LENGTH / 2 + 1);
    melVect(melFBin, FFT_FRAME_LENGTH / 2 + 1);
    dumpFloatArray(melFBin, "/sdcard/pokus/melfbin1.txt", FFT_FRAME_LENGTH / 2 + 1);

    melCBin = initLinSpace(melPoint(LOW_FREQ), melPoint(HIGH_FREQ), MEL_BANK_FRAME_LENGTH + 2);
    dumpFloatArray(melCBin, "/sdcard/pokus/melcbin.txt", MEL_BANK_FRAME_LENGTH + 2);

    melCinD = new int[MEL_BANK_FRAME_LENGTH + 2];

    for(int i = 0; i < MEL_BANK_FRAME_LENGTH + 2; ++i){
        melCinD[i] = (int)floor(melInvPoint(melCBin[i]) / TARGET_SAMPLING_RATE * FFT_FRAME_LENGTH) + 1;
    }
    dumpIntArray(melCinD, "/sdcard/pokus/melcind.txt", MEL_BANK_FRAME_LENGTH + 2);

    mfb.init(FFT_FRAME_LENGTH / 2 + 1, MEL_BANK_FRAME_LENGTH);

    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
        for(int j = melCinD[i]; j < melCinD[i + 1]; ++j){
            mfb.getFeaturesMatrix()[j][i] = (melCBin[i] - melFBin[j]) / (melCBin[i] - melCBin[i + 1]);
        }
        for(int j = melCinD[i + 1]; j < melCinD[i + 2]; ++j){
            mfb.getFeaturesMatrix()[j][i] = (melCBin[i + 2] - melFBin[j]) / (melCBin[i + 2] - melCBin[i + 1]);
        }
    }

    if(LOW_FREQ > 0.0 && (LOW_FREQ / TARGET_SAMPLING_RATE * FFT_FRAME_LENGTH + 0.5) > melCinD[0]){
        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
            mfb.getFeaturesMatrix()[melCinD[0]][i] = 0.0f;
        }
    }

    mfb.dumpResultToFile("/sdcard/pokus/melmfb.txt");
}


void MelFilterBank::deleteStatic() {
    delete[] melFBin;
    delete[] melCBin;
    delete[] melCinD;
}

float* MelFilterBank::initLinSpace(float min, float max, int n) {
    float* space = new float[n];
    for(int i = 0; i < n - 1; ++i){
        space[i] = min + i * (max - min) / (n - 1.0f);
    }
    space[n - 1] = max;
    return space;
}

void MelFilterBank::melVect(float *x, int length) {
    for(int i = 0; i < length; ++i){
        x[i] = (float)(1127.0 * log(1.0 + x[i] / 700.0));
    }
}

float MelFilterBank::melPoint(float x) {
    return (float)(1127.0 * log(1.0 + x /700.0));
}

float MelFilterBank::melInvPoint(float x) {
    return (float)((exp(x / 1127.0) - 1.0) * 700.0);
}
//\STATIC

MelFilterBank::MelFilterBank(){

}

void MelFilterBank::calculateMelBanks(int frameCount, kiss_fft_cpx** fftFrames) {
    melBankFrames = new FeaturesMatrixFloat();

    melBankFrames->init(frameCount, MEL_BANK_FRAME_LENGTH);

    float melBankFramesSum[MEL_BANK_FRAME_LENGTH] = {0.0};

    float melBankFrame[MEL_BANK_FRAME_LENGTH] = {0.0};

    for(int frameNum = 0; frameNum < frameCount; ++frameNum){
        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
            for(int j = 0; j < FFT_FRAME_LENGTH / 2; ++j){
                melBankFrame[i] +=
                        mfb.getFeaturesMatrix()[j][i] * (float)((pow(fftFrames[frameNum][j].r, 2))
                        + (pow(fftFrames[frameNum][j].i, 2)));
            }

            if(melBankFrame[i] < 1){
                melBankFrame[i] = 0.0;
            } else{
                melBankFrame[i] = (float)(log(melBankFrame[i]));
            }
            melBankFramesSum[i] += melBankFrame[i];
        }

        melBankFrames->setFeatureMatrixFrame(frameNum, melBankFrame);
    }

    for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
        melBankFramesSum[i] /= frameCount;
    }

    for(int frameNum = 0; frameNum < frameCount; ++frameNum){
        for(int i = 0; i < MEL_BANK_FRAME_LENGTH; ++i){
            melBankFrames->getFeaturesMatrix()[frameNum][i] -= melBankFramesSum[i];
        }
    }
}

MelFilterBank::~MelFilterBank() {
    delete melBankFrames;
}

void MelFilterBank::dumpResultToFile(std::string path) {
    melBankFrames->dumpResultToFile(path);
}




