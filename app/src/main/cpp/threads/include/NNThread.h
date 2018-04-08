//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_NNTHREAD_H
#define VOICERECOGNITION_NNTHREAD_H


#include <thread>
#include <RSNeuralNetwork.h>
#include <SafeQueue.h>
#include <QueueData.h>


class NNThread{
private:
    RSNeuralNetwork* neuralNetwork;
    std::thread thread;

    bool run = true;

    void threadNN();

    void prepareInput(float* result, std::vector<float*>& data);

    void deleteBuffer(std::vector<float*>& data);
public:
    NNThread(const char* cacheDir);

    virtual ~NNThread();

    SafeQueue<Q_MelData*> inputQueue;

    SafeQueue<Q_NNData*>* decoderQueue;
};


#endif //VOICERECOGNITION_NNTHREAD_H
