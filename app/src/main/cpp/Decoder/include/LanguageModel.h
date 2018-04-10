//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_LANGUAGEMODEL_H
#define VOICERECOGNITION_LANGUAGEMODEL_H


#include <vector>
#include <AudioSubsampler.h>
#include "LMWord.h"

namespace SpeechRecognition::Decoder {
    using namespace Utility;
    class LanguageModel {
    private:
        void saveWord(char *input);

    public:
        std::vector<LMWord> words;

        LanguageModel(std::string path);
    };
}


#endif //VOICERECOGNITION_LANGUAGEMODEL_H
