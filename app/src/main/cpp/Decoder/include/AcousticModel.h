//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_ACOUSTICMODEL_H
#define VOICERECOGNITION_ACOUSTICMODEL_H


#include <Word.h>
#include "LanguageModel.h"

namespace SpeechRecognition::Decoder {
/**
 * Contains information about acoustic model.
 */
    class AcousticModel {
    public:
        std::vector<Word> words; //< vector of all words in the model

        // do not use
        AcousticModel(std::string path);

        /**
         * Creates acoustic model and links it with language model
         * @param path path to file containing acoustic model
         * @param languageModel Language model
         */
        AcousticModel(std::string path, LanguageModel* languageModel);
    };
}

#endif //VOICERECOGNITION_ACOUSTICMODEL_H
