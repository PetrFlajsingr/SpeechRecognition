//
// Created by Petr Flajsingr on 31/03/2018.
//

#ifndef VOICERECOGNITION_GRAPHNODE_H
#define VOICERECOGNITION_GRAPHNODE_H


#include <vector>

class GraphNode {
public:
    std::vector<float> pathProbablity;
    std::vector<GraphNode*> nextNode;

    GraphNode(const std::vector<float> &pathProbablity, const std::vector<GraphNode *> &nextNode);
};


#endif //VOICERECOGNITION_GRAPHNODE_H
