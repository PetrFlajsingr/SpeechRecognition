//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <algorithm>
#include <Token.h>
#include <GraphNode.h>

std::vector<Token*> Token::tokenVector;
//std::vector<unsigned int> Token::indexesToDelete;
unsigned int Token::tokenCounter;
AcousticModel* Token::acousticModel;
/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
Token::Token(GraphNode* currentNode, int word) : currentNode(currentNode) {
    index_TokenVector = tokenVector.size();
    tokenVector.push_back(this);
    tokenCounter++;

    if(word >= 0)
        this->word = word;
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
void Token::passInGraph(float *inputVector) {
    if(currentNode->wordID == -1){
        needWord = true;
        return;
    } else if(currentNode->xPos == 0 && needWord){
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
float Token::calculateLikelihood(float* inputVector, unsigned int pathNumber) {
    return likelihood
           + inputVector[currentNode->successorNodes.at(pathNumber)->inputVectorIndex]
             + currentNode->pathProbablity.at(pathNumber);
}

/**
 * Passes all tokens through the graph (one step)
 * @param inputVector
 */
void Token::passAllTokens(float *inputVector) {
    unsigned int oldTokenCount = tokenVector.size();
    for(unsigned int i = 0; i < oldTokenCount; i++){
        tokenVector.at(i)->passInGraph(inputVector);
    }
}

/**
 * Registers token for deletion
 */
/*void Token::addIndexToDelete(unsigned int index) {
    indexesToDelete.push_back(index);
}*/

/**
 * Updates indexes for deletion in given range
 */
/*void Token::updateIndexes(unsigned int beginIndex, unsigned int endIndex, int toAdd){
    for(auto iterator = tokenVector.begin() + beginIndex;
            iterator != tokenVector.begin() + endIndex;
            iterator++){
        (*iterator)->index_TokenVector += toAdd;
    }
}*/

// TODO speed up, a lot
/**
 * Deletes all tokens marked for deletion in "indexesToDelete".
 * Updates "index_TokenVector" for all remaining.
 */
void Token::deleteInvalidTokens() {
    for(auto iterator = tokenVector.begin();
            iterator != tokenVector.end();
            iterator++){
        if((*iterator)->markedToKill){
            delete *iterator;
            tokenVector.erase(iterator);
            iterator--;
        }
    }
    // TODO remove
/*
    std::sort(indexesToDelete.begin(), indexesToDelete.end());
    unsigned int deletedCounter = 0;
    unsigned int startIndex, endIndex;
    for(auto indexIterator = indexesToDelete.begin();
            indexIterator != indexesToDelete.end();
            indexIterator++){
        startIndex = *indexIterator - deletedCounter;

        delete tokenVector.at(startIndex);
        tokenVector.erase(tokenVector.begin() + startIndex);
        deletedCounter++;

        endIndex = (indexIterator == indexesToDelete.end() - 1)
                   ? tokenVector.size() : *(indexIterator + 1) - deletedCounter;

        updateIndexes(startIndex,
                      endIndex,
                      -(int)deletedCounter);
    }
    indexesToDelete.clear();
    */
}

Token::~Token() {
    tokenCounter--;
}

/**
 * Deletes all tokens recorded in tokenVector.
 */
void Token::deleteStatic() {
    for(auto iterator = tokenVector.begin();
            iterator != tokenVector.end();){
        delete *iterator;
        tokenVector.erase(iterator);
    }
    tokenVector.clear();
    //indexesToDelete.clear();

    tokenCounter = 0;
}

void Token::addWordToHistory() {
    LMWord word(acousticModel->words.at(this->word).writtenForm);
    wordHistory.push_back(word);
}

void Token::markToKill() {
    this->markedToKill = true;
}
