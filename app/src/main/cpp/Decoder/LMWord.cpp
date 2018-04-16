//
// Created by Petr Flajsingr on 30/03/2018.
//


#include <LMWord.h>

unsigned int SpeechRecognition::Decoder::LMWord::idCounter = 0;

SpeechRecognition::Decoder::LMWord::LMWord(const std::string &writtenForm) : writtenForm(writtenForm) {
    id = idCounter++;
}

void SpeechRecognition::Decoder::LMWord::resetIdCounter() {
    idCounter = 0;
}
