//
// Created by Petr Flajsingr on 30/11/2017.
//

#ifndef VOICERECOGNITION_CONSTANTS_H
#define VOICERECOGNITION_CONSTANTS_H


#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"

const int AUDIO_FRAME_LENGTH = 200;
const int FFT_FRAME_LENGTH = 256;
const int MEL_BANK_FRAME_LENGTH = 24;

const int TARGET_SAMPLING_RATE = 8000;
const int SAMPLING_RATE = 48000;

const int ORIG_FRAME_LENGTH = SAMPLING_RATE * 0.025;
const int SMALL_RECORDER_FRAMES = SAMPLING_RATE * 0.010; // min length of recorded frame - 10ms

const float LOW_FREQ = 64.0;
const float HIGH_FREQ = 3800.0;

const unsigned int NN_INPUT_SIZE = 360;

#endif //VOICERECOGNITION_CONSTANTS_H
