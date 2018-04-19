//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_UTILS_H
#define VOICERECOGNITION_UTILS_H
#include <vector>

namespace SpeechRecognition::Utility {
    std::string toString(float &value);

    std::string toString(int &value);

    std::vector<std::string> split(const char *str, std::string delimiter);

    void dumpToFile(std::string path, bool *array, unsigned int length);

    void dumpToFile(std::string path, short *array, unsigned int length);

    void dumpToFile(std::string path, float *array, unsigned int length);

    void dumpToFile(std::string path, float **array, unsigned int length1, unsigned int length2);
}
#endif //VOICERECOGNITION_UTILS_H