//
// Created by Petr Flajsingr on 25/03/2018.
//


#include <string>
#include <LanguageModel.h>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <Decoder.h>
#include <vector>
#include <GraphNode.h>
#include <Token.h>
#include <Utils.h>
#include <limits>

/**
 * Prepares language and acoustic models and lays foundation for a graph.
 * @param pathToLexicon
 * @param pathToNgram
 */
Decoder::Decoder(std::string pathToLexicon, std::string pathToNgram) {
    this->acousticModel = new AcousticModel(pathToLexicon);
    this->languageModel = new LanguageModel(pathToNgram);

    this->graph = new HMMGraph(this->acousticModel);
    graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));
}

Decoder::~Decoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;

    Token::deleteStatic();
}

/**
 * Sends data through the graph.
 * @param input clearOutputNode of NN
 */
void Decoder::decode(float *input) {
    graph->update(acousticModel);
    graph->clearOutputNode();
    Token::deleteInvalidTokens();

    Token::passAllTokens(input);
    graph->applyViterbiCriterium();
    Token::deleteInvalidTokens();

    graph->applyPruning();
    Token::deleteInvalidTokens();
}

/**
 * Resets the decoder. Removes all tokens.
 */
void Decoder::reset() {
    for(auto iterator = graph->outputNode->tokens.begin();
            iterator != graph->outputNode->tokens.end();){
        Token::addIndexToDelete((*iterator)->index_TokenVector);
        graph->outputNode->tokens.erase(iterator);
    }

    Token::deleteInvalidTokens();
    Token::deleteStatic();
    graph->eraseTokenRecords();
    graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));
}

/**
 * Returns the word with the highest likelihood from the clearOutputNode node.
 */
std::string Decoder::getWinner() {
    auto vector = this->graph->outputNode->tokens;

    float maxLikelihood = -std::numeric_limits<float>::max();
    int maxIndex = -1, i = 0;
    for(auto iterator = vector.begin();
            iterator != vector.end();
            iterator++, i++){
        if((*iterator)->likelihood > maxLikelihood){
            maxLikelihood = (*iterator)->likelihood;
            maxIndex = i;
        }
    }

    return acousticModel->words.at(vector.at(maxIndex)->word).writtenForm;
}
