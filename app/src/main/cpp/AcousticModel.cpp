//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <ios>
#include <fstream>
#include <android/log.h>
#include "AcousticModel.h"
#include "constants.h"

std::vector<std::string> split(char* str, std::string delimiter, bool removeNewLine){
    std::vector<std::string> result;

    std::string toSplit(str);
    size_t pos = 0;
    std::string token;

    while((pos = toSplit.find(delimiter)) != std::string::npos){
        token = toSplit.substr(0, pos);
        result.push_back(token);
        toSplit.erase(0, pos + delimiter.length());
    }

    token = toSplit;
    if(removeNewLine && token.find('\n') != std::string::npos){
        token.pop_back();
    }
    result.push_back(token);

    return result;
}

AcousticModel::AcousticModel(std::string path) {
    std::ifstream file;
    file.open(path.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        char inputBuffer[1024];

        std::vector<std::string> splitString;
        while(!file.eof()) {
            file.getline(inputBuffer, 1024);

            splitString = split(inputBuffer, " ", true);

            Word word(splitString.at(0));
            for(int i = 1; i < splitString.size();i++) {
                word.phonemes.push_back(Phoneme::stringToPhoneme(splitString.at(i)));
            }
            this->words.push_back(word);
        }
    }
    file.close();

    print();
}
