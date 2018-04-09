//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_ACOUSTICMODEL_H
#define VOICERECOGNITION_ACOUSTICMODEL_H


#include <Word.h>

/**
 * Contains information about acoustic model.
 */
class AcousticModel {
public:
    std::vector<Word> words; //< vector of all words in the model
    AcousticModel(std::string path);
};


#endif //VOICERECOGNITION_ACOUSTICMODEL_H
