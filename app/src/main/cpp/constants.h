//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_CONSTANTS_H
#define VOICERECOGNITION_CONSTANTS_H


#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"
#define SHORT_MAX_FLOAT (32768.0f)

const int AUDIO_FRAME_LENGTH = 200;
const int FFT_FRAME_LENGTH = 256;
const int MEL_BANK_FRAME_LENGTH = 24;

const int TARGET_SAMPLING_RATE = 8000;

const float LOW_FREQ = 64.0;
const float HIGH_FREQ = 3800.0;

#endif //VOICERECOGNITION_CONSTANTS_H
