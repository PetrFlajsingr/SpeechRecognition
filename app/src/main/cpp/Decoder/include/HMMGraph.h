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

        void applyBeamPruning(std::vector<Token *> &tokens);

        void searchTokens(std::vector<std::vector<Token *>> &allTokens, GraphNode *node,
                          unsigned int level);

        void addSILNode(GraphNode *node, int wordID, int phonemeIndex, GraphNode* predecessor);

        void passTokens(GraphNode* node, float* input);

        void addTokensToOutputNode();

        void passOutputNode(float* input);

        std::vector<std::vector<Token*>> tokensForBeamPruning;
    public:
        GraphNode *rootNode;
        GraphNode *outputNode;

        HMMGraph(AcousticModel *model);

        virtual ~HMMGraph();

        void build(AcousticModel *model);

        void applyPruning();

        void clearOutputNode();

        void passTokens(float* input);

        static float getBigramMapValue(Token* token);
    };
}

#endif //VOICERECOGNITION_GRAPH_H
