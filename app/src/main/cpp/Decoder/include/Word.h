//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_WORD_H
#define VOICERECOGNITION_WORD_H

#include <vector>
#include <Phoneme.h>
#include "LMWord.h"

namespace SpeechRecognition::Decoder {

    /**
     * @brief Class representing a word in acoustic model.
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class Word {
    private:
        static int idCounter;
    public:
        int id;
        std::string writtenForm;
        std::vector<PHONEME> phonemes;

        LMWord* lmword;

        Word(const std::string &writtenForm);

        static void resetIdCounter();
    };
}


#endif //VOICERECOGNITION_WORD_H
