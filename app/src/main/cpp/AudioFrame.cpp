//
// Created by Petr Flajsingr on 11/11/2017.
//

#include "AudioFrame.h"
#include <math.h>

#include <string.h>
#include <android/log.h>
#include <vector>
#include <iterator>

bool AudioFrame::hammingCalculated = false;

/**
 * Applies hamming window to the given data. Length of data is defined by DATA_LENGTH.
 * Saves the data in hammingData array.
 * @param data input data
 */
void AudioFrame::applyHammingWindow(short* data) {
    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i){
        hammingData[i] = hammingCoef[i] * (data[i]/SHORT_MAX_FLOAT);
    }
}

/**
 * Calculates static hamming coefficients when the first object is created.
 */
AudioFrame::AudioFrame() {
    hammingData = new float[AUDIO_FRAME_LENGTH];
    fftData = NULL;
}

void AudioFrame::calcHammingCoef() {
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "calcHammingCoef(): calculating hamming coefs");
    const double PI_MUL_2 = M_PI*2;

    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i){
        hammingCoef[i] = (float) ( ALPHA - (BETA * cos(PI_MUL_2 * (double)i / (double)(AUDIO_FRAME_LENGTH - 1))));
    }
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "calcHammingCoef(): calculating hamming coefs DONE");
}

AudioFrame::~AudioFrame() {
    delete[] hammingData;
    free(fftData);
}

void AudioFrame::applyFFT(kiss_fftr_cfg *cfg) {
    this->fftData = (kiss_fft_cpx*)malloc((FFT_FRAME_LENGTH/2+1) * sizeof(kiss_fft_cpx));

    // adding padding to the fft input
    float fftInput[FFT_FRAME_LENGTH];
    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i)
        fftInput[i] = hammingData[i];
    for(int i = AUDIO_FRAME_LENGTH; i < FFT_FRAME_LENGTH; ++i)
        fftInput[i] = 0;

    kiss_fftr(*cfg, fftInput, this->fftData);

    // freeing not needed data
    delete[] hammingData;
    hammingData = NULL;
}
