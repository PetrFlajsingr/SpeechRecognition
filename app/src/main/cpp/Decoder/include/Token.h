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
#include <list>

namespace SpeechRecognition::Decoder {
/**
 * Represents a token in a HMM graph.
 */
    class Token {
        float calculateLikelihood(float *inputVector, unsigned int pathNumber);

        static AcousticModel *acousticModel;

        static LanguageModel* languageModel;

        bool needWord = true;
    public:
        float likelihood = 0.0f;

        int word;

        std::list<LMWord*> wordHistory; //< word history for ngrams

        Token(GraphNode *currentNode, int word);

        virtual ~Token();

        void passInGraph(float *inputVector);

        static void setAcousticModel(AcousticModel &model) {
            acousticModel = &model;
        }

        static void setLanguageModel(LanguageModel &model){
            languageModel = &model;
        }

        void addWordToHistory();

        static int tokenCount;

        GraphNode *currentNode; //< node in which the token is placed
    };
}

#endif //VOICERECOGNITION_TOKEN_H
