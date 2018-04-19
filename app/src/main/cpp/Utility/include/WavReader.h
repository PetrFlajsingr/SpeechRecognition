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

    unsigned int dataSize = 0;

    const unsigned int HEADER_SIZE = 44;

    const unsigned int FORMAT_OFFSET = 20;

    WavInfo wavInfo;

    bool readHeader(std::ifstream& stream);

    std::string errorMessage = "";
public:

    std::string getErrorMessage();

    unsigned int getDataSize();

    short* wavToPcm(std::ifstream& stream);
};


#endif //VOICERECOGNITION_VAWREADER_H
