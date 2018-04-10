//
// Created by Petr Flajsingr on 30/03/2018.
//

#ifndef VOICERECOGNITION_GRAPH_H
#define VOICERECOGNITION_GRAPH_H

#include <GraphNode.h>
#include <AcousticModel.h>

namespace SpeechRecognition::Decoder {
    class HMMGraph {
    private:
        const unsigned int MAX_TOKEN_COUNT = 500; //< pruning: max count per "step" (1 level of graph)

        const float WORD_INSERTION_PENALTY = -15;

        void addSuccessors(GraphNode *node, AcousticModel *model, int wordID, int phonemeIndex);

        void destroySuccessors(GraphNode *node);

        void destroyGraph(GraphNode *node);

        void applyViterbiCriterium(GraphNode *node);

        void eraseTokenRecords(GraphNode *node);

        void deleteLowLikelihood(std::vector<Token *> &tokens);

        void searchTokens(std::vector<std::vector<Token *>> &allTokens, GraphNode *node,
                          unsigned int level);

        void addSILNode(GraphNode *node, int wordID, int phonemeIndex);

    public:
        GraphNode *rootNode;
        GraphNode *outputNode;

        HMMGraph(AcousticModel *model);

        virtual ~HMMGraph();

        void build(AcousticModel *model);

        void applyViterbiCriterium();

        void applyPruning();

        void clearOutputNode();

        void eraseTokenRecords();

        void addLM();
    };
}

#endif //VOICERECOGNITION_GRAPH_H
