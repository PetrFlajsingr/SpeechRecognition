//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_LMWORD_H
#define VOICERECOGNITION_LMWORD_H

#include <string>

namespace SpeechRecognition::Decoder {
    class LMWord {
    private:
        static unsigned int idCounter; //< static counter
    public:
        std::string writtenForm;

        unsigned int id;

        float unigramProbability;

        LMWord(const std::string &writtenForm);

    };
}


#endif //VOICERECOGNITION_LMWORD_H
