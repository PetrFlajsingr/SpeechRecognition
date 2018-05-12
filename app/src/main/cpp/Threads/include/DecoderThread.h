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
    /**
     * @brief Class representing a decoder thread
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class DecoderThread {
    private:
        ViterbiDecoder *decoder;

        JavaCallbacks *callbacks;

        /**
         * Method to be run in thread. Decodes data given by neural network thread and notifies listeners.
         */
        void threadDecoder();

        std::string recognitionResult = "";
    public:
        std::thread thread;

        /**
         * Starts the thread on creation
         * @param callbacks callbacks to JVM
         * @param decoder Decoder for recognition
         */
        DecoderThread(JavaCallbacks &callbacks, ViterbiDecoder* decoder);

        virtual ~DecoderThread();

        SafeQueue<Q_NNData *> inputQueue;

        std::string getResult();
    };

}


#endif //VOICERECOGNITION_DECODERTHREAD_H
