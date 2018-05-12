//
// Created by Petr Flajsingr on 07/04/2018.
//

#ifndef VOICERECOGNITION_MELBANKTHREAD_H
#define VOICERECOGNITION_MELBANKTHREAD_H


#include <thread>
#include <RSMelFilterBank.h>
#include <SafeQueue.h>
#include <QueueData.h>
#include <kiss_fftr.h>
#include <VoiceActivityDetector.h>
#include "JavaCallbacks.h"
namespace SpeechRecognition::Threads {
    using namespace Feature_Extraction;
    using namespace VoiceActivityDetection;

    /**
     * @brief Class representing a Mel/VAD thread
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class MelBankThread {
    private:
        RSMelFilterBank *melFilterBank;

        VoiceActivityDetector *VADetector;

        JavaCallbacks *callbacks;

        /**
         * Method to be run in thread. Buffers audio sent from recoder.
         */
        void threadMelBank();

        /**
         * Prepares data from buffer - segmentation
         * @param data buffered data
         * @param newData output data
         */
        void prepareAudioData(short *data, short *newData);

        bool subsample;
    public:
        /**
         * Starts the thread on creation
         * @param cacheDir cache dir for RenderScript
         * @param callbacks callbacks to JVM
         * @param subsample from WAV?
         */
        MelBankThread(JavaCallbacks &callbacks, RSMelFilterBank* melFilterBank, bool subsample);

        virtual ~MelBankThread();

        void stopThread();

        SafeQueue<Q_AudioData *> inputQueue;

        SafeQueue<Q_MelData *> *nnQueue;

        std::thread thread;
    };
}


#endif //VOICERECOGNITION_MELBANKTHREAD_H
