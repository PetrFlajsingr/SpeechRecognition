//
// Created by Petr Flajsingr on 30/03/2018.
//



#include <vector>
#include <AcousticModel.h>
#include <HMMGraph.h>
#include <GraphNode.h>
#include <Token.h>

std::vector<float> TEMP_PROB = {0.6, 0.4};


/**
 * Prepares root node of a graph and creates first token;
 * @param model Acoustic model
 */
HMMGraph::HMMGraph(AcousticModel* model) {
    std::vector<float> probs;
    std::vector<GraphNode*> nodes;

    float prob = 1 / model->words.size();
    for(int i = 0; i < model->words.size(); i++){
        probs.push_back(prob);
        GraphNode* node = new GraphNode(TEMP_PROB, i, 0,
                                        model->words.at(i).phonemes.at(0));
        nodes.push_back(node);
    }
    this->rootNode = new GraphNode(probs, -1, -1, NONE);
    this->rootNode->successorNodes = nodes;

    this->rootNode->tokens.push_back(new Token(rootNode));

    this->outputNode = new GraphNode(TEMP_PROB, -1, -1, NONE);
}

HMMGraph::~HMMGraph() {
    delete this->rootNode;
}

void HMMGraph::update(AcousticModel* model) {
    int i = 0;
    for(auto iterator = rootNode->successorNodes.begin();
            iterator != rootNode->successorNodes.end();
            iterator++, i++){
        if((*iterator)->tokens.empty()) {
            //destroySuccessors((*iterator));
            //} else if((*iterator)->successorNodes.empty()){
        }else{
            addSuccessors(*iterator, model, i, 1);
        }
    }
}

void HMMGraph::addSuccessors(GraphNode *node, AcousticModel* model, int wordID, int phonemeIndex) {
    if(node->successorNodes.empty()) {
        node->successorNodes.push_back(node);
        if(phonemeIndex == model->words.at(wordID).phonemes.size() - 1){
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
