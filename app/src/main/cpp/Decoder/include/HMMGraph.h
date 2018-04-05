//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H

#include <GraphNode.h>
#include <AcousticModel.h>

class HMMGraph {
private:
    const unsigned int MAX_TOKEN_COUNT = 15; //< pruning: max count per "step" (1 level of graph)

    void addSuccessors(GraphNode *node, AcousticModel* model, int wordID, int phonemeIndex);

    void destroySuccessors(GraphNode* node);

    void destroyGraph(GraphNode* node);

    void applyViterbiCriterium(GraphNode* node);

    void eraseTokenRecords(GraphNode* node);

    void deleteLowLikelihood(std::vector<Token*>& tokens);

    void searchTokens(std::vector<std::vector<Token*>>& allTokens, GraphNode* node, unsigned int level);
public:
    GraphNode* rootNode;
    GraphNode* outputNode;

    HMMGraph(AcousticModel* model);

    virtual ~HMMGraph();

    void update(AcousticModel* model);

    void applyViterbiCriterium();

    void applyPruning();

    std::string output(AcousticModel* model);

    void eraseTokenRecords();
};


#endif //VOICERECOGNITION_GRAPH_H
