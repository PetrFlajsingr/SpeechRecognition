//
// Created by Petr Flajsingr on 17/04/2018.
//

#include <android/log.h>
#include <constants.h>
#include "GCList.h"

SpeechRecognition::Decoder::GCList* SpeechRecognition::Decoder::GCList::assign() {
    referenceCount++;
    return this;
}

void SpeechRecognition::Decoder::GCList::unasign() {
    referenceCount--;
    if(referenceCount == 0){
        deleted = true;
        delete this;
    }
}

SpeechRecognition::Decoder::GCList *SpeechRecognition::Decoder::GCList::copy() {
    GCList* newList = new GCList(this);
    newList->referenceCount = 1;
}

SpeechRecognition::Decoder::GCList::GCList() {
    referenceCount = 1;
}

SpeechRecognition::Decoder::GCList::GCList(SpeechRecognition::Decoder::GCList *list) {
    referenceCount = 1;
    this->words = list->words;
}
