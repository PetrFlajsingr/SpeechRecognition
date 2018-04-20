//
// Created by Petr Flajsingr on 20/04/2018.
//

#ifndef VOICERECOGNITION_WORDLISTRECORD_H
#define VOICERECOGNITION_WORDLISTRECORD_H

#include "LMWord.h"

namespace SpeechRecognition::Decoder {

    class WordLinkRecord {
    private:
        unsigned int referenceCount = 1;
    public:
        WordLinkRecord(WordLinkRecord *previous, LMWord* word);

        LMWord* word;

        WordLinkRecord* previous;

        WordLinkRecord* assign();

        void unasign();

        WordLinkRecord* addRecord(LMWord* word);
    };
}

#endif //VOICERECOGNITION_WORDLISTRECORD_H
