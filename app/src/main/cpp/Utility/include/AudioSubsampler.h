//
// Created by Petr Flajsingr on 11/11/2017.
//

#ifndef NATIVEAUDIO_AUDIOSUBSAMPLER_H
#define NATIVEAUDIO_AUDIOSUBSAMPLER_H

#include <filt.h>

namespace SpeechRecognition::Utility {
    /**
     * @brief Wrapper for static classes handling subsampling to 8kHz sample rate for recognition.
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class AudioSubsampler {
    private:
        Filter *lpFIRFilter; //< low pass filter
    public:
        AudioSubsampler();

        virtual ~AudioSubsampler();

        /**
         * Subsamples given audio data
         * @param data pcm 16bit little-endian, 48 kHz audio
         * @param dataLength length of input data
         * @return subsampled data
         */
        short *subSample(short *data, int dataLength);
    };
}

#endif //NATIVEAUDIO_AUDIOSUBSAMPLER_H
