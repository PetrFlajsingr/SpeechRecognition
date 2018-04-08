//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_DECODERTHREAD_H
#define VOICERECOGNITION_DECODERTHREAD_H


#include <thread>
#include <Decoder.h>
#include <QueueData.h>
#include <SafeQueue.h>
#include "JavaCallbacks.h"

class DecoderThread {
private:
    Decoder* decoder;

    JavaCallbacks* callbacks;

    void threadDecoder();
public:
    std::thread thread;
    DecoderThread(JavaCallbacks& callbacks);

    virtual ~DecoderThread();

    SafeQueue<Q_NNData*> inputQueue;
};


#endif //VOICERECOGNITION_DECODERTHREAD_H
