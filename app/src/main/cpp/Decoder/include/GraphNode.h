//
// Created by Petr Flajsingr on 31/03/2018.
//

#ifndef VOICERECOGNITION_GRAPHNODE_H
#define VOICERECOGNITION_GRAPHNODE_H


#include <vector>
#include <GraphNodeFWD.h>
#include <TokenFWD.h>
#include <Phoneme.h>
#include <list>

namespace SpeechRecognition::Decoder {
/**
 * Represents one node in the HMM graph.
 */
    class GraphNode {
    public:
        // indexes of these vectors correlate
        std::vector<float> pathProbablity; //< probability of path to the next node

        std::vector<GraphNode *> predecessorNodes; //< next nodes
        std::vector<GraphNode *> successorNodes; //< next nodes

        std::vector<Token *> tokens; //< tokens currently residing in this node

        int wordID; //< id of acoustic model ("row")

        short xPos; //< position in sequence ("column")

        PHONEME_ENUM inputVectorIndex; //< index for NN clearOutputNode

        GraphNode(const std::vector<float> &pathProbablity, int wordID, short xPos,
                  PHONEME_ENUM inputVectorIndex);
    };
}

#endif //VOICERECOGNITION_GRAPHNODE_H
