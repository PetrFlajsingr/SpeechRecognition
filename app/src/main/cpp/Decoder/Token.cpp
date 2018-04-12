//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <algorithm>
#include <Token.h>
#include <GraphNode.h>
#include <android/log.h>
#include <constants.h>

//std::vector<SpeechRecognition::Decoder::Token*> SpeechRecognition::Decoder::Token::tokenVector;
int SpeechRecognition::Decoder::Token::tokenCount = 0;
SpeechRecognition::Decoder::AcousticModel* SpeechRecognition::Decoder::Token::acousticModel;
SpeechRecognition::Decoder::LanguageModel* SpeechRecognition::Decoder::Token::languageModel;

/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
SpeechRecognition::Decoder::Token::Token(GraphNode* currentNode, int word, unsigned int position)
        : currentNode(currentNode), position(position) {
    if(currentNode->xPos == 0)
        this->needWord = true;
    else
        this->needWord = false;

    if(currentNode->xPos == -1)
        alive = true;

    tokenCount++;
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
void SpeechRecognition::Decoder::Token::passInGraph(float *inputVector) {
    Token* sourceToken = getBestToken(currentNode->predecessorNodes.at(position));

    if(sourceToken == NULL){
        alive = false;
        delete wordHistory;
        wordHistory = NULL;
        return;
    }

    if(needWord){
        // TODO check
        alive = true;
        if(sourceToken->wordHistory == NULL){
            wordHistory = new std::list<LMWord*>();
        } else{
            wordHistory = sourceToken->wordHistory;
        }
        addWordToHistory();
    }

    likelihood = calculateLikelihood(inputVector, position, sourceToken);
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
           + currentNode->predecessorNodes.at(pathNumber)->pathProbablity.at(pathNumber + 1);
    /*return likelihood
           + inputVector[currentNode->successorNodes.at(pathNumber)->inputVectorIndex]
             + currentNode->pathProbablity.at(pathNumber);*/
}

SpeechRecognition::Decoder::Token::~Token() {
    tokenCount--;
}

void SpeechRecognition::Decoder::Token::addWordToHistory() {
    wordHistory->push_back(languageModel->getLMWord(acousticModel->words.at(currentNode->wordID).writtenForm));
}

SpeechRecognition::Decoder::Token *
SpeechRecognition::Decoder::Token::getBestToken(SpeechRecognition::Decoder::GraphNode *node) {
    for(auto iterator = node->tokens.begin();
            iterator != node->tokens.end();
            iterator++){
        if((*iterator)->alive)
            return *iterator;
    }

    return NULL;
}
