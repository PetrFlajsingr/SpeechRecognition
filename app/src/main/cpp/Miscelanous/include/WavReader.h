//
// Created by Petr Flajsingr on 16/04/2018.
//

#ifndef VOICERECOGNITION_VAWREADER_H
#define VOICERECOGNITION_VAWREADER_H


#include <fstream>

class WavReader {
private:
    typedef struct s_WavInfo{
        unsigned int sampleRate;
        unsigned int bits;
        unsigned int dataSize;

        bool isStereo;
    }WavInfo;

    const unsigned int HEADER_SIZE = 44;

    WavInfo wavInfo;

    bool readHeader(std::ifstream& stream);

    std::string errorMessage = "";
public:

    std::string getErrorMessage();

    short* wavToPcm(std::ifstream& stream);
};


#endif //VOICERECOGNITION_VAWREADER_H
