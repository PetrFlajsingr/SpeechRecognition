//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_LMWORD_H
#define VOICERECOGNITION_LMWORD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <BigramFWD.h>

namespace SpeechRecognition::Decoder {

    /**
     * @brief Class containing information about word in language model
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class LMWord {
    private:
        static unsigned int idCounter; //< static counter
    public:
        std::string writtenForm;

        unsigned int id;

        float unigramScore;

        float unigramBackoff;

        LMWord(const std::string &writtenForm);

        std::unordered_map<unsigned int, Bigram*> bigramsMap;

        static void resetIdCounter();
    };
}


#endif //VOICERECOGNITION_LMWORD_H
