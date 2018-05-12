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
     * @brief Main class of decoder. Provides methods to forwards data through recognition graph.
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class ViterbiDecoder {
    private:
        AcousticModel *acousticModel;
        LanguageModel *languageModel;

        HMMGraph *graph;

        std::string buildString(Token& token);

        bool firstPass = true;
    public:
        /**
         * Prepares language and acoustic models and lays foundation for a graph.
         * @param pathToLexicon
         * @param pathToNgram
         */
        ViterbiDecoder(std::string pathToLexicon, std::string pathToNgram);

        virtual ~ViterbiDecoder();

        /**
         * Sends data through the graph.
         * @param output of NN
         */
        void decode(float *input);

        /**
         * Returns the word with the highest likelihood from the outpu node.
         */
        std::string getWinner();

        /**
         * Resets the decoder. Resets all tokens.
         */
        void reset();
    };
}


#endif //VOICERECOGNITION_DECODER_H
