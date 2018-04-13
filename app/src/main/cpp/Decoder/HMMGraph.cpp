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
#include <android/log.h>
#include <constants.h>

// TODO remove using
using namespace SpeechRecognition::Decoder;
//TODO temp variable, remove
std::vector<float> TEMP_PROB = {
        static_cast<float>(log(0.87)),
        static_cast<float>(log(0.13))
};

/**
 * Prepares root node of a graph and creates first token;
 * @param model Acoustic model
 */
SpeechRecognition::Decoder::HMMGraph::HMMGraph(AcousticModel* model) {

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

/**
 * Recursively adds nodes where it is needed.
 * @param wordID id of word in Acoustic model
 * @param phonemeIndex index of a phoneme
 */
void SpeechRecognition::Decoder::HMMGraph::addSuccessors(GraphNode *node, AcousticModel* model,
                                                         int wordID, int phonemeIndex, GraphNode* predecessor) {
    node->successorNodes.push_back(node);
    node->predecessorNodes.push_back(node);
    node->predecessorNodes.push_back(predecessor);
    // TOKEN
    for(int i = 0; i < node->predecessorNodes.size(); i++)
        node->tokens.push_back(new Token(node, false, i));
    //\ TOKEN
    if(phonemeIndex == model->words.at(wordID).phonemes.size()){
        addSILNode(node, wordID, phonemeIndex, node);
    }else {
        GraphNode *newNode = new GraphNode(
                TEMP_PROB, wordID,
                phonemeIndex, model->words.at(wordID).phonemes.at(phonemeIndex));
        node->successorNodes.push_back(newNode);

        addSuccessors(newNode, model, wordID, phonemeIndex + 1, node);
    }
}

/**
 * Adds/removes states from the graph.
 * @param model Acoustic model
 */
void SpeechRecognition::Decoder::HMMGraph::build(AcousticModel *model) {
    // root
    std::vector<float> probabilities;
    for(int i = 0; i < model->words.size(); i++)
        probabilities.push_back(0);

    std::vector<GraphNode*> nodes;

    this->rootNode = new GraphNode(probabilities, -2, -1, NONE);

    for(int i = 0; i < model->words.size(); i++){
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words.at(i).phonemes.at(0));
        nodes.push_back(node);
    }

    this->rootNode->successorNodes = nodes;

    this->rootNode->tokens.push_back(new Token(rootNode, false, 0));

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

/**
 * Deletes all nodes behind the given one.
 * @param node
 */
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

// TODO add to object
/**
 * Iterates through all tokens in a node, finds the one with highest likelihood.
 */
void keepMax(std::vector<Token*>& tokens){


    float maxLikelihood = -std::numeric_limits<float>::max();
    unsigned int maxIndex = 0;
    unsigned int i = 0;
    for(auto iterator = tokens.begin();
         iterator != tokens.end();
         iterator++, i++){
        if((*iterator)->alive && (*iterator)->likelihood > maxLikelihood) {
            maxLikelihood = (*iterator)->likelihood;
            maxIndex = i;
        }
    }

    i = 0;
    for(auto iterator = tokens.begin();
            iterator != tokens.end();
            iterator++, i++){
        if((*iterator)->alive) {
            (*iterator)->alive = (i == maxIndex);
        }
    }
}

/**
 * Recursively iterates through all nodes in a graph and find tokens with highest likelihood.
 */
void SpeechRecognition::Decoder::HMMGraph::applyViterbiCriterium(GraphNode* node) {
    if(node != rootNode)
        keepMax(node->tokens);

    // exit condition
    if(node == outputNode)
        return;

    int offset = 1;
    if(node == rootNode)
        offset = 0;
    //skips loopback node
    for(auto nodeIterator = node->successorNodes.begin() + offset;
            nodeIterator != node->successorNodes.end();
            nodeIterator++){
        applyViterbiCriterium(*nodeIterator);
    }
}



/**
 * Debug clearOutputNode
 * @param model
 * @return
 */
void SpeechRecognition::Decoder::HMMGraph::clearOutputNode() {
    Token* bestToken = Token::getBestToken(outputNode);
    if(bestToken != NULL) {
        rootNode->tokens.front()->wordHistory = bestToken->wordHistory;
        rootNode->tokens.front()->likelihood = bestToken->likelihood;
    }
}

/**
 * Recursive deletion of GraphNodes
 */
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


void SpeechRecognition::Decoder::HMMGraph::deleteLowLikelihood(std::vector<Token*>& tokens){
    if(tokens.size() <= MAX_TOKEN_COUNT)
        return;

    for(auto iterator = tokens.begin() + MAX_TOKEN_COUNT;
        iterator != tokens.end();
        iterator++){
        (*iterator)->alive = false;
    }

    tokens.clear();
}

void SpeechRecognition::Decoder::HMMGraph::searchTokens(std::vector<std::vector<Token *>> &allTokens, GraphNode* node, unsigned int level) {
    if(node == outputNode)
        return;

    if(allTokens.size() < level + 1){
        std::vector<Token*> vector;
        allTokens.push_back(vector);
    }
    // root node doesn't matter - it starts of the recursion
    if(node == rootNode) {
        for(auto iterator = node->successorNodes.begin();
                iterator != node->successorNodes.end();
                iterator++){
            searchTokens(allTokens, *iterator, level);
        }
        return;
    }else {
        for(auto iterator = node->tokens.begin();
                iterator != node->tokens.end();
                iterator++){
            if((*iterator)->alive)
                allTokens.at(level).push_back(*iterator);
        }
    }

    searchTokens(allTokens, node->successorNodes.at(1), level + 1);
}

/**
 * Marks tokens with low likelihood for deletion.
 * Viterbi criterium has to be applied before using this function (only one token per node).
 */
void SpeechRecognition::Decoder::HMMGraph::applyPruning() {
    // each inner vector represents tokens in one level
    std::vector<std::vector<Token*>> allTokens;

    // for filling the vectors
    searchTokens(allTokens, rootNode, 0);

    // sorting by likelihood - descending
    for(auto iterator = allTokens.begin();
            iterator != allTokens.end();
            iterator++){
        std::sort((*iterator).begin(), (*iterator).end(),
                  [](const Token* ls, const Token* rs){
                      return ls->likelihood > rs->likelihood;
                  });
        deleteLowLikelihood(*iterator);
    }

    allTokens.clear();
}

void SpeechRecognition::Decoder::HMMGraph::applyViterbiCriterium() {
    applyViterbiCriterium(rootNode);
}

void SpeechRecognition::Decoder::HMMGraph::addLM() {
    for(auto iterator = outputNode->tokens.begin();
            iterator != outputNode->tokens.end();
            iterator++){
        if((*iterator)->alive)
            (*iterator)->likelihood += WORD_INSERTION_PENALTY + (*iterator)->wordHistory.back()->unigramScore * SCALE_FACTOR_LM;
    }
}

void HMMGraph::passTokens(GraphNode *node, float* input) {
    /*if(node == outputNode)
        return;*/

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
            for(auto iterator = node->tokens.begin();
                iterator != node->tokens.end();
                iterator++) {
                (*iterator)->passInGraph(input);
            }
        }
    }
}

void HMMGraph::passTokens(float *input) {
    passOutputNode(input);
    passTokens(rootNode, input);
}

void HMMGraph::addTokensToOutputNode() {
    for(int i = 0; i < outputNode->predecessorNodes.size(); i++){
        outputNode->tokens.push_back(new Token(outputNode, true, i));
    }
}

void HMMGraph::passOutputNode(float* input) {
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++) {
        (*iterator)->passInGraph(input);
    }
}
