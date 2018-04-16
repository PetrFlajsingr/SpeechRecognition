//
// Created by Petr Flajsingr on 15/04/2018.
//

#include <Bigram.h>

/**
 * Create bigram
 * @param secondWord second word in a bigram
 * @param bigramProbability bigram probability
 */
SpeechRecognition::Decoder::Bigram::Bigram(LMWord* secondWord, float bigramProbability): secondWord(secondWord), bigramProbability(bigramProbability) {}
