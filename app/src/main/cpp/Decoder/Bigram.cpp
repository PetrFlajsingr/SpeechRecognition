//
// Created by Petr Flajsingr on 15/04/2018.
//

#include <Bigram.h>

SpeechRecognition::Decoder::Bigram::Bigram(LMWord* secondWord, float bigramProbability): secondWord(secondWord), bigramProbability(bigramProbability) {}
