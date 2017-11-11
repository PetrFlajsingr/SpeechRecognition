//
// Created by Petr Flajsingr on 11/11/2017.
//

#include "AudioSubsampler.h"


/**
 * Creates a new array with resampled audio from 48 kHz to 8 kHz.
 * @param data data in 48 kHz sample rate
 * @param dataLength length of valid data in an array
 * @return data in 8 kHz sample rate
 */
short *AudioSubsampler::subsample48kHzto8kHz(short *data, size_t dataLength) {
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
