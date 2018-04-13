//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <algorithm>
#include <Token.h>
#include <GraphNode.h>
#include <android/log.h>
#include <constants.h>

int SpeechRecognition::Decoder::Token::tokenCount = 0;
SpeechRecognition::Decoder::AcousticModel* SpeechRecognition::Decoder::Token::acousticModel;
SpeechRecognition::Decoder::LanguageModel* SpeechRecognition::Decoder::Token::languageModel;
std::vector<SpeechRecognition::Decoder::Token*> SpeechRecognition::Decoder::Token::allTokens;
/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
SpeechRecognition::Decoder::Token::Token(GraphNode* currentNode, bool output, unsigned int position)
        : output(output), currentNode(currentNode), position(position) {
    if(currentNode->xPos == 0 && position == 0)
        this->needWord = true;
    else
        this->needWord = false;

    if(currentNode->xPos == -1)
        alive = true;

    tokenCount++;

    allTokens.push_back(this);
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
void SpeechRecognition::Decoder::Token::passInGraph(float *inputVector) {
    Token* sourceToken = getBestToken(currentNode->predecessorNodes.at(position));

    if(sourceToken == NULL){
        alive = false;
        return;
    }
    alive = true;

    if(sourceToken != this) {

        wordHistory = sourceToken->wordHistory;

        if(needWord)
            addWordToHistory();
    }

    if(output){
        likelihood = sourceToken->likelihood;
    }else {
        likelihood = calculateLikelihood(inputVector, position, sourceToken);
    }
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
           + currentNode->predecessorNodes.at(pathNumber)->pathProbablity.at(pathNumber);
}

SpeechRecognition::Decoder::Token::~Token() {
    tokenCount--;
}

void SpeechRecognition::Decoder::Token::addWordToHistory() {
    if(currentNode->wordID < 0){
        wordHistory.push_back(languageModel->getLMWord(
                acousticModel->words.at(currentNode->predecessorNodes.at(this->position)->wordID).writtenForm
        ));
    } else {
        wordHistory.push_back(languageModel->getLMWord(
                acousticModel->words.at(currentNode->wordID).writtenForm
        ));
    }
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
