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

Decoder::Decoder(std::string pathToLexicon, std::string pathToNgram) {
    this->acousticModel = new AcousticModel(pathToLexicon);
    this->languageModel = new LanguageModel(pathToNgram);

    this->graph = new HMMGraph(this->acousticModel);
    graph->update(acousticModel);
}

Decoder::~Decoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;
}

void Decoder::decode(float *input) {
    this->graph->update(acousticModel);

    graph->rootNode->tokens.at(0)->cloneInGraph();
}
