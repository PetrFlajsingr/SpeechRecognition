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
/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
SpeechRecognition::Decoder::Token::Token(GraphNode* currentNode, bool output, unsigned int position)
        : output(output), currentNode(currentNode), position(position) {

    // first node and not loopback token
    if(currentNode->xPos == 0 && position == 1)
        this->needWord = true;
    else
        this->needWord = false;

    if(currentNode->xPos == -1) {
        alive = true;
        wordHistory = new GCList();
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
        if(wordHistory != NULL)
            wordHistory->unasign();
        wordHistory = NULL;
        return -std::numeric_limits<float>::max();
    }

    if(sourceToken != this) {
        alive = true;

        if(needWord) {
            if(wordHistory != NULL)
                wordHistory->unasign();
            wordHistory = new GCList();
            addWordToHistory();
        }else{
            if(wordHistory != NULL)
                wordHistory->unasign();
            wordHistory = sourceToken->wordHistory->assign();
        }
    }

    if(output){
        likelihood = sourceToken->likelihood + WORD_INSERTION_PENALTY
                                    + HMMGraph::getBigramMapValue(this)*
                                     SCALE_FACTOR_LM;

    }else {
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

void SpeechRecognition::Decoder::Token::addWordToHistory() {
    wordHistory->words.push_back(acousticModel->words[currentNode->wordID].lmword);
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

void SpeechRecognition::Decoder::Token::deleteAllTokens() {
    for(auto iterator = allTokens.begin();
            iterator != allTokens.end();){
        delete *iterator;
        allTokens.erase(iterator);
    }
}
