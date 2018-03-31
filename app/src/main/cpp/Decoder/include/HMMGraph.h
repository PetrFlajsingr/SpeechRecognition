//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H


#include "GraphNode.h"
#include "AcousticModel.h"

class HMMGraph {
public:
    GraphNode* rootNode;

    HMMGraph(AcousticModel* model);

    virtual ~HMMGraph();
};


#endif //VOICERECOGNITION_GRAPH_H
