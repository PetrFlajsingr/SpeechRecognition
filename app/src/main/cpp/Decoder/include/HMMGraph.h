//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H

#include <GraphNode.h>
#include <AcousticModel.h>
#include <mutex>

namespace SpeechRecognition::Decoder {
    /**
     * @brief Represents recognition network/graph
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class HMMGraph {
    private:
        /**
         * Recursively adds nodes where it is needed.
         * @param wordID id of word in Acoustic model
         * @param phonemeIndex index of a phoneme
         */
        void addSuccessors(GraphNode *node, AcousticModel *model, int wordID, int phonemeIndex, GraphNode* predecessor);

        /**
         * Recursive destruction of graph
         * @param node starting node
         */
        void destroySuccessors(GraphNode *node);

        /**
         * Recursive deletion of GraphNodes
         */
        void destroyGraph(GraphNode *node);

        /**
         * Adds SIL node to the end of the word and links it
         * @param node predecessor node
         * @param wordID id of a word
         * @param phonemeIndex index in a word
         * @param predecessor
         */
        void addSILNode(GraphNode *node, int wordID, int phonemeIndex, GraphNode* predecessor);

        /**
         * Pass tokens in graph. Recursive
         * @param node current node
         * @param input NN output
         */
        void passTokens(GraphNode* node, float* input);

        /**
         * Allocate tokens for ouput node
         */
        void addTokensToOutputNode();

        /**
         * Pass tokens back to start of graph
         */
        void passOutputNode(float* input);

        AcousticModel* acousticModel;
    public:
        GraphNode *rootNode;
        GraphNode *outputNode;

        /**
         * @param model Acoustic model
         */
        HMMGraph(AcousticModel *model);

        /**
         * Free graph memory - including nodes
         */
        virtual ~HMMGraph();

        /**
         * Adds states to the graph. Builds it based on provided acoustic model
         * @param model Acoustic model
         */
        void build(AcousticModel *model);

        /**
         * Marks tokens with low likelihood for deletion.
         * Viterbi criterium has to be applied before using this function (only one token per node).
         */
        void applyPruning();

        /**
         * Debug clearOutputNode
         */
        void clearOutputNode();

        /**
         * Pass all tokens in graph
         * @param input NN output
         */
        void passTokens(float* input);
    };
}

#endif //VOICERECOGNITION_GRAPH_H
