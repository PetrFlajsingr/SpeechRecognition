//
// Created by Petr Flajsingr on 25/03/2018.
//

#ifndef VOICERECOGNITION_DECODER_H
#define VOICERECOGNITION_DECODER_H


#include <AcousticModel.h>
#include <LanguageModel.h>
#include <HMMGraph.h>

/**
 * Wrapper for all functions of Viterbi decoding.
 */
class Decoder {
private:
    AcousticModel* acousticModel;
    LanguageModel* languageModel;

    HMMGraph* graph;
public:
    Decoder(std::string pathToLexicon, std::string pathToNgram);

    virtual ~Decoder();

    void decode(float* input);

};


#endif //VOICERECOGNITION_DECODER_H
