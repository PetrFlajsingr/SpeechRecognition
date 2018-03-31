//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <Token.h>
#include <GraphNode.h>

std::vector<Token*> Token::tokenVector;
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
void Token::cloneInGraph(float* inputVector) {
    int i = 0;
    for(auto iterator = currentNode->successorNodes.begin();
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
    return likelihood + inputVector[currentNode->inputVectorIndex] * currentNode->pathProbablity.at(pathNumber);
}
