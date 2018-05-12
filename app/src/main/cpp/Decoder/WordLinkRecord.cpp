//
// Created by Petr Flajsingr on 20/04/2018.
//

#include <WordLinkRecord.h>
#include <Bigram.h>

SpeechRecognition::Decoder::WordLinkRecord::WordLinkRecord(
        SpeechRecognition::Decoder::WordLinkRecord *previous, LMWord* word) : previous(previous), word(word) {
    if(previous != NULL)
        previous->assign();
}

SpeechRecognition::Decoder::WordLinkRecord *SpeechRecognition::Decoder::WordLinkRecord::assign() {
    referenceCount++;
    return this;
}

void SpeechRecognition::Decoder::WordLinkRecord::unassign() {
    referenceCount--;
    if(referenceCount == 0)
        delete this;
}

SpeechRecognition::Decoder::WordLinkRecord *
SpeechRecognition::Decoder::WordLinkRecord::addRecord(LMWord* word) {
    WordLinkRecord* newRecord = new WordLinkRecord(this, word);
    this->unassign();

    return newRecord;
}

SpeechRecognition::Decoder::WordLinkRecord::~WordLinkRecord() {
    if(previous != NULL)
        previous->unassign();
}

float SpeechRecognition::Decoder::WordLinkRecord::getBigramProbability() {
    LMWord* lastWord = this->word;
    LMWord* nextToLastWord = this->previous->word;

    auto bigram = nextToLastWord->bigramsMap.find(lastWord->id);

    if(bigram != nextToLastWord->bigramsMap.end()){
        return bigram->second->bigramProbability;
    }

    return lastWord->unigramScore + nextToLastWord->unigramBackoff;
}
