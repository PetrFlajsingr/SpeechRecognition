//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_TOKEN_H
#define VOICERECOGNITION_TOKEN_H


#include <vector>
#include <GraphNodeFwd.h>
#include <TokenFWD.h>
#include <LMWord.h>
#include <Word.h>
#include "AcousticModel.h"
#include "LanguageModel.h"
#include "WordLinkRecord.h"
#include <list>

namespace SpeechRecognition::Decoder {

    /**
     * @brief Class representing a token for Token passing algorithm
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class Token {
        /**
         * Calculates likelihood of next token based on this one.
         * @param inputVector vector of NN outputs
         * @param pathNumber connection number
         * @return new likelihood
         */
        float calculateLikelihood(float *inputVector, unsigned int pathNumber, Token* sourceToken);

        static AcousticModel *acousticModel;

        static LanguageModel* languageModel;
    public:
        float likelihood = -std::numeric_limits<float>::max();

        unsigned int position;

        bool alive = false;

        WordLinkRecord* wordLinkRecord = NULL;

        /**
         * Registers token in static vector for all tokens.
         * @param currentNode node of graph
         * @param is output token?
         * @param phoneme position
         */
        Token(GraphNode *currentNode, bool output, unsigned int position);

        virtual ~Token();

        /**
         * Step in an algorithm. Clones the token into next states and calculates new likelihood.
         * @param inputVector vector of NN outputs
         */
        float passInGraph(float *inputVector);

        static void setAcousticModel(AcousticModel &model) {
            acousticModel = &model;
        }

        static void setLanguageModel(LanguageModel &model){
            languageModel = &model;
        }

        static int tokenCount;

        GraphNode *currentNode; //< node in which the token is placed

        /**
         * Get best token in a node
         * @param node node in which to search
         * @return pointer to best token, NULL if no tokens are active
         */
        static Token* getBestToken(GraphNode* node);

        static std::vector<Token*> allTokens;

        static void deleteAllTokens();

        bool output = false;

        static std::vector<Token*> livingTokens;
    };
}

#endif //VOICERECOGNITION_TOKEN_H
