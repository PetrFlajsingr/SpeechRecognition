//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <vector>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <GraphNode.h>
#include <Token.h>
#include <sstream>
#include <cmath>
#include <Utils.h>
#include <limits>
#include <algorithm>

//TODO temp variable, remove
std::vector<float> TEMP_PROB = {
        static_cast<float>(log(0.6)),
        static_cast<float>(log(0.4))
};

/**
 * Prepares root node of a graph and creates first token;
 * @param model Acoustic model
 */
HMMGraph::HMMGraph(AcousticModel* model) {
    std::vector<float> probabilities;
    std::vector<GraphNode*> nodes;

    for(int i = 0; i < model->words.size(); i++){
        probabilities.push_back(0);
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words.at(i).phonemes.at(0));
        nodes.push_back(node);
    }
    this->rootNode = new GraphNode(probabilities, -2, -1, NONE);
    this->rootNode->successorNodes = nodes;

    this->outputNode = new GraphNode(TEMP_PROB, -1, -1, NONE);
}

HMMGraph::~HMMGraph() {
    destroyGraph(rootNode);
    delete outputNode;
}

// TODO dynamic
/**
 * Adds/removes states from the graph.
 * @param model Acoustic model
 */
void HMMGraph::update(AcousticModel* model) {
    int i = 0;
    for(auto iterator = rootNode->successorNodes.begin();
            iterator != rootNode->successorNodes.end();
            iterator++, i++){
        //if((*iterator)->tokens.empty()) {
            //destroySuccessors((*iterator));
        //}else{
            addSuccessors(*iterator, model, i, 1);
        //}
    }
}

/**
 * Recursively adds nodes where it is needed.
 * @param wordID id of word in Acoustic model
 * @param phonemeIndex index of a phoneme
 */
void HMMGraph::addSuccessors(GraphNode *node, AcousticModel* model, int wordID, int phonemeIndex) {
    if(node->successorNodes.empty()) {
        node->successorNodes.push_back(node);
        if(phonemeIndex == model->words.at(wordID).phonemes.size()){
            node->successorNodes.push_back(this->outputNode);
        }else{
            GraphNode* newNode = new GraphNode(
                    TEMP_PROB, wordID,
                    phonemeIndex, model->words.at(wordID).phonemes.at(phonemeIndex));
            node->successorNodes.push_back(newNode);

            addSuccessors(newNode, model, wordID, phonemeIndex + 1);
        }
    }
}

/**
 * Deletes all nodes behind the given one.
 * @param node
 */
void HMMGraph::destroySuccessors(GraphNode *node) {
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

/**
 * Iterates through all tokens in a node, finds the one with highest likelihood.
 */
void keepMax(std::vector<Token*>& tokens){
    if(tokens.size() <= 1)
        return;
    float maxLikelihood = -std::numeric_limits<float>::max();
    for(auto iterator = tokens.begin();
         iterator != tokens.end();
         iterator++){
        if((*iterator)->likelihood > maxLikelihood)
            maxLikelihood = (*iterator)->likelihood;
    }

    for(auto iterator = tokens.begin();
        iterator != tokens.end();
        iterator++){
        if((*iterator)->likelihood < maxLikelihood){
            Token::addIndexToDelete((*iterator)->index_TokenVector);
            tokens.erase(iterator);
            iterator--;
        }
    }
}

/**
 * Recursively iterates through all nodes in a graph and find tokens with highest likelihood.
 */
void HMMGraph::applyViterbiCriterium(GraphNode* node) {
    // exit condition
    if(node == outputNode)
        return;

    if(node != rootNode)
        keepMax(node->tokens);

    if(node->successorNodes.size() <= 1)
        return;
    int offset = 0;
    if(node != rootNode)
        offset = 1;
    //skips loopback node
    for(auto nodeIterator = node->successorNodes.begin() + offset;
            nodeIterator != node->successorNodes.end();
            nodeIterator++){
        applyViterbiCriterium(*nodeIterator);
    }
}

/**
 * Returns most likely word.
 * @param vector Vector of tokens to check
 * @param model Acoustic model
 */
std::string getMostLikely(std::vector<Token*>& vector, AcousticModel* model){
    int index = -1;
    float maxSoFar = -std::numeric_limits<float>::max();
    int i = 0;
    for(auto iterator = vector.begin();
            iterator != vector.end();
            iterator++, i++){
        if((*iterator)->likelihood > maxSoFar){
            maxSoFar = (*iterator)->likelihood;
            index = i;
        }
    }

    return model->words.at(vector.at(index)->word).writtenForm;
}

/**
 * Debug clearOutputNode
 * @param model
 * @return
 */
void HMMGraph::clearOutputNode() {
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();){
        (*iterator)->currentNode = rootNode;
        rootNode->tokens.push_back(*iterator);
        outputNode->tokens.erase(iterator);
    }


    return;
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++){
            Token::addIndexToDelete((*iterator)->index_TokenVector);
            outputNode->tokens.erase(iterator);
            iterator--;
    }
}

/**
 * Recursive deletion of GraphNodes
 */
void HMMGraph::destroyGraph(GraphNode* node) {
    // exit condition
    if(node == outputNode)
        return;
    for(auto iterator = node->successorNodes.begin() + 1;
            iterator != node->successorNodes.end();
            iterator++){
        destroyGraph(*iterator);
    }
    delete node;
}

/**
 * Recursive deletion of Token records inside nodes.
 * @param node
 */
void HMMGraph::eraseTokenRecords(GraphNode* node) {
    node->tokens.clear();
    // exit condition
    if(node == outputNode)
        return;
    int offset = 1;
    if(node == rootNode)
        offset = 0;
    for(auto iterator = node->successorNodes.begin() + offset;
            iterator != node->successorNodes.end();
            iterator++){
        eraseTokenRecords(*iterator);
    }
}

void HMMGraph::deleteLowLikelihood(std::vector<Token*>& tokens){
    if(tokens.size() <= MAX_TOKEN_COUNT)
        return;
    for(auto iterator = tokens.begin() + MAX_TOKEN_COUNT;
            iterator != tokens.end();
            iterator++){
        Token::addIndexToDelete((*iterator)->index_TokenVector);
        (*iterator)->currentNode->tokens.clear();
    }

    tokens.clear();
}

void HMMGraph::searchTokens(std::vector<std::vector<Token *>> &allTokens, GraphNode* node, unsigned int level) {
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
    }else if(!node->tokens.empty()) {
        allTokens.at(level).push_back(node->tokens.at(0));
    }

    searchTokens(allTokens, node->successorNodes.at(1), level + 1);
}

/**
 * Marks tokens with low likelihood for deletion.
 * Viterbi criterium has to be applied before using this function (only one token per node).
 */
void HMMGraph::applyPruning() {
    /*
     * TODO get vector of all tokens in a "level"
     * sort using Token::sortByLikelihood
     * keep MAX_TOKEN_COUNT of best tokens
     * register the rest for deletion using Token::addIndexToDelete
     * remove record from node.tokens
     * destroy the vector
     */

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

void HMMGraph::applyViterbiCriterium() {
    applyViterbiCriterium(rootNode);
}

void HMMGraph::eraseTokenRecords() {
    eraseTokenRecords(rootNode);
}

void HMMGraph::saveWords() {
    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();){
        (*iterator)->addWordToHistory();
    }
}
