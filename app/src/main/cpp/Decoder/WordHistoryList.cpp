//
// Created by Petr Flajsingr on 17/04/2018.
//

#include <android/log.h>
#include <constants.h>
#include <WordHistoryList.h>

SpeechRecognition::Decoder::WordHistoryList* SpeechRecognition::Decoder::WordHistoryList::assign() {
    referenceCount++;
    return this;
}

void SpeechRecognition::Decoder::WordHistoryList::unasign() {
    referenceCount--;
    if(referenceCount == 0){
        delete this;
    }
}

SpeechRecognition::Decoder::WordHistoryList *SpeechRecognition::Decoder::WordHistoryList::copy() {
    return new WordHistoryList(this);

}

SpeechRecognition::Decoder::WordHistoryList::WordHistoryList() {
    referenceCount = 1;
}

SpeechRecognition::Decoder::WordHistoryList::WordHistoryList(SpeechRecognition::Decoder::WordHistoryList *list) {
    referenceCount = 1;
    this->words = list->words;
}
