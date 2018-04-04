//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_TOKEN_H
#define VOICERECOGNITION_TOKEN_H


#include <vector>
#include <GraphNodeFwd.h>
#include <TokenFWD.h>
#include <LMWord.h>
#include "Word.h"

/**
 * Represents a token in a HMM graph.
 */
class Token {
    static unsigned int tokenCounter;

    static std::vector<Token*> tokenVector; //< vector of all tokens in the graph
        // serves as an access point for faster cloning

    static std::vector<unsigned int> indexesToDelete;

    std::vector<LMWord> wordHistory; //< word history for ngrams

    GraphNode* currentNode; //< node in which the token is placed

    float calculateLikelihood(float* inputVector, unsigned int pathNumber);

    static void updateIndexes(unsigned int beginIndex, unsigned int endIndex, int toAdd);
public:
    float likelihood = 0.0f;

    int word;

    unsigned long index_TokenVector; //< index in tokenVector, for fast deletion from tokenVector

    Token(GraphNode *currentNode, int word);

    void passInGraph(float *inputVector);

    static void passAllTokens(float* inputVector);

    static void addIndexToDelete(unsigned int index);

    static void deleteInvalidTokens();

    virtual ~Token();

    static void deleteStatic();

    struct sortByLikelihood {
        bool operator()(Token const &a, Token const &b) const {
            return a.likelihood > b.likelihood;
        }
    };
};


#endif //VOICERECOGNITION_TOKEN_H
