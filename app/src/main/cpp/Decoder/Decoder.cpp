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

/**
 * Prepares language and acoustic models and lays foundation for a graph.
 * @param pathToLexicon
 * @param pathToNgram
 */
Decoder::Decoder(std::string pathToLexicon, std::string pathToNgram) {
    this->acousticModel = new AcousticModel(pathToLexicon);
    this->languageModel = new LanguageModel(pathToNgram);

    this->graph = new HMMGraph(this->acousticModel);
}

Decoder::~Decoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;
}

/**
 * Sends data through the graph.
 * @param input output of NN
 */
void Decoder::decode(float *input) {
    graph->update(acousticModel);
    Token::passAllTokens(input);
    graph->applyViterbiCriterium(graph->rootNode);
    Token::deleteInvalidTokens();
}
