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

    /**
     * @brief Class containing information about language model for recognition
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class LanguageModel {
    private:
        /**
         * Add word to vector
         * @param input raw data from file
         */
        void saveWord(char *input);

        /**
         * Adds bigram to a word
         * @param input raw data from file
         */
        void saveBigram(char* input);

        std::vector<unsigned long> unigramAlphabetPositions;

        char forBuilding = 'a';

        inline unsigned long getUnigramAlphabetPosition(int ch);
    public:
        std::vector<LMWord> words; //< all words in language model

        /**
         * Reads language model from an .arpa formatted file
         * @param path path to .arpa file
         */
        LanguageModel(std::string path);

        virtual ~LanguageModel();

        /**
         * Returns pointer to desired LM word
         * @param word written form of the word
         * @return pointer to LMWord
         */
        LMWord* getLMWord(std::string word);
    };
}


#endif //VOICERECOGNITION_LANGUAGEMODEL_H
