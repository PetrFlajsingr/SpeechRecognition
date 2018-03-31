//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_ACOUSTICMODEL_H
#define VOICERECOGNITION_ACOUSTICMODEL_H


#include "Word.h"

class AcousticModel {
public:
    std::vector<Word> words;
    AcousticModel(std::string path);
};


#endif //VOICERECOGNITION_ACOUSTICMODEL_H
