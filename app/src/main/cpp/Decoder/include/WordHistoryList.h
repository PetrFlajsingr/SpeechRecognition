//
// Created by Petr Flajsingr on 17/04/2018.
//

#ifndef VOICERECOGNITION_GCLIST_H
#define VOICERECOGNITION_GCLIST_H


#include <list>
#include <LMWord.h>

namespace SpeechRecognition::Decoder {
    class WordHistoryList {
    private:

    public:
        unsigned int referenceCount = 1;

        WordHistoryList();

        WordHistoryList(WordHistoryList* list);

        WordHistoryList *assign();

        WordHistoryList *copy();

        void unasign();

        std::list<LMWord*> words;
    };
}


#endif //VOICERECOGNITION_GCLIST_H
