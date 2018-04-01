//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <vector>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <GraphNode.h>
#include <Token.h>
#include <sstream>

//TODO temp variable, remove
std::vector<float> TEMP_PROB = {0.6, 0.4};

//TODO memory leak - uvolneni pameti

/**
 * Prepares root node of a graph and creates first token;
 * @param model Acoustic model
 */
HMMGraph::HMMGraph(AcousticModel* model) {
    std::vector<float> probs;
    std::vector<GraphNode*> nodes;


    float prob = 1.0f / model->words.size();
    for(int i = 0; i < model->words.size(); i++){
        probs.push_back(prob);
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words.at(i).phonemes.at(0));
        nodes.push_back(node);
    }
    this->rootNode = new GraphNode(probs, -2, -1, NONE);
    this->rootNode->successorNodes = nodes;

    //this->rootNode->tokens.push_back(new Token(rootNode, -1));

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
void keepMax(std::vector<Token*>* tokens){
    if(tokens->size() <= 1)
        return;
    float maxLikelihood = 0.0;
    for(auto iterator = tokens->begin();
         iterator != tokens->end();
         iterator++){
        if((*iterator)->likelihood > maxLikelihood)
            maxLikelihood = (*iterator)->likelihood;
    }

    for(auto iterator = tokens->begin();
        iterator != tokens->end();
        iterator++){
        if((*iterator)->likelihood < maxLikelihood){
            Token::addIndexToDelete((*iterator)->index_TokenVector);
            tokens->erase(iterator);
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
    keepMax(&(node->tokens));

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

std::string toString(float& value){
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string toString(int& value){
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string HMMGraph::output(AcousticModel* model) {
    std::string result = "";
    for(int i = 0; i < outputNode->tokens.size(); i++){
        result += "Word:\t" + model->words.at(outputNode->tokens.at(i)->word).writtenForm
                  + "\tLikelihood:\t" + toString(outputNode->tokens.at(i)->likelihood)
                  + "\n";
    }


    for(auto iterator = outputNode->tokens.begin();
        iterator != outputNode->tokens.end();
        iterator++){
            Token::addIndexToDelete((*iterator)->index_TokenVector);
            outputNode->tokens.erase(iterator);
            iterator--;
    }
    return result;
}

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
