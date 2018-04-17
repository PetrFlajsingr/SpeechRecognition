//
// Created by Petr Flajsingr on 17/04/2018.
//

#ifndef VOICERECOGNITION_GCLIST_H
#define VOICERECOGNITION_GCLIST_H


#include <list>
#include <LMWord.h>

namespace SpeechRecognition::Decoder {
    class GCList {
    private:

    public:
        unsigned int referenceCount = 1;

        bool deleted = false;

        GCList();

        GCList(GCList* list);

        GCList *assign();

        GCList *copy();

        void unasign();

        std::list<LMWord*> words;
    };
}


#endif //VOICERECOGNITION_GCLIST_H
