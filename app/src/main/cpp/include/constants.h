//
// Created by Petr Flajsingr on 30/11/2017.
//
//// This file contains constants used across multiple source files in this project

#ifndef VOICERECOGNITION_CONSTANTS_H
#define VOICERECOGNITION_CONSTANTS_H


#include <vector>
#include <cmath>
#include <string>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"

const int AUDIO_FRAME_LENGTH = 200;
const int FFT_FRAME_LENGTH = 256;
const int MEL_BANK_FRAME_LENGTH = 24;

const int TARGET_SAMPLING_RATE = 8000;
const int SAMPLING_RATE = 48000;

const int ORIG_FRAME_LENGTH = SAMPLING_RATE * 0.025;
const int SMALL_RECORDER_FRAMES = SAMPLING_RATE * 0.010; // min length of recorded frame - 10ms

const int SUBSAMPLED_OVERLAP_LENGTH = TARGET_SAMPLING_RATE * 0.010;

const float LOW_FREQ = 64.0;
const float HIGH_FREQ = 3800.0;

const unsigned int NN_INPUT_SIZE = 360;

const unsigned int BEAM_PRUNING_LIMIT = 190;
const unsigned int LIVE_STATES_PRUNING_LIMIT = 2000; //< pruning: max count of all active tokens

const float SCALE_FACTOR_LM = 2;

const float WORD_INSERTION_PENALTY = -10;

const std::string LM_PATH = "/sdcard/speechrecognition/lm.arpa";

const std::string LEXICON_PATH = "/sdcard/speechrecognition/lexicon.bin";

const std::string NN_PATH = "/sdcard/speechrecognition/nn.bin";

const std::vector<float> TEMP_PROB = {
        static_cast<float>(log(0.87)),
        static_cast<float>(log(0.13))
};

#endif //VOICERECOGNITION_CONSTANTS_H
