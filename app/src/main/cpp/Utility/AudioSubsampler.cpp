//
// Created by Petr Flajsingr on 11/11/2017.
//


#include <AudioSubsampler.h>
#include <filt.h>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"


/**
 * Creates a new array with resampled audio from 48 kHz to 8 kHz.
 * @param data data in 48 kHz sample rate
 * @param dataLength length of valid data in an array
 * @return data in 8 kHz sample rate
 */
short *SpeechRecognition::Utility::AudioSubsampler::subsample48kHzto8kHz(short *data, int dataLength) {
    Filter lpFIRFiler(LPF, 8, 48.0, 4.0);
    for(int i = 0; i < dataLength; ++i) {
        data[i] = (short) lpFIRFiler.do_sample((double) data[i]);
    }

    const int RATIO = 6; // ratio for size difference
    int newDataLength = dataLength / RATIO + 1; //length of new array, +1 because of integer division

    short *resultArray = new short[newDataLength];


    int arrayIterator = 0;
    // copying every important data(short), given by RATIO
    for(int i = 0; i < dataLength; i += RATIO) {
        resultArray[arrayIterator] = data[i];
        arrayIterator++;
    }

    return resultArray;
}

SpeechRecognition::Utility::AudioSubsampler::AudioSubsampler() {
    lpFIRFilter = new Filter(LPF, 8, 48.0, 4.0);
}

SpeechRecognition::Utility::AudioSubsampler::~AudioSubsampler() {
    delete lpFIRFilter;
}

short *SpeechRecognition::Utility::AudioSubsampler::sample(short *data, int dataLength) {
    for(int i = 0; i < dataLength; ++i) {
        data[i] = (short) lpFIRFilter->do_sample((double) data[i]);
    }

    const int RATIO = 6; // ratio for size difference
    int newDataLength = dataLength / RATIO + 1; //length of new array, +1 because of integer division

    short *resultArray = new short[newDataLength];


    int arrayIterator = 0;
    // copying every important data(short), given by RATIO
    for(int i = 0; i < dataLength; i += RATIO) {
        resultArray[arrayIterator] = data[i];
        arrayIterator++;
    }

    return resultArray;
}
