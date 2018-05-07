//
// Created by Petr Flajsingr on 11/11/2017.
//


#include <AudioSubsampler.h>
#include <filt.h>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"


SpeechRecognition::Utility::AudioSubsampler::AudioSubsampler() {
    lpFIRFilter = new Filter(LPF, 8, 48.0, 4.0);
}

SpeechRecognition::Utility::AudioSubsampler::~AudioSubsampler() {
    delete lpFIRFilter;
}

short *SpeechRecognition::Utility::AudioSubsampler::subSample(short *data, int dataLength) {
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
