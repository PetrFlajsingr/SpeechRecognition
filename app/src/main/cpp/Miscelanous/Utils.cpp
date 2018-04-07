//
// Created by Petr Flajsingr on 30/03/2018.
//


#include <string>
#include <vector>
#include <Utils.h>
#include <sstream>
#include <fstream>

std::string toString(float& value){
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string toString(int& value){
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::vector<std::string> split(char* str, std::string delimiter){
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
    if(token.find('\n') != std::string::npos){
        token.pop_back();
    }
    result.push_back(token);

    return result;
}

void dumpToFile(std::string path, bool* array, unsigned int length) {
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < length; ++i){
        if(array[i])
            out.write("true", 4);
        else
            out.write("false", 5);
        out.write("\n", 1);
    }
    out.close();
}
void dumpToFile(std::string path, float* array, unsigned int length) {
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < length; ++i){
        std::stringstream ss;
        ss << array[i];
        out.write(ss.str().c_str(), ss.str().size());
        out.write("\n", 1);
    }
    out.close();
}

void dumpToFile(std::string path, short *array, unsigned int length) {
    std::ofstream out;
    out.open(path.c_str());

    for(int i = 0; i < length; ++i){
        std::stringstream ss;
        ss << array[i];
        out.write(ss.str().c_str(), ss.str().size());
        out.write("\n", 1);
    }
    out.close();
}
