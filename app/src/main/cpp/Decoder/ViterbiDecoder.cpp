//
// Created by Petr Flajsingr on 25/03/2018.
//


#include <string>
#include <LanguageModel.h>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <ViterbiDecoder.h>
#include <vector>
#include <GraphNode.h>
#include <Token.h>
#include <Utils.h>
#include <limits>
#include <chrono>
#include <android/log.h>
#include <constants.h>
#include <list>

/**
 * Prepares language and acoustic models and lays foundation for a graph.
 * @param pathToLexicon
 * @param pathToNgram
 */
SpeechRecognition::Decoder::ViterbiDecoder::ViterbiDecoder(std::string pathToLexicon, std::string pathToNgram) {
    this->languageModel = new LanguageModel(pathToNgram);
    this->acousticModel = new AcousticModel(pathToLexicon, languageModel);

    this->graph = new HMMGraph(this->acousticModel);
    //graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));

    Token::setAcousticModel(*acousticModel);
    Token::setLanguageModel(*languageModel);

    graph->build(acousticModel);

    graph->rootNode->tokens.push_back(new Token(graph->rootNode, false, INT32_MAX));

    graph->rootNode->tokens.front()->wordHistory.push_back(languageModel->getLMWord("<s>"));
}

SpeechRecognition::Decoder::ViterbiDecoder::~ViterbiDecoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;

    LMWord::resetIdCounter();
    Word::resetIdCounter();

    Token::deleteAllTokens();
}

/**
 * Sends data through the graph.
 * @param input clearOutputNode of NN
 */
void SpeechRecognition::Decoder::ViterbiDecoder::decode(float *input) {
    graph->clearOutputNode();
    graph->passTokens(input);
    graph->applyPruning();
}

/**
 * Resets the decoder. Removes all tokens.
 */
void SpeechRecognition::Decoder::ViterbiDecoder::reset() {
    for(auto iterator = Token::allTokens.begin();
        iterator != Token::allTokens.end();
        iterator++){

        (*iterator)->alive = (*iterator)->currentNode == graph->rootNode;

        (*iterator)->likelihood = 0;
        (*iterator)->wordHistory.clear();
    }
    graph->rootNode->tokens.front()->wordHistory.push_back(languageModel->getLMWord("<s>"));

}

std::string SpeechRecognition::Decoder::ViterbiDecoder::buildString(SpeechRecognition::Decoder::Token& token){
    std::string result = "";
    for(auto iterator = token.wordHistory.begin();
            iterator != token.wordHistory.end();
            iterator++){
        result += (*iterator)->writtenForm + " ";
    }

    return result;
}

/**
 * Returns the word with the highest likelihood from the clearOutputNode node.
 */
std::string SpeechRecognition::Decoder::ViterbiDecoder::getWinner() {
    auto bestToken = Token::getBestToken(graph->outputNode);

    if(bestToken == NULL)
        return "ERR";

    bestToken->wordHistory.push_back(languageModel->getLMWord("</s>"));
    return buildString(*bestToken);
}
