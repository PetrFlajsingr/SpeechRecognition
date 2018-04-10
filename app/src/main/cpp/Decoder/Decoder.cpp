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
#include <chrono>
#include <android/log.h>
#include <constants.h>

// TODO remove using

/**
 * Prepares language and acoustic models and lays foundation for a graph.
 * @param pathToLexicon
 * @param pathToNgram
 */
SpeechRecognition::Decoder::ViterbiDecoder::ViterbiDecoder(std::string pathToLexicon, std::string pathToNgram) {
    this->acousticModel = new AcousticModel(pathToLexicon);
    this->languageModel = new LanguageModel(pathToNgram);

    this->graph = new HMMGraph(this->acousticModel);
    graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));

    Token::setAcousticModel(*acousticModel);

    graph->build(acousticModel);
}

SpeechRecognition::Decoder::ViterbiDecoder::~ViterbiDecoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;

    Token::deleteStatic();
}

int test = 0;

unsigned long sum1 = 0;
unsigned long sum2 = 0;
unsigned long sum3 = 0;
unsigned long sum4 = 0;
unsigned long sum5 = 0;
unsigned long sum6 = 0;
unsigned long sum7 = 0;
/**
 * Sends data through the graph.
 * @param input clearOutputNode of NN
 */
void SpeechRecognition::Decoder::ViterbiDecoder::decode(float *input) {
    unsigned long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    test++;

    graph->clearOutputNode();
    unsigned long timestamp1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum1 += timestamp1-startTime;

    Token::passAllTokens(input);
    unsigned long timestamp2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum2 += timestamp2-timestamp1;

    graph->applyViterbiCriterium();
    unsigned long timestamp3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum3 += timestamp3-timestamp2;

    Token::deleteInvalidTokens();
    unsigned long timestamp4 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum4 += timestamp4-timestamp3;

    graph->applyPruning();
    unsigned long timestamp5 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum5 += timestamp5-timestamp4;

    Token::deleteInvalidTokens();
    unsigned long timestamp6 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum6 += timestamp6-timestamp5;

    graph->addLM();
    unsigned long timestamp7 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum7 += timestamp7-timestamp6;

    if(test % 500 == 0){
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "clear: %d, pass: %d, viterbi: %d, viterbi del: %d, pruning: %d, pruning del: %d, LM: %d",
        sum1, sum2, sum3, sum4, sum5, sum6, sum7);
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "TOKEN COUNT: %d", Token::tokenVector.size());
    }


}

/**
 * Resets the decoder. Removes all tokens.
 */
void SpeechRecognition::Decoder::ViterbiDecoder::reset() {
    for(auto iterator = graph->outputNode->tokens.begin();
            iterator != graph->outputNode->tokens.end();){
        (*iterator)->markToKill();
        //Token::addIndexToDelete((*iterator)->index_TokenVector);
        graph->outputNode->tokens.erase(iterator);
    }

    Token::deleteInvalidTokens();
    Token::deleteStatic();
    graph->eraseTokenRecords();
    graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));
}

//TODO add to utils
std::string buildString(SpeechRecognition::Decoder::Token& token){
    std::string result = "";
    for(auto iterator = token.wordHistory.begin();
            iterator != token.wordHistory.end();
            iterator++){
        result += iterator->writtenForm + " ";
    }

    return result;
}

/**
 * Returns the word with the highest likelihood from the clearOutputNode node.
 */
std::string SpeechRecognition::Decoder::ViterbiDecoder::getWinner() {
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
    return buildString(*vector.at(maxIndex));
}
