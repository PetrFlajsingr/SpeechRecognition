//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <ios>
#include <fstream>
#include "../Miscelanous/include/Utils.h"
#include "include/AcousticModel.h"
#include "constants.h"


AcousticModel::AcousticModel(std::string path) {
    std::ifstream file;
    file.open(path.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        char inputBuffer[1024];

        std::vector<std::string> splitString;
        while(!file.eof()) {
            file.getline(inputBuffer, 1024);

            splitString = split(inputBuffer, " ");

            Word word(splitString.at(0));
            for(int i = 1; i < splitString.size();i++) {
                word.phonemes.push_back(Phoneme::stringToPhoneme(splitString.at(i)));
            }
            this->words.push_back(word);
        }
    }
    file.close();
}
