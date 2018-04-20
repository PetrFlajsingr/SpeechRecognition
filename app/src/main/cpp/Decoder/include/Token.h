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
 * Represents a token in a HMM graph.
 */
    class Token {
        float calculateLikelihood(float *inputVector, unsigned int pathNumber, Token* sourceToken);

        static AcousticModel *acousticModel;

        static LanguageModel* languageModel;
    public:
        float likelihood = -std::numeric_limits<float>::max();

        unsigned int position;

        bool alive = false;

        WordLinkRecord* wordLinkRecord = NULL;

        Token(GraphNode *currentNode, bool output, unsigned int position);

        virtual ~Token();

        float passInGraph(float *inputVector);

        static void setAcousticModel(AcousticModel &model) {
            acousticModel = &model;
        }

        static void setLanguageModel(LanguageModel &model){
            languageModel = &model;
        }

        static int tokenCount;

        GraphNode *currentNode; //< node in which the token is placed

        static Token* getBestToken(GraphNode* node);

        static std::vector<Token*> allTokens;

        static void deleteAllTokens();

        bool output = false;

        static std::vector<Token*> livingTokens;
    };
}

#endif //VOICERECOGNITION_TOKEN_H
