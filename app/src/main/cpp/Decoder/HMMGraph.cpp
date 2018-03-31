//
// Created by Petr Flajsingr on 30/03/2018.
//

#include "include/HMMGraph.h"

HMMGraph::HMMGraph(AcousticModel* model) {
    std::vector<float> probs;
    std::vector<GraphNode*> nodes;

    float prob = 1/model->words.size();
    for(int i = 0; i < model->words.size(); i++){
        probs.push_back(prob);
        nodes.push_back(new GraphNode(nullptr, nullptr));
    }
    this->rootNode = new GraphNode(probs, nodes);
}

HMMGraph::~HMMGraph() {

}
