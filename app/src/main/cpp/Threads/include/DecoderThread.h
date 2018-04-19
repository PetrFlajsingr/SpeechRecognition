//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_DECODERTHREAD_H
#define VOICERECOGNITION_DECODERTHREAD_H


#include <thread>
#include <ViterbiDecoder.h>
#include <QueueData.h>
#include <SafeQueue.h>
#include "JavaCallbacks.h"
namespace SpeechRecognition::Threads {
    using namespace Decoder;
    class DecoderThread {
    private:
        ViterbiDecoder *decoder;

        JavaCallbacks *callbacks;

        void threadDecoder();

        std::string recognitionResult = "";
    public:
        std::thread thread;

        DecoderThread(JavaCallbacks &callbacks, ViterbiDecoder* decoder);

        virtual ~DecoderThread();

        SafeQueue<Q_NNData *> inputQueue;

        std::string getResult();
    };

}


#endif //VOICERECOGNITION_DECODERTHREAD_H
