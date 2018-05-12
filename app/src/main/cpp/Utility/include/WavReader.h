//
// Created by Petr Flajsingr on 16/04/2018.
//

#ifndef VOICERECOGNITION_VAWREADER_H
#define VOICERECOGNITION_VAWREADER_H


#include <fstream>

namespace SpeechRecognition::Utility {
    /**
     * @brief Class reading WAV from ifstream.
     *
     * Requires the audio to be: 8 kHz, 16-bit little endian, PCM, mono
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class WavReader {
    private:
        /**
         * Basic information about WAV
         */
        typedef struct s_WavInfo {
            unsigned int sampleRate;
            unsigned int bits;
            unsigned int dataSize;

            bool isStereo;
        } WavInfo;

        unsigned int dataSize = 0;

        const unsigned int HEADER_SIZE = 44;

        const unsigned int FORMAT_OFFSET = 20;

        WavInfo wavInfo;

        /**
         * Reads WAV header and checks requirements
         * @param stream input file stream
         * @return
         */
        bool readHeader(std::ifstream &stream);

        std::string errorMessage = "";
    public:
        /**
         * Error message in case the file could not be read
         * @return
         */
        std::string getErrorMessage();

        unsigned int getDataSize();

        /**
         * Converts wav to array of shorts
         * @param stream input file stream
         * @return array of audio data as short
         */
        short *wavToPcm(std::ifstream &stream);
    };
}


#endif //VOICERECOGNITION_VAWREADER_H
