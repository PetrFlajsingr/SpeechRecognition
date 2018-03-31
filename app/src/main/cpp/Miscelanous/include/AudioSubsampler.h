//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef NATIVEAUDIO_AUDIOSUBSAMPLER_H
#define NATIVEAUDIO_AUDIOSUBSAMPLER_H


#include <stddef.h>

/**
 * Wrapper for static classes handling subsamoling to 8kHz sample rate for recognition.
 */
class AudioSubsampler {
public:
    static short* subsample48kHzto8kHz(short* data, int dataLength);

};


#endif //NATIVEAUDIO_AUDIOSUBSAMPLER_H
