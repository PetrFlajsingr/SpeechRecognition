//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H

#include <GraphNode.h>
#include <AcousticModel.h>

class HMMGraph {
private:
    void addSuccessors(GraphNode *node, AcousticModel* model, int wordID, int phonemeIndex);

    void destroySuccessors(GraphNode* node);
public:
    GraphNode* rootNode;
    GraphNode* outputNode;

    HMMGraph(AcousticModel* model);

    virtual ~HMMGraph();

    void update(AcousticModel* model);

    void applyViterbiCriterium(GraphNode* node);
};


#endif //VOICERECOGNITION_GRAPH_H
