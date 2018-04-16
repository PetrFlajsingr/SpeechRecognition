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

        void saveBigram(char* input);

        std::vector<unsigned long> unigramAlphabetPositions;

        char forBuilding = 'a';

        inline unsigned long getUnigramAlphabetPosition(char ch);
    public:
        std::vector<LMWord> words;

        LanguageModel(std::string path);

        virtual ~LanguageModel();

        LMWord* getLMWord(std::string word);
    };
}


#endif //VOICERECOGNITION_LANGUAGEMODEL_H
