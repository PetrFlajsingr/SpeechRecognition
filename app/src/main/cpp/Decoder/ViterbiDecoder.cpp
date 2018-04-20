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

unsigned long sum1 = 0, sum2 = 0, sum3 = 0, cnt = 0;
/**
 * Sends data through the graph.
 * @param input clearOutputNode of NN
 */
void SpeechRecognition::Decoder::ViterbiDecoder::decode(float *input) {
    unsigned long timeStamp1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    graph->clearOutputNode();
    unsigned long timeStamp2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    graph->passTokens(input);
    unsigned long timeStamp3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    graph->applyPruning();
    unsigned long timeStamp4 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if(firstPass){
        graph->rootNode->tokens.front()->alive = false;
        firstPass = false;
    }
    sum1 += timeStamp2 - timeStamp1;
    sum2 += timeStamp3 - timeStamp2;
    sum3 += timeStamp4 - timeStamp3;
    cnt++;
    if(cnt % 100 == 0)
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "clear: %lu, pass: %lu, pruning: %lu", sum1, sum2, sum3);
}

/**
 * Resets the decoder. Removes all tokens.
 */
void SpeechRecognition::Decoder::ViterbiDecoder::reset() {
    firstPass = true;
    for(auto iterator = Token::allTokens.begin();
        iterator != Token::allTokens.end();
        iterator++){

        (*iterator)->alive = (*iterator)->currentNode == graph->rootNode;

        (*iterator)->likelihood = 0;
        if((*iterator)->alive) {
            if((*iterator)->wordLinkRecord != NULL) {
                (*iterator)->wordLinkRecord->unasign();
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

/**
 * Returns the word with the highest likelihood from the clearOutputNode node.
 */
std::string SpeechRecognition::Decoder::ViterbiDecoder::getWinner() {
    auto bestToken = Token::getBestToken(graph->outputNode);

    if(bestToken == NULL)
        return "ERR";

    bestToken->wordLinkRecord = bestToken->wordLinkRecord->addRecord(languageModel->getLMWord("</s>"));
    std::string result =  buildString(*bestToken);
    return result;
}
