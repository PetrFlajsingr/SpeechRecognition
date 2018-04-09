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

/**
 * Represents a token in a HMM graph.
 */
class Token {
    static unsigned int tokenCounter;

    static std::vector<Token*> tokenVector; //< vector of all tokens in the graph
        // serves as an access point for faster cloning

    static std::vector<unsigned int> indexesToDelete;

    float calculateLikelihood(float* inputVector, unsigned int pathNumber);

    static void updateIndexes(unsigned int beginIndex, unsigned int endIndex, int toAdd);

    static AcousticModel* acousticModel;

    bool needWord = true;
public:
    float likelihood = 0.0f;

    int word;

    unsigned long index_TokenVector; //< index in tokenVector, for fast deletion from tokenVector

    std::vector<LMWord> wordHistory; //< word history for ngrams

    Token(GraphNode *currentNode, int word);

    virtual ~Token();

    void passInGraph(float *inputVector);

    static void passAllTokens(float* inputVector);

    static void addIndexToDelete(unsigned int index);

    static void deleteInvalidTokens();

    static void deleteStatic();

    static void setAcousticModel(AcousticModel& model){
        acousticModel = &model;
    }

    void addWordToHistory();

    GraphNode* currentNode; //< node in which the token is placed
};


#endif //VOICERECOGNITION_TOKEN_H
