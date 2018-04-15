//
// Created by Petr Flajsingr on 15/04/2018.
//

#ifndef VOICERECOGNITION_BIGRAM_H
#define VOICERECOGNITION_BIGRAM_H

#include <LMWordFWD.h>

namespace SpeechRecognition::Decoder {
    class Bigram {
    public:
        Bigram(LMWord *secondWord, float bigramProbablity);

        LMWord *secondWord;

        float bigramProbability;
    };
}


#endif //VOICERECOGNITION_BIGRAM_H
