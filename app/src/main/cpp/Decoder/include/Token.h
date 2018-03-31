//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_TOKEN_H
#define VOICERECOGNITION_TOKEN_H


#include <vector>
#include <GraphNodeFwd.h>
#include <TokenFWD.h>
#include <LMWord.h>

/**
 * Represents a token in a HMM graph.
 */
class Token {
    static std::vector<Token*> tokenVector; //< vector of all tokens in the graph
        // serves as an access point for faster cloning

    unsigned long index_TokenVector; //< index in tokenVector, for fast deletion from tokenVector

    float likelihood = 0.0f;

    std::vector<LMWord> wordHistory; //< word history for ngrams

    GraphNode* currentNode; //< node in which the token is placed

    float calculateLikelihood(float* inputVector, unsigned int pathNumber);
public:
    Token(GraphNode *currentNode);

    void cloneInGraph(float* inputVector);
};


#endif //VOICERECOGNITION_TOKEN_H
