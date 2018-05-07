//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <algorithm>
#include <Token.h>
#include <GraphNode.h>
#include <android/log.h>
#include <constants.h>
#include <HMMGraph.h>

int SpeechRecognition::Decoder::Token::tokenCount = 0;
SpeechRecognition::Decoder::AcousticModel* SpeechRecognition::Decoder::Token::acousticModel;
SpeechRecognition::Decoder::LanguageModel* SpeechRecognition::Decoder::Token::languageModel;
std::vector<SpeechRecognition::Decoder::Token*> SpeechRecognition::Decoder::Token::allTokens;
std::vector<SpeechRecognition::Decoder::Token*> SpeechRecognition::Decoder::Token::livingTokens;
/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
SpeechRecognition::Decoder::Token::Token(GraphNode* currentNode, bool output, unsigned int position)
        : output(output), currentNode(currentNode), position(position) {
    if(currentNode->xPos == -1){
        alive = true;
        likelihood = 0.0f;
    }

    tokenCount++;

    allTokens.push_back(this);
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
float SpeechRecognition::Decoder::Token::passInGraph(float *inputVector) {
    Token* sourceToken = getBestToken(currentNode->predecessorNodes[position]);

    if(sourceToken == NULL){
        alive = false;
        if(wordLinkRecord != NULL)
            wordLinkRecord->unassign();
        wordLinkRecord = NULL;
        likelihood = -std::numeric_limits<float>::max();
        return -std::numeric_limits<float>::max();
    }

    if(sourceToken != this) {
        alive = true;
        if(wordLinkRecord != NULL)
            wordLinkRecord->unassign();
        wordLinkRecord = sourceToken->wordLinkRecord->assign();
    }

    if(output){
        LMWord* toAdd = acousticModel->words[currentNode->predecessorNodes[position]->wordID].lmword;
        wordLinkRecord = wordLinkRecord->addRecord(toAdd);
        likelihood = sourceToken->likelihood + WORD_INSERTION_PENALTY
                                    + wordLinkRecord->getBigramProbability()
                                     SCALE_FACTOR_LM;

    }else {
        Token::livingTokens.push_back(this);
        likelihood = calculateLikelihood(inputVector, position, sourceToken);
    }

    return likelihood;
}

/**
 * Calculates likelihood of next token based on this one.
 * @param inputVector vector of NN outputs
 * @param pathNumber connection number
 * @return new likelihood
 */
float SpeechRecognition::Decoder::Token::calculateLikelihood(float* inputVector, unsigned int pathNumber, Token* sourceToken) {
    return sourceToken->likelihood
           + inputVector[currentNode->inputVectorIndex]
           + currentNode->predecessorNodes[pathNumber]->pathProbablity[pathNumber];
}

SpeechRecognition::Decoder::Token::~Token() {
    tokenCount--;
}

SpeechRecognition::Decoder::Token *
SpeechRecognition::Decoder::Token::getBestToken(SpeechRecognition::Decoder::GraphNode *node) {

    if(node->bestToken != NULL && node->bestToken->alive)
        return node->bestToken;

    return NULL;
    /*
    for(auto iterator = node->tokens.begin();
            iterator != node->tokens.end();
            iterator++){
        if((*iterator)->alive)
            return *iterator;
    }

    return NULL;*/
}

void SpeechRecognition::Decoder::Token::deleteAllTokens() {
    for(auto iterator = allTokens.begin();
            iterator != allTokens.end();){
        delete *iterator;
        allTokens.erase(iterator);
    }
}
