//
// Created by Petr Flajsingr on 31/03/2018.
//


#include <vector>
#include <Phoneme.h>
#include <GraphNode.h>

/**
 * Creates a node for a graph
 * @param pathProbablity probability of transition
 * @param wordID id of a word represented by this node
 * @param xPos position in a word
 * @param inputVectorIndex index of neural network output
 */
SpeechRecognition::Decoder::GraphNode::GraphNode(const std::vector<float> &pathProbablity, int wordID, short xPos,
                     PHONEME inputVectorIndex) : pathProbablity(pathProbablity),
                                                      wordID(wordID), xPos(xPos),
                                                      inputVectorIndex(inputVectorIndex) {}
