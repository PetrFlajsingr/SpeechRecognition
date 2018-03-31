//
// Created by Petr Flajsingr on 30/03/2018.
//


#include <LMWord.h>

unsigned int LMWord::idCounter = 0;

LMWord::LMWord(const std::string &writtenForm) : writtenForm(writtenForm) {
    id = idCounter++;
}
