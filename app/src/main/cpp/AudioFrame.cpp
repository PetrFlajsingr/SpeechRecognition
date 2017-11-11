//
// Created by Petr Flajsingr on 11/11/2017.
//

#include "AudioFrame.h"
#include <math.h>

#include <string.h>

FILE* AudioFrame::test;
float AudioFrame::hammingCoef[DATA_LENGTH];
bool AudioFrame::hammingCalculated = false;


void AudioFrame::applyHammingWindow() {
    char array[10];

    for(int i = 0; i < DATA_LENGTH; ++i){
        hammingData[i] = hammingCoef[i] * data[i];
        sprintf(array, "%f", hammingData[i]);
        fwrite(array, 1, strlen(array), test);
        fwrite(",", 1, 1, test);
    }
    fwrite("\n", 1, 1, test);
}

//debug
void AudioFrame::openFile() {
    test = fopen("/sdcard/AAAhamming.txt", "w");
}

void AudioFrame::closeFile() {
    fclose(test);
}
//\debug

AudioFrame::AudioFrame() {
    if(hammingCalculated){
        hammingCalculated = true;
        calcHammingCoef();
    }
}

void AudioFrame::calcHammingCoef() {
    for(int i = 0; i < DATA_LENGTH; ++i){
        hammingCoef[i] = (float) ( ALPHA - (BETA * cos(M_2_PI * (double)i / (double)(DATA_LENGTH - 1))));
    }
}
