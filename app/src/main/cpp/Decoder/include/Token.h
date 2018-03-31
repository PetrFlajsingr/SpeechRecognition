//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_TOKEN_H
#define VOICERECOGNITION_TOKEN_H


#include <vector>
#include <LMWord.h>

class Token {
    float likelihood = 0.0f;

    std::vector<LMWord> wordHistory;

   // GraphNode* currentNode;

public:
    Token(/*GraphNode *currentNode*/);

    void cloneInGraph();
};


#endif //VOICERECOGNITION_TOKEN_H
