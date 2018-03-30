//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <string>
#include "Word.h"

int Word::idCounter = 0;

Word::Word(const std::string &writtenForm) {
    this->writtenForm = writtenForm;
    this->id = idCounter++;
}
