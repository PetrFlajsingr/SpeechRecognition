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
#include <Bigram.h>

/**
 * Enum for finite state machine reading from file
 */
enum FSM_LM{
    NONE, //< skipping
    INFO, //< header
    DATAUNIGRAM, //< 1grams
    DATABIGRAM //< 2grams
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

        unsigned int unigramCount = 0, bigramCount = 0;
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
                        if(ngram == 1)
                            state = DATAUNIGRAM;
                        else if(ngram == 2)
                            state = DATABIGRAM;
                        continue;
                    }
                    break;
                case INFO:
                    if(std::string(inputBuffer).length() == 0) {
                        state = NONE;
                        continue;
                    }
                    record = split(inputBuffer, "=");
                    if(split(record[0].c_str(), " ")[1] == "1")
                        unigramCount = static_cast<unsigned int>(atoi(record[1].c_str()));
                    else if(split(record[0].c_str(), " ")[1] == "2")
                        bigramCount = static_cast<unsigned int>(atoi(record[1].c_str()));
                    break;
                case DATAUNIGRAM:
                    if(std::string(inputBuffer).length() == 0){
                        state = NONE;
                        continue;
                    }
                    saveWord(inputBuffer);
                    for(int j = 1; j < unigramCount; j++) {
                        file.getline(inputBuffer, 1024);
                        saveWord(inputBuffer);
                    }
                    break;
                case DATABIGRAM:
                    if(std::string(inputBuffer).length() == 0){
                        state = NONE;
                        continue;
                    }
                    saveBigram(inputBuffer);
                    for(int j = 1; j < bigramCount; j++) {
                        file.getline(inputBuffer, 1024);
                        saveBigram(inputBuffer);
                    }
                    break;
            }

        }
        file.close();
    }

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "LM loaded");
}

void SpeechRecognition::Decoder::LanguageModel::saveBigram(char *input) {
    std::vector<std::string> record = split(input, "\t");

    std::vector<std::string> words = split(record[1].c_str(), " ");

    LMWord* bigramWord = getLMWord(words[1]);

    Bigram* newBigram = new Bigram(
            bigramWord,
            static_cast<float>(atof(record[0].c_str())));
    LMWord* word = getLMWord(words[0]);

    word->bigramsMap.insert({newBigram->secondWord->id, newBigram});
}

/**
 * Saves word into representation using LMWord.
 * @param input data from file (f.e. "-0.1110232 car")
 */
void SpeechRecognition::Decoder::LanguageModel::saveWord(char *input) {
    std::vector<std::string> record = split(input, "\t");

    while(record[1][0] > forBuilding){
        forBuilding++;
        this->unigramAlphabetPositions.push_back(words.size());
    }

    if(record[1][0] == forBuilding){
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "for building %c", forBuilding);
        forBuilding++;
        this->unigramAlphabetPositions.push_back(words.size());
    }

    LMWord word(record[1]);
    word.unigramScore = static_cast<float>(atof(record[0].c_str()));

    if(record.size() > 2) {
        word.unigramBackoff = static_cast<float>(atof(record[2].c_str()));
    }else{
        word.unigramBackoff = 0;
    }

    words.push_back(word);
}

inline unsigned long SpeechRecognition::Decoder::LanguageModel::getUnigramAlphabetPosition(int ch) {
    static int last = CHAR_MAX;

    unsigned long result = 0;

    if(ch >= 'a' && ch - 'a' < unigramAlphabetPositions.size()) {
        return unigramAlphabetPositions[ch - 'a'];
    }

    if(ch > last) {
        result = words.size() - 1;
        last = CHAR_MAX;
    }else{
        last = ch;
    }


    return result;
}

SpeechRecognition::Decoder::LMWord*
SpeechRecognition::Decoder::LanguageModel::getLMWord(std::string word) {
    unsigned long startIndex = getUnigramAlphabetPosition(word.c_str()[0]);
    unsigned long endIndex = getUnigramAlphabetPosition((word.c_str()[0] + 1));
    for(auto iterator = words.begin() + startIndex;
        iterator != words.begin() + endIndex + 1;
        iterator++){
        if(word == iterator->writtenForm){
            return &(*iterator);
        }
    }
}

SpeechRecognition::Decoder::LanguageModel::~LanguageModel() {

}



