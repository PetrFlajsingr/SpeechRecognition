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

class MelBankThread{
private:
    RSMelFilterBank* melFilterBank;

    VoiceActivityDetector* VADetector;

    std::thread thread;

    bool run = true;

    void threadMelBank();

    kiss_fftr_cfg cfg;
public:
    MelBankThread(const char* cacheDir);

    virtual ~MelBankThread();

    void stopThread();

    SafeQueue<Q_AudioData*> inputQueue;

    SafeQueue<Q_MelData*>* nnQueue;
};


#endif //VOICERECOGNITION_MELBANKTHREAD_H
