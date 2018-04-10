//
// Created by Petr Flajsingr on 30/03/2018.
//



#include <Word.h>

int SpeechRecognition::Decoder::Word::idCounter = 0;

SpeechRecognition::Decoder::Word::Word(const std::string &writtenForm) {
    this->writtenForm = writtenForm;
    this->id = idCounter++;
}

void SpeechRecognition::Decoder::Word::resetIdCounter() {
    idCounter = 0;
}
