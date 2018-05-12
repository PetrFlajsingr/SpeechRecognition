//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_ACOUSTICMODEL_H
#define VOICERECOGNITION_ACOUSTICMODEL_H


#include <Word.h>
#include "LanguageModel.h"

namespace SpeechRecognition::Decoder {
    /**
     * @brief Class representing an acoustic model for recognition
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class AcousticModel {
    public:
        std::vector<Word> words; //< vector of all words in the model

        /**
         * Reads a lexicon from file. DO NOT USE
         * @param path path to lexicon file
         */
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
