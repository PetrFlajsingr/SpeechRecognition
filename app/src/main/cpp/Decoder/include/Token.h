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
#include "GCList.h"
#include <list>

namespace SpeechRecognition::Decoder {
/**
 * Represents a token in a HMM graph.
 */
    class Token {
        float calculateLikelihood(float *inputVector, unsigned int pathNumber, Token* sourceToken);

        static AcousticModel *acousticModel;

        static LanguageModel* languageModel;

        bool needWord = true;

        unsigned int position;
    public:
        float likelihood = 0.0f;

        bool alive = false;

        GCList* wordHistory = NULL; //< word history for ngrams

        Token(GraphNode *currentNode, bool output, unsigned int position);

        virtual ~Token();

        float passInGraph(float *inputVector);

        static void setAcousticModel(AcousticModel &model) {
            acousticModel = &model;
        }

        static void setLanguageModel(LanguageModel &model){
            languageModel = &model;
        }

        void addWordToHistory();

        static int tokenCount;

        GraphNode *currentNode; //< node in which the token is placed

        static Token* getBestToken(GraphNode* node);

        static std::vector<Token*> allTokens;

        static void deleteAllTokens();

        bool output = false;
    };
}

#endif //VOICERECOGNITION_TOKEN_H
