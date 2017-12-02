//
// Created by Petr Flajsingr on 11/11/2017.
//

#include "AudioFrame.h"
#include <math.h>

#include <string.h>
#include <android/log.h>

float AudioFrame::hammingCoef[AUDIO_FRAME_LENGTH];

/**
 * Applies hamming window to the given data. Length of data is defined by DATA_LENGTH.
 * Saves the data in hammingData array.
 * @param data input data
 */
void AudioFrame::applyHammingWindow(short* data) {
    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i){
        hammingData[i] = hammingCoef[i] * (data[i]/*SHORT_MAX_FLOAT*/);
    }
}

/**
 * Allocates memory for first operation (hamming window).
 */
AudioFrame::AudioFrame() {
    hammingData = new float[AUDIO_FRAME_LENGTH];
    fftData = NULL;
}

/**
 * Calculates hamming window coefficients.
 * coef(n) = alpha - beta * cos((2*pi*n)/(N - 1))
 */
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

/**
 * Allocates memory for results of FFT. Calculates only first half of the data due to FFT symetry.
 * Deletes no longer necessary audio data.
 * @param cfg configuration for kiss_fftr function
 */
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

    /*for(int i = 0; i < FFT_FRAME_LENGTH / 2 + 1; ++i){
        fftData[i].r *= SHORT_MAX_FLOAT;
        fftData[i].i *= SHORT_MAX_FLOAT;
    }*/
}

kiss_fft_cpx *AudioFrame::getFftData() const {
    return fftData;
}

float *AudioFrame::getHammingData() const {
    return hammingData;
}
