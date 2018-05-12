//
// Created by Petr Flajsingr on 11/11/2017.
//


#include <constants.h>
#include <AudioFrame.h>


float SpeechRecognition::Feature_Extraction::AudioFrame::hammingCoefficients[AUDIO_FRAME_LENGTH];
const double SpeechRecognition::Feature_Extraction::AudioFrame::ALPHA = 0.54;
const double SpeechRecognition::Feature_Extraction::AudioFrame::BETA = (1 - ALPHA);

void SpeechRecognition::Feature_Extraction::AudioFrame::applyHammingWindow(short* data) {
    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i){
        hammingData[i] = hammingCoefficients[i] * (data[i]);
    }
}

SpeechRecognition::Feature_Extraction::AudioFrame::AudioFrame() {
    hammingData = new float[AUDIO_FRAME_LENGTH];
    fftData = NULL;
}

void SpeechRecognition::Feature_Extraction::AudioFrame::calculateHammingCoefficients() {
    const double PI_MUL_2 = M_PI*2;

    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i){
        hammingCoefficients[i] = (float) ( ALPHA - (BETA * cos(PI_MUL_2 * (double)i / (double)(AUDIO_FRAME_LENGTH - 1))));
    }
}

SpeechRecognition::Feature_Extraction::AudioFrame::~AudioFrame() {
    delete[] hammingData;
    delete[] fftData;
}

void SpeechRecognition::Feature_Extraction::AudioFrame::applyFFT(kiss_fftr_cfg *cfg) {
    int a = 10;
    if(a)
        this->fftData = new kiss_fft_cpx[FFT_FRAME_LENGTH/2+1];

    // adding padding to the fft input
    float fftInput[FFT_FRAME_LENGTH];
    for(int i = 0; i < AUDIO_FRAME_LENGTH; ++i)
        fftInput[i] = hammingData[i];
    for(int i = AUDIO_FRAME_LENGTH; i < FFT_FRAME_LENGTH; ++i)
        fftInput[i] = 0;

    kiss_fftr(*cfg, fftInput, this->fftData);
}

kiss_fft_cpx *SpeechRecognition::Feature_Extraction::AudioFrame::getFftData() const {
    return fftData;
}

float *SpeechRecognition::Feature_Extraction::AudioFrame::getHammingData() const {
    return hammingData;
}
