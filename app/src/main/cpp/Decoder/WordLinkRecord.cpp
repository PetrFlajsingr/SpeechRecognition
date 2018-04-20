//
// Created by Petr Flajsingr on 20/04/2018.
//

#include <WordLinkRecord.h>


SpeechRecognition::Decoder::WordLinkRecord::WordLinkRecord(
        SpeechRecognition::Decoder::WordLinkRecord *previous, LMWord* word) : previous(previous), word(word) {
    if(previous != NULL)
        previous->assign();
}

/*SpeechRecognition::Decoder::WordLinkRecord *SpeechRecognition::Decoder::WordLinkRecord::copy() {
    WordLinkRecord* copiedRecord = new WordLinkRecord(this->previous, this->word);

    return copiedRecord;
}*/

SpeechRecognition::Decoder::WordLinkRecord *SpeechRecognition::Decoder::WordLinkRecord::assign() {
    referenceCount++;
    return this;
}

void SpeechRecognition::Decoder::WordLinkRecord::unasign() {
    referenceCount--;
    if(referenceCount == 0)
        delete this;
}

SpeechRecognition::Decoder::WordLinkRecord *
SpeechRecognition::Decoder::WordLinkRecord::addRecord(LMWord* word) {
    WordLinkRecord* newRecord = new WordLinkRecord(this, word);
    this->unasign();

    return newRecord;
}
