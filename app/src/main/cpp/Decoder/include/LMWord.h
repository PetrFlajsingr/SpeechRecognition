//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_LMWORD_H
#define VOICERECOGNITION_LMWORD_H

#include <string>
#include <vector>
#include <BigramFWD.h>

namespace SpeechRecognition::Decoder {
    class LMWord {
    private:
        static unsigned int idCounter; //< static counter
    public:
        std::string writtenForm;

        unsigned int id;

        float unigramScore;

        float unigramBackoff;

        LMWord(const std::string &writtenForm);

        std::vector<Bigram*> bigrams;

        static void resetIdCounter();
    };
}


#endif //VOICERECOGNITION_LMWORD_H
