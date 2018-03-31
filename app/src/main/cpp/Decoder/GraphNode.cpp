//
// Created by Petr Flajsingr on 31/03/2018.
//

#include "include/GraphNode.h"

GraphNode::GraphNode(const std::vector<float> &pathProbablity,
                     const std::vector<GraphNode *> &nextNode) : pathProbablity(pathProbablity),
                                                                 nextNode(nextNode) {}
