//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <Token.h>
#include <GraphNode.h>

std::vector<Token*> Token::tokenVector;
std::vector<unsigned int> Token::indexesToDelete;
/**
 * Registers token in static vector for all tokens.
 * Saves index in the vector for fast deletion from the vector.
 * @param currentNode node of graph
 */
Token::Token(GraphNode* currentNode) : currentNode(currentNode) {
    index_TokenVector = tokenVector.size();
    tokenVector.push_back(this);
}

/**
 * Step in an algorithm. Clones the token into next states and calculates new likelihood.
 * @param inputVector vector of NN outputs
 */
void Token::passInGraph(float *inputVector) {
    int i = 0;
    this->currentNode = currentNode->successorNodes.at(0);
    this->likelihood = calculateLikelihood(inputVector, 0);
    for(auto iterator = currentNode->successorNodes.begin() + 1;
            iterator != currentNode->successorNodes.end();
            iterator++, i++){
        Token* newToken = new Token(*iterator);
        newToken->likelihood = calculateLikelihood(inputVector, i);
        (*iterator)->tokens.push_back(newToken);
    }
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
             * currentNode->pathProbablity.at(pathNumber);
}

/**
 * Passes all tokens through the graph (one step)
 * @param inputVector
 */
void Token::passAllTokens(float *inputVector) {
    for(auto iterator = tokenVector.begin();
            iterator != tokenVector.end();
            iterator++){
        (*iterator)->passInGraph(inputVector);
    }
}

/**
 * Registers token for deletion
 * @param index
 */
void Token::addIndexToDelete(unsigned int index) {
    indexesToDelete.push_back(index);
}

/**
 * Updates indexes for deletion in given range
 */
void Token::updateIndexes(unsigned int beginIndex, unsigned int endIndex, int toAdd){
    for(auto iterator = tokenVector.begin() + beginIndex;
            iterator != tokenVector.begin() + endIndex;
            iterator++){
        (*iterator)->index_TokenVector += toAdd;
    }
}

/**
 * Deletes all tokens marked for deletion in "indexesToDelete".
 * Updates "index_TokenVector" for all remaining.
 */
void Token::deleteInvalidTokens() {
    unsigned int deletedCounter = 0;

    for(auto indexIterator = indexesToDelete.begin();
            indexIterator != indexesToDelete.end();
            indexIterator++){
        delete tokenVector.at(*indexIterator - deletedCounter);
        tokenVector.erase(tokenVector.begin() + *indexIterator - deletedCounter);
        deletedCounter++;

        updateIndexes(*indexIterator,
                      (indexIterator == indexesToDelete.end() - 1)
                      ? tokenVector.size() : *(indexIterator + 1),
                      -(int)deletedCounter);
    }
}
