//
// Created by Petr Flajsingr on 31/03/2018.
//


#include <vector>
#include <Phoneme.h>
#include <GraphNode.h>

GraphNode::GraphNode(const std::vector<float> &pathProbablity, int wordID, short xPos,
                     PHONEME_ENUM inputVectorIndex) : pathProbablity(pathProbablity),
                                                      wordID(wordID), xPos(xPos),
                                                      inputVectorIndex(inputVectorIndex) {}
