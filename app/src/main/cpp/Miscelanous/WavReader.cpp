//
// Created by Petr Flajsingr on 16/04/2018.
//

#include "WavReader.h"

bool WavReader::readHeader(std::ifstream& stream) {
    unsigned int dataRead = FORMAT_OFFSET;
    stream.ignore(FORMAT_OFFSET);

    // read and check format
    uint16_t format;
    stream.read((char*)(&format), 2);
    if(format != 1){
        errorMessage = "Unsupported format";
        return false;
    }
    dataRead += 2;

    // read and check channel count
    uint16_t channels;
    stream.read((char*)(&channels), 2);
    if(channels != 1){
        errorMessage = "Unsupported channels";
        return false;
    }
    dataRead += 2;

    // read and check sampling rate
    uint32_t sampleRate;
    stream.read((char*)(&sampleRate), 4);
    if(sampleRate != 8000){
        errorMessage = "Unsupported sample rate";
        return false;
    }
    dataRead += 4;

    stream.ignore(6);
    dataRead += 6;

    // read and check data width
    uint16_t bits;
    stream.read((char*)(&bits), 2);
    if(bits != 16){
        errorMessage = "Unsupported bits";
        return false;
    }
    dataRead += 2;

    // ignored the rest of the header until data size
    stream.ignore(HEADER_SIZE - dataRead - 4);

    uint32_t dataSize;
    stream.read((char*)(&dataSize), 4);

    wavInfo.bits = bits;
    wavInfo.sampleRate = sampleRate;
    wavInfo.isStereo = channels == 2;
    wavInfo.dataSize = dataSize;
    return true;
}

std::string WavReader::getErrorMessage() {
    return errorMessage;
}

short *WavReader::wavToPcm(std::ifstream& stream) {
    if(!readHeader(stream))
        return NULL;

    short* data = new short[wavInfo.dataSize / 2];

    dataSize = wavInfo.dataSize / 2;

    stream.read((char*)data, wavInfo.dataSize);

    return data;
}

unsigned int WavReader::getDataSize() {
    return dataSize;
}
