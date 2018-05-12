//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_NNTHREAD_H
#define VOICERECOGNITION_NNTHREAD_H


#include <thread>
#include <RSNeuralNetwork.h>
#include <SafeQueue.h>
#include <QueueData.h>
#include "JavaCallbacks.h"

namespace SpeechRecognition::Threads {
    using namespace Feature_Extraction;

    /**
     * @brief Class representing a neural network thread
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class NNThread {
    private:
        RSNeuralNetwork *neuralNetwork;

        /**
         * Method to be run in a thread. Takes data from inputQueue.
         */
        void threadNN();

        /**
         * Prepares the input for neural network
         */
        void prepareInput(float *result, std::vector<float *> &data);

        /**
         * Deletion of frame buffer. Called on finishing sequence.
         */
        void deleteBuffer(std::vector<float *> &data);

    public:
        /**
         * Starts thread on creation
         * @param cacheDir cache dir for Renderscript
         */
        NNThread(RSNeuralNetwork *neuralNetwork);

        virtual ~NNThread();

        SafeQueue<Q_MelData *> inputQueue;

        SafeQueue<Q_NNData *> *decoderQueue;

        std::thread thread;

        JavaCallbacks *callbacks;
    };
}


#endif //VOICERECOGNITION_NNTHREAD_H
