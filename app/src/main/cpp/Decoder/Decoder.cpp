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
#include <list>
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
    //graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1));

    Token::setAcousticModel(*acousticModel);
    Token::setLanguageModel(*languageModel);

    graph->build(acousticModel);

    graph->rootNode->tokens.push_back(new Token(graph->rootNode, -1, INT32_MAX));
}

SpeechRecognition::Decoder::ViterbiDecoder::~ViterbiDecoder() {
    delete this->acousticModel;
    delete this->languageModel;
    delete this->graph;

//    Token::deleteStatic();
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

    //OK
    graph->clearOutputNode();
    unsigned long timestamp1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum1 += timestamp1-startTime;
    //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "Clear TOKEN COUNT: %d", Token::tokenCount);

    // TODO remake all
    // SEGFAULT
    graph->passTokens(input);
    unsigned long timestamp2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum2 += timestamp2-timestamp1;
    //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "Pass TOKEN COUNT: %d", Token::tokenCount);

    graph->applyViterbiCriterium();
    unsigned long timestamp3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum3 += timestamp3-timestamp2;
    //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "Viterbi TOKEN COUNT: %d", Token::tokenCount);

    graph->applyPruning();
    unsigned long timestamp4 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum4 += timestamp4-timestamp3;
    //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "Pruing TOKEN COUNT: %d", Token::tokenCount);

    /*Token::deleteInvalidTokens();
    unsigned long timestamp6 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum6 += timestamp6-timestamp5;*/

    graph->addLM();
    unsigned long timestamp5 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sum7 += timestamp5-timestamp4;

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "clear: %d, pass: %d, viterbi: %d, pruning: %d", sum1, sum2, sum3, sum4);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "LM TOKEN COUNT: %d", Token::tokenCount);
}

/**
 * Resets the decoder. Removes all tokens.
 */
void SpeechRecognition::Decoder::ViterbiDecoder::reset() {
    /*
    for(auto iterator = graph->outputNode->tokens.begin();
            iterator != graph->outputNode->tokens.end();){
        delete *iterator;
        graph->outputNode->tokens.erase(iterator);
    }*/

    //Token::deleteInvalidTokens();
    //Token::deleteStatic();
    //graph->eraseTokenRecords();
}

//TODO add to utils
std::string buildString(SpeechRecognition::Decoder::Token& token){
    std::string result = "";
    for(auto iterator = token.wordHistory->begin();
            iterator != token.wordHistory->end();
            iterator++){
        result += (*iterator)->writtenForm + " ";
    }

    return result;
}

/**
 * Returns the word with the highest likelihood from the clearOutputNode node.
 */
std::string SpeechRecognition::Decoder::ViterbiDecoder::getWinner() {
    auto bestToken = this->graph->outputNode->tokens.front();

    return buildString(*bestToken);

    /*float maxLikelihood = -std::numeric_limits<float>::max();
    int maxIndex = -1, i = 0;
    for(auto iterator = vector.begin();
            iterator != vector.end();
            iterator++, i++){
        if((*iterator)->likelihood > maxLikelihood){
            maxLikelihood = (*iterator)->likelihood;
            maxIndex = i;
        }
    }
    return buildString(*(vector.at(maxIndex)));*/
}
