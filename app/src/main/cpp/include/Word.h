//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_WORD_H
#define VOICERECOGNITION_WORD_H


#include "Phoneme.h"
#include <vector>

class Word {
private:
    static int idCounter;

    static void resetIdCounter();
public:
    int id;
    std::string writtenForm;
    std::vector<PHONEME_ENUM> phonemes;

    Word(const std::string &writtenForm);
};


#endif //VOICERECOGNITION_WORD_H
