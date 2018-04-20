//
// Created by Petr Flajsingr on 25/03/2018.
//

#ifndef VOICERECOGNITION_DECODER_H
#define VOICERECOGNITION_DECODER_H


#include <AcousticModel.h>
#include <LanguageModel.h>
#include <HMMGraph.h>

namespace SpeechRecognition::Decoder {
/**
 * Wrapper for all functions of Viterbi decoding.
 */
    class ViterbiDecoder {
    private:
        AcousticModel *acousticModel;
        LanguageModel *languageModel;

        HMMGraph *graph;

        std::string buildString(Token& token);

        bool firstPass = true;
    public:
        ViterbiDecoder(std::string pathToLexicon, std::string pathToNgram);

        virtual ~ViterbiDecoder();

        void decode(float *input);

        std::string getWinner();

        void reset();
    };
}


#endif //VOICERECOGNITION_DECODER_H
