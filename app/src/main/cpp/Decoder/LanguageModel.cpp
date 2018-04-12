//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <string>
#include <fstream>
#include <LanguageModel.h>
#include <vector>
#include <Utils.h>
#include <stdlib.h>
#include <android/log.h>
#include <constants.h>

/**
 * Enum for finite state machine reading from file
 */
enum FSM_LM{
    NONE, //< skipping
    INFO, //< header
    DATA //< ngrams
};

/**
 * Reads language model from an .arpa formatted file
 * @param path path to .arpa file
 */
SpeechRecognition::Decoder::LanguageModel::LanguageModel(std::string path) {
    std::ifstream file;
    file.open(path.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        char inputBuffer[1024];
        FSM_LM state = NONE;

        int ngram = 0;

        unsigned int unigramCount = 0;
        std::vector<std::string> record;
        unsigned int i = 0;
        while(!file.eof()){
            file.getline(inputBuffer, 1024);
            i++;
            switch(state){
                case NONE:
                    if(std::string(inputBuffer).find("data") != std::string::npos){
                        state = INFO;
                        continue;
                    }
                    if(std::string(inputBuffer).find("grams") != std::string::npos) {
                        ngram = inputBuffer[1] - '0';
                        state = DATA;
                        continue;
                    }
                    break;
                case INFO:
                    if(std::string(inputBuffer).length() == 0) {
                        state = NONE;
                        continue;
                    }
                    record = split(inputBuffer, "=");
                    if(split(record.at(0).c_str(), " ").at(1) == "1")
                        unigramCount = static_cast<unsigned int>(atoi(record.at(1).c_str()));
                    break;
                case DATA:
                    if(std::string(inputBuffer).length() == 0){
                        state = NONE;
                        continue;
                    }
                    if(ngram == 1){
                        saveWord(inputBuffer);
                        for(int j = 1; j < unigramCount; j++){
                            file.getline(inputBuffer, 1024);
                            saveWord(inputBuffer);
                        }
                    }
                    break;
            }

        }
    }
    file.close();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "LM loaded");
}

/**
 * Saves word into representation using LMWord.
 * @param input data from file (f.e. "-0.1110232 car")
 */
void SpeechRecognition::Decoder::LanguageModel::saveWord(char *input) {
    std::vector<std::string> record = split(input, "\t");

    while(record.at(1)[0] > forBuilding){
        forBuilding++;
        this->unigramAlphabetPositions.push_back(words.size());
    }

    if(record.at(1)[0] == forBuilding){
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "for building %c", forBuilding);
        forBuilding++;
        this->unigramAlphabetPositions.push_back(words.size());
    }

    LMWord word(record.at(1));
    word.unigramScore = static_cast<float>(atof(record.at(0).c_str()));

    if(record.size() > 2) {
        word.unigramBackoff = static_cast<float>(atof(record.at(2).c_str()));
    }else{
        word.unigramBackoff = 0;
    }

    words.push_back(word);
}

float SpeechRecognition::Decoder::LanguageModel::getUnigramScore(std::string word) {
    unsigned long startIndex = getUnigramAlphabetPosition(word[0]);
    unsigned long endIndex = getUnigramAlphabetPosition(static_cast<char>(word[0] + 1));
    for(auto iterator = words.begin() + startIndex;
            iterator != words.end() - endIndex;
            iterator++){
        if(word == iterator->writtenForm){
            return iterator->unigramScore;
        }
    }
    return -2;
}

inline unsigned long SpeechRecognition::Decoder::LanguageModel::getUnigramAlphabetPosition(char ch) {
    if(ch - 'a' >= unigramAlphabetPositions.size())
        return unigramAlphabetPositions.size()-1;
    if(ch >= 'a' && ch < words.size()) {
        return unigramAlphabetPositions[ch - 'a'];
    }
    return 0;
}

SpeechRecognition::Decoder::LMWord*
SpeechRecognition::Decoder::LanguageModel::getLMWord(std::string word) {
    unsigned long startIndex = getUnigramAlphabetPosition(word[0]);
    unsigned long endIndex = getUnigramAlphabetPosition(static_cast<char>(word[0] + 1));
    for(auto iterator = words.begin() + startIndex;
        iterator != words.end() - endIndex;
        iterator++){
        if(word == iterator->writtenForm){
            return &(*iterator);
        }
    }
    return new LMWord("UNKNOWN");
}


