//
// Created by Petr Flajsingr on 15/04/2018.
//

#ifndef VOICERECOGNITION_BIGRAM_H
#define VOICERECOGNITION_BIGRAM_H

#include <LMWordFWD.h>

namespace SpeechRecognition::Decoder {
    /**
     * @brief Represents bigram of a specified word
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class Bigram {
    public:
        /**
         * Create bigram
         * @param secondWord second word in a bigram
         * @param bigramProbability bigram probability
         */
        Bigram(LMWord *secondWord, float bigramProbablity);

        LMWord *secondWord;

        float bigramProbability;
    };
}


#endif //VOICERECOGNITION_BIGRAM_H
