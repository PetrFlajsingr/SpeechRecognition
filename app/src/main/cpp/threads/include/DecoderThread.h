//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_DECODERTHREAD_H
#define VOICERECOGNITION_DECODERTHREAD_H


#include <thread>
#include <Decoder.h>
#include <QueueData.h>
#include <SafeQueue.h>

class DecoderThread {
private:
    Decoder* decoder;


    bool run = true;

    void threadDecoder();
public:
    std::thread thread;
    DecoderThread();

    virtual ~DecoderThread();

    SafeQueue<Q_NNData*> inputQueue;
};


#endif //VOICERECOGNITION_DECODERTHREAD_H
