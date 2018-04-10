//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <string>
#include <fstream>
#include <LanguageModel.h>
#include <vector>
#include <Utils.h>
#include <stdlib.h>

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

        unsigned int unigramCount = 0;
        std::vector<std::string> record;
        while(!file.eof()){
            file.getline(inputBuffer, 1024);
            switch(state){
                case NONE:
                    if(std::string(inputBuffer).find("data") != std::string::npos){
                        state = INFO;
                        continue;
                    }
                    if(std::string(inputBuffer).find("grams") != std::string::npos) {
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
                    unigramCount = static_cast<unsigned int>(atoi(record.at(1).c_str()));
                    break;
                case DATA:
                    if(std::string(inputBuffer).length() == 0){
                        state = NONE;
                        continue;
                    }
                    saveWord(inputBuffer);
                    for(int i = 1; i < unigramCount; i++){
                        file.getline(inputBuffer, 1024);
                        saveWord(inputBuffer);
                    }
                    break;
            }

        }
    }
    file.close();
}

/**
 * Saves word into representation using LMWord.
 * @param input data from file (f.e. "-0.1110232 car")
 */
void SpeechRecognition::Decoder::LanguageModel::saveWord(char *input) {
    std::vector<std::string> record = split(input, "\t");

    LMWord word(record.at(1));
    word.unigramProbability = static_cast<float>(atof(record.at(0).c_str()));

    words.push_back(word);
}
