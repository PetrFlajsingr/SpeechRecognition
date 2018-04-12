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
SpeechRecognition::Decoder::Token::Token(GraphNode* currentNode, int word) : currentNode(currentNode) {
    //tokenVector.push_back(this);

    if(word >= 0)
        this->word = word;

    tokenCount++;
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
void SpeechRecognition::Decoder::Token::passInGraph(float *inputVector) {
    if(currentNode->wordID == -1){
        needWord = true;
        return;
    } else if(needWord && currentNode->xPos == 0){
        this->word = this->currentNode->wordID;
        addWordToHistory();
        needWord = false;
    }
    int i = 0;
    for(auto iterator = currentNode->successorNodes.begin() + 1;
            iterator != currentNode->successorNodes.end();
            iterator++, i++){
        int wordID = ((*iterator)->wordID >= 0) ? (*iterator)->wordID : currentNode->wordID;
        Token* newToken = new Token(*iterator, wordID);
        newToken->wordHistory = this->wordHistory;
        newToken->likelihood = calculateLikelihood(inputVector, i);
        (*iterator)->tokens.push_back(newToken);
    }
    //removing old record of token
    this->currentNode->tokens.erase(std::find(currentNode->tokens.begin(), currentNode->tokens.end(), this));
    //moving to new node
    this->currentNode = currentNode->successorNodes.at(0);
    //adding record of token in node
    this->currentNode->tokens.push_back(this);
    this->likelihood = calculateLikelihood(inputVector, 0);
}

/**
 * Calculates likelihood of next token based on this one.
 * @param inputVector vector of NN outputs
 * @param pathNumber connection number
 * @return new likelihood
 */
float SpeechRecognition::Decoder::Token::calculateLikelihood(float* inputVector, unsigned int pathNumber) {
    return likelihood
           + inputVector[currentNode->successorNodes.at(pathNumber)->inputVectorIndex]
             + currentNode->pathProbablity.at(pathNumber);
}

/**
 * Passes all tokens through the graph (one step)
 * @param inputVector
 */
/*void SpeechRecognition::Decoder::Token::passAllTokens(float *inputVector) {
    unsigned int oldTokenCount = tokenVector.size();
    for(unsigned int i = 0; i < oldTokenCount; i++){
        tokenVector.at(i)->passInGraph(inputVector);
    }
}*/

// TODO speed up, a lot
/**
 * Deletes all tokens marked for deletion in "indexesToDelete".
 * Updates "index_TokenVector" for all remaining.
 */
/*void SpeechRecognition::Decoder::Token::deleteInvalidTokens() {
    for(auto iterator = tokenVector.begin();
            iterator != tokenVector.end();){
        if((*iterator)->markedToKill){
            delete *iterator;
            tokenVector.erase(iterator);
        } else
            iterator++;
    }
}*/

SpeechRecognition::Decoder::Token::~Token() {
    tokenCount--;
}

/**
 * Deletes all tokens recorded in tokenVector.
 */
/*void SpeechRecognition::Decoder::Token::deleteStatic() {
    for(auto iterator = tokenVector.begin();
            iterator != tokenVector.end();){
        delete *iterator;
        tokenVector.erase(iterator);
    }
    tokenVector.clear();
}*/

void SpeechRecognition::Decoder::Token::addWordToHistory() {
    wordHistory.push_back(languageModel->getLMWord(acousticModel->words.at(this->word).writtenForm));
}

/*void SpeechRecognition::Decoder::Token::markToKill() {
    this->markedToKill = true;
}*/
