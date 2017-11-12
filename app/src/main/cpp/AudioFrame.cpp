//
// Created by Petr Flajsingr on 11/11/2017.
//

#include "AudioFrame.h"
#include <math.h>

#include <string.h>
#include <android/log.h>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"

FILE* AudioFrame::test;
float AudioFrame::hammingCoef[DATA_LENGTH];
bool AudioFrame::hammingCalculated = false;


void AudioFrame::applyHammingWindow(short* data) {
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
    if(!hammingCalculated){
        hammingCalculated = true;
        calcHammingCoef();
    }
}

void AudioFrame::calcHammingCoef() {
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "calculating coefs");
    FILE* testfile = fopen("/sdcard/AAAAAHAMMCOEFS.txt", "w");
    char array[10];
    const double PI_MUL_2 = M_PI*2;

    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "PI2: %e ALPHA: %e BETA: %e", PI_MUL_2, ALPHA, BETA);

    for(int i = 0; i < DATA_LENGTH; ++i){
        hammingCoef[i] = (float) ( ALPHA - (BETA * cos(PI_MUL_2 * (double)i / (double)(DATA_LENGTH - 1))));
        sprintf(array, "%f", hammingCoef[i]);
        fwrite(array, 1, strlen(array), testfile);
        fwrite(",", 1, 1, testfile);
    }
    fclose(testfile);
}
