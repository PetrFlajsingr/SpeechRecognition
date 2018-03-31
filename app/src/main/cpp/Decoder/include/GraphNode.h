//
// Created by Petr Flajsingr on 31/03/2018.
//

#ifndef VOICERECOGNITION_GRAPHNODE_H
#define VOICERECOGNITION_GRAPHNODE_H


#include <vector>
#include <GraphNodeFWD.h>
#include <TokenFWD.h>
#include <Phoneme.h>

class GraphNode {
public:
    std::vector<float> pathProbablity;
    std::vector<GraphNode*> successorNodes;

    std::vector<Token*> tokens;

    int wordID; //< id of acoustic model ("row")

    short xPos; //< position in sequence ("column")

    PHONEME_ENUM inputVectorIndex;

    GraphNode(const std::vector<float> &pathProbability,
              int wordID, short xPos, PHONEME_ENUM inputVectorIndex);
};


#endif //VOICERECOGNITION_GRAPHNODE_H
