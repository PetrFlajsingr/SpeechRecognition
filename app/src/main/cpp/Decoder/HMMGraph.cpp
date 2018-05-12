//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <vector>
#include <list>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <GraphNode.h>
#include <Token.h>
#include <sstream>
#include <cmath>
#include <Utils.h>
#include <limits>
#include <algorithm>
#include <chrono>
#include <Bigram.h>
#include <thread>
#include <constants.h>
#include <android/log.h>


SpeechRecognition::Decoder::HMMGraph::HMMGraph(AcousticModel* model) {
    acousticModel = model;
}

SpeechRecognition::Decoder::HMMGraph::~HMMGraph() {
    destroyGraph(rootNode);
    delete outputNode;
}

void SpeechRecognition::Decoder::HMMGraph::addSILNode(GraphNode *node,
                                                      int wordID, int phonemeIndex,
                                                      GraphNode* predecessor) {
    std::vector<float> TO_SIL_PROB = {
            static_cast<float>(log(0.80)),
            static_cast<float>(log(0.10)),
            static_cast<float>(log(0.10))
    };
    node->pathProbablity = TO_SIL_PROB;
    GraphNode* newNode = new GraphNode(
            TEMP_PROB, wordID,
            phonemeIndex, SIL);
    node->successorNodes.push_back(newNode);
    node->successorNodes.push_back(outputNode);

    newNode->predecessorNodes.push_back(newNode);
    newNode->predecessorNodes.push_back(predecessor);
    newNode->successorNodes.push_back(newNode);
    newNode->successorNodes.push_back(outputNode);

    // TOKEN
    for(int i = 0; i < newNode->predecessorNodes.size(); i++)
        newNode->tokens.push_back(new Token(newNode, false, i));
    //\ TOKEN

    outputNode->predecessorNodes.push_back(node);
    outputNode->predecessorNodes.push_back(newNode);
}

void SpeechRecognition::Decoder::HMMGraph::addSuccessors(GraphNode *node, AcousticModel* model,
                                                         int wordID, int phonemeIndex, GraphNode* predecessor) {
    node->successorNodes.push_back(node);
    node->predecessorNodes.push_back(node);
    node->predecessorNodes.push_back(predecessor);
    // TOKEN
    for(int i = 0; i < node->predecessorNodes.size(); i++)
        node->tokens.push_back(new Token(node, false, i));
    //\ TOKEN
    if(phonemeIndex == model->words[wordID].phonemes.size()){
        addSILNode(node, wordID, phonemeIndex, node);
    }else {
        GraphNode *newNode = new GraphNode(
                TEMP_PROB, wordID,
                phonemeIndex, model->words[wordID].phonemes[phonemeIndex]);
        node->successorNodes.push_back(newNode);

        addSuccessors(newNode, model, wordID, phonemeIndex + 1, node);
    }
}

void SpeechRecognition::Decoder::HMMGraph::build(AcousticModel *model) {
    // root
    std::vector<float> probabilities;
    for(int i = 0; i < model->words.size(); i++)
        probabilities.push_back(0);

    std::vector<GraphNode*> nodes;

    this->rootNode = new GraphNode(probabilities, -2, -1, NONE);

    for(int i = 0; i < model->words.size(); i++){
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words[i].phonemes[0]);
        nodes.push_back(node);
    }

    this->rootNode->successorNodes = nodes;

    this->rootNode->tokens.push_back(new Token(rootNode, false, 0));
    this->rootNode->bestToken = this->rootNode->tokens.front();

    this->outputNode = new GraphNode(TEMP_PROB, -1, -2, NONE);
    //\root

    int i = 0;
    for(auto iterator = rootNode->successorNodes.begin();
            iterator != rootNode->successorNodes.end();
            iterator++, i++){
        addSuccessors(*iterator, model, i, 1, rootNode);
    }

    addTokensToOutputNode();
}

void SpeechRecognition::Decoder::HMMGraph::destroySuccessors(GraphNode *node) {
    for(auto iterator = node->successorNodes.begin();
            iterator != node->successorNodes.end();
            iterator++){
        if((*iterator)->tokens.empty() && (*iterator) != node){
            destroySuccessors((*iterator));
        }
        delete *iterator;
    }
    node->successorNodes.clear();
}

void SpeechRecognition::Decoder::HMMGraph::clearOutputNode() {
    Token* bestToken = Token::getBestToken(outputNode);
    if(bestToken != NULL) {
        rootNode->tokens.front()->wordLinkRecord->unassign();
        rootNode->tokens.front()->wordLinkRecord = bestToken->wordLinkRecord->assign();
        rootNode->tokens.front()->likelihood = bestToken->likelihood;
        rootNode->tokens.front()->alive = true;
    }
}

void SpeechRecognition::Decoder::HMMGraph::destroyGraph(GraphNode* node) {
    // exit condition
    if(node == outputNode)
        return;

    int offset = 1;
    if(node == rootNode)
        offset = 0;
    for(auto iterator = node->successorNodes.begin() + offset;
            iterator != node->successorNodes.end();
            iterator++){
        destroyGraph(*iterator);
    }
    delete node;
}

void SpeechRecognition::Decoder::HMMGraph::applyPruning() {
    Token::livingTokens.clear();
    return;
    if(Token::livingTokens.size() < LIVE_STATES_PRUNING_LIMIT)
        return;

    std::sort(Token::livingTokens.begin(), Token::livingTokens.end(),
              [](const Token* ls, const Token* rs){
                  return ls->likelihood > rs->likelihood;
              });

    float threshold = Token::livingTokens[0]->likelihood - BEAM_PRUNING_LIMIT;

    if(threshold > Token::livingTokens[LIVE_STATES_PRUNING_LIMIT]->likelihood){
        unsigned long vecLength = Token::livingTokens.size();
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "TOKENS ALIVE: %lu", Token::livingTokens.size());
        for(int i = 1; i < vecLength; i++){
            if(Token::livingTokens[i]->likelihood < threshold)
                Token::livingTokens[i]->alive = false;
        }
    }
    unsigned long vecLength = Token::livingTokens.size();
    for(int i = LIVE_STATES_PRUNING_LIMIT; i < vecLength; i++){
        Token::livingTokens[i]->alive = false;
    }

    Token::livingTokens.clear();

}

void SpeechRecognition::Decoder::HMMGraph::passTokens(SpeechRecognition::Decoder::GraphNode *node, float* input) {
    if(node != outputNode) {
        int offset = 1;
        if(node == rootNode)
            offset = 0;
        if(node->successorNodes.size() > 1) {
            for(auto iterator = node->successorNodes.begin() + offset;
                iterator != node->successorNodes.end();
                iterator++) {
                passTokens(*iterator, input);
            }
        }

        if(node != rootNode) {
            float maxLikelihood = -std::numeric_limits<float>::max();
            float likelihood;
            unsigned int maxIndex = 0, i = 0;
            for(auto iterator = node->tokens.begin();
                iterator != node->tokens.end();
                iterator++, i++) {
                likelihood = (*iterator)->passInGraph(input);
                if((*iterator)->alive && likelihood > maxLikelihood){
                    maxLikelihood = likelihood;
                    maxIndex = i;
                }
            }

            i = 0;
            for(auto iterator = node->tokens.begin();
                iterator != node->tokens.end();
                iterator++, i++){
                if((*iterator)->alive) {
                    (*iterator)->alive = (i == maxIndex);
                    if((*iterator)->alive)
                        (*iterator)->currentNode->bestToken = (*iterator);
                }
            }
        }
    }
}

void SpeechRecognition::Decoder::HMMGraph::passTokens(float *input) {
    passOutputNode(input);
    passTokens(rootNode, input);
}

void SpeechRecognition::Decoder::HMMGraph::addTokensToOutputNode() {
    for(int i = 0; i < outputNode->predecessorNodes.size(); i++){
        outputNode->tokens.push_back(new Token(outputNode, true, i));
    }
}

void SpeechRecognition::Decoder::HMMGraph::passOutputNode(float* input) {
    float maxLikelihood = -std::numeric_limits<float>::max();
    float likelihood;
    unsigned int maxIndex = 0, i = 0;
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++, i++) {
        likelihood = (*iterator)->passInGraph(input);
        if((*iterator)->alive && likelihood > maxLikelihood){
            maxLikelihood = likelihood;
            maxIndex = i;
        }
    }

    i = 0;
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++, i++){
        if((*iterator)->alive) {
            (*iterator)->alive = (i == maxIndex);
            if((*iterator)->alive)
                outputNode->bestToken = (*iterator);
        }
    }
}
