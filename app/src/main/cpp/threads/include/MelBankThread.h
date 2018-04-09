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

class MelBankThread{
private:
    RSMelFilterBank* melFilterBank;

    VoiceActivityDetector* VADetector;

    JavaCallbacks* callbacks;

    void threadMelBank();

    void prepareAudioData(short* data, short* newData);
public:
    MelBankThread(const char* cacheDir, JavaCallbacks& callbacks);

    virtual ~MelBankThread();

    void stopThread();

    SafeQueue<Q_AudioData*> inputQueue;

    SafeQueue<Q_MelData*>* nnQueue;

    std::thread thread;
};


#endif //VOICERECOGNITION_MELBANKTHREAD_H