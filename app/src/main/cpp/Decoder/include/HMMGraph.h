//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H

#include <GraphNode.h>
#include <AcousticModel.h>
#include <mutex>

namespace SpeechRecognition::Decoder {
    class HMMGraph {
    private:
        void addSuccessors(GraphNode *node, AcousticModel *model, int wordID, int phonemeIndex, GraphNode* predecessor);

        void destroySuccessors(GraphNode *node);

        void destroyGraph(GraphNode *node);

        void addSILNode(GraphNode *node, int wordID, int phonemeIndex, GraphNode* predecessor);

        void passTokens(GraphNode* node, float* input);

        void addTokensToOutputNode();

        void passOutputNode(float* input);

        AcousticModel* acousticModel;
    public:
        GraphNode *rootNode;
        GraphNode *outputNode;

        HMMGraph(AcousticModel *model);

        virtual ~HMMGraph();

        void build(AcousticModel *model);

        void applyPruning();

        void clearOutputNode();

        void passTokens(float* input);
    };
}

#endif //VOICERECOGNITION_GRAPH_H
