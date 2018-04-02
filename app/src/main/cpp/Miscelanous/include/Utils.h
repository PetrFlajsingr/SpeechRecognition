//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_UTILS_H
#define VOICERECOGNITION_UTILS_H
#include <vector>

std::string toString(float& value);
std::string toString(int& value);
std::vector<std::string> split(char* str, std::string delimiter);

#endif //VOICERECOGNITION_UTILS_H
