//
// Created by Petr Flajsingr on 11/11/2017.
//

#include <stdio.h>
#include <cmath>
#include <android/log.h>
#include "AudioSubsampler.h"

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"


/**
 * Creates a new array with resampled audio from 48 kHz to 8 kHz.
 * @param data data in 48 kHz sample rate
 * @param dataLength length of valid data in an array
 * @return data in 8 kHz sample rate
 */
short *AudioSubsampler::subsample48kHzto8kHz(short *data, int dataLength) {
    // apply low pass filter to minimize aliasing
    lowPassFilter(data, dataLength, 48000);

    const size_t RATIO = 6; // ratio for size difference
    size_t newDataLength = dataLength/RATIO + 1; //length of new array, +1 because of integer division

    short* resultArray = new short[newDataLength];

    int arrayIterator = 0;
    // copying every important data(short), given by RATIO
    for(int i = 0; i < dataLength; i += RATIO){
        resultArray[arrayIterator] = data[i];
        arrayIterator++;
    }

    return resultArray;
}

/**
 * Applies low pass filter to avoid aliasing while downsampling.
 * @param data data in any sample rate
 * @param dataLength length of data in an array
 * y[i] := ß * x[i] + (1-ß) * y[i-1]
 */
void AudioSubsampler::lowPassFilter(short *data, int dataLength, int sampleRate) {
    const int CUTOFF = 4000;
    float RC = 1.0/(CUTOFF * 2 * M_PI);
    float dt = 1.0/sampleRate;
    float alpha = dt/(RC + dt);
    for(size_t i = 1; i < dataLength; ++i) {
        data[i] = (short) (data[i - 1] + (alpha * (data[i] - data[i - 1])));
    }
}
