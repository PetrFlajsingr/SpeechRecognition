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
    std::vector<float> probs;
    std::vector<GraphNode*> nodes;

    for(int i = 0; i < model->words.size(); i++){
        probs.push_back(0);
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words.at(i).phonemes.at(0));
        nodes.push_back(node);
    }
    this->rootNode = new GraphNode(probs, -2, -1, NONE);
    this->rootNode->successorNodes = nodes;

    this->outputNode = new GraphNode(TEMP_PROB, -1, -1, NONE);
}

HMMGraph::~HMMGraph() {
    destroyGraph(rootNode);
    delete outputNode;
}

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
 * Iterates through all nodes in a graph and find tokens with highest likelihood.
 */
void HMMGraph::applyViterbiCriterium(GraphNode* node) {
    if(node == outputNode)
        return;
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
 * Debug output
 * @param model
 * @return
 */
std::string HMMGraph::output(AcousticModel* model) {
    std::string result = "";
    for(int i = 0; i < outputNode->tokens.size(); i++){
        std::string tabs = "\t";
        if(model->words.at(outputNode->tokens.at(i)->word).writtenForm.length() < 4)
            tabs += "\t";
        result += "Word:\t" + model->words.at(outputNode->tokens.at(i)->word).writtenForm
                  + tabs + "Likelihood:\t" + toString(outputNode->tokens.at(i)->likelihood)
                  + "\n";
    }
    if(outputNode->tokens.size() > 0)
        result += "Most likely: " + getMostLikely(outputNode->tokens, model) + "\n";

    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++){
            Token::addIndexToDelete((*iterator)->index_TokenVector);
            outputNode->tokens.erase(iterator);
            iterator--;
    }
    return result;
}

/**
 * Recursive deletion of GraphNodes
 */
void HMMGraph::destroyGraph(GraphNode* node) {
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
