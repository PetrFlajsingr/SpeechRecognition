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
     * @brief Class representing a node in a graph
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class GraphNode {
    public:
        // indexes of these vectors correlate
        std::vector<float> pathProbablity; //< probability of path to the next node

        std::vector<GraphNode *> predecessorNodes; //< next nodes
        std::vector<GraphNode *> successorNodes; //< next nodes

        std::vector<Token *> tokens; //< tokens currently residing in this node

        Token* bestToken = NULL;

        int wordID; //< id of acoustic model ("row")

        short xPos; //< position in sequence ("column")

        PHONEME inputVectorIndex; //< index for NN clearOutputNode

        /**
         * Creates a node for a graph
         * @param pathProbablity probability of transition
         * @param wordID id of a word represented by this node
         * @param xPos position in a word
         * @param inputVectorIndex index of neural network output
         */
        GraphNode(const std::vector<float> &pathProbablity, int wordID, short xPos,
                  PHONEME inputVectorIndex);
    };
}

#endif //VOICERECOGNITION_GRAPHNODE_H
