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
#include <algorithm>
#include <deque>

SpeechRecognition::Decoder::ViterbiDecoder::ViterbiDecoder(std::string pathToLexicon, std::string pathToNgram) {
    this->languageModel = new LanguageModel(pathToNgram);
    this->acousticModel = new AcousticModel(pathToLexicon, languageModel);

    this->graph = new HMMGraph(this->acousticModel);

    Token::setAcousticModel(*acousticModel);
    Token::setLanguageModel(*languageModel);

    graph->build(acousticModel);

    graph->rootNode->tokens.push_back(new Token(graph->rootNode, false, INT32_MAX));

    graph->rootNode->tokens.front()->wordLinkRecord = new WordLinkRecord(NULL, languageModel->getLMWord("<s>"));
}

SpeechRecognition::Decoder::ViterbiDecoder::~ViterbiDecoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;

    LMWord::resetIdCounter();
    Word::resetIdCounter();

    Token::deleteAllTokens();
}

void SpeechRecognition::Decoder::ViterbiDecoder::decode(float *input) {
    graph->clearOutputNode();
    graph->passTokens(input);
    graph->applyPruning();

    if(firstPass){
        graph->rootNode->tokens.front()->alive = false;
        firstPass = false;
    }
}

void SpeechRecognition::Decoder::ViterbiDecoder::reset() {
    firstPass = true;
    for(auto iterator = Token::allTokens.begin();
        iterator != Token::allTokens.end();
        iterator++){

        (*iterator)->alive = (*iterator)->currentNode == graph->rootNode;

        (*iterator)->likelihood = 0;
        if((*iterator)->alive) {
            if((*iterator)->wordLinkRecord != NULL) {
                (*iterator)->wordLinkRecord->unassign();
                (*iterator)->wordLinkRecord = NULL;
            }
        }
    }
    graph->rootNode->tokens.front()->wordLinkRecord = new WordLinkRecord(NULL, languageModel->getLMWord("<s>"));

}

std::string SpeechRecognition::Decoder::ViterbiDecoder::buildString(SpeechRecognition::Decoder::Token& token){
    std::deque<LMWord*> words;

    for(auto recordIterator = token.wordLinkRecord;
        recordIterator != NULL;
        recordIterator = recordIterator->previous){
        words.push_front(recordIterator->word);
    }


    std::string result = "";

    for(auto iterator = words.begin();
            iterator != words.end();
            iterator++){
        result.append((*iterator)->writtenForm);
        result.append(" ");
    }

    return result;
}

std::string SpeechRecognition::Decoder::ViterbiDecoder::getWinner() {
    auto bestToken = Token::getBestToken(graph->outputNode);

    if(bestToken == NULL)
        return "ERR";

    bestToken->wordLinkRecord = bestToken->wordLinkRecord->addRecord(languageModel->getLMWord("</s>"));
    std::string result =  buildString(*bestToken);
    return result;
}
