//
// Created by Petr Flajsingr on 18/04/2018.
//

#ifndef VOICERECOGNITION_FILESTREAMTHREAD_H
#define VOICERECOGNITION_FILESTREAMTHREAD_H


#include <WavReader.h>
#include <thread>
#include "QueueData.h"
#include "SafeQueue.h"

namespace SpeechRecognition::Threads {
    class FileStreamThread {
    private:
        WavReader wavReader;

        void threadFileStream();

        std::ifstream* fileStream;

        std::mutex startMutex;

        std::condition_variable conditionVariable;
    public:
        FileStreamThread(std::ifstream &fileStream);

        std::thread thread;

        SafeQueue<Q_AudioData *> *melQueue;

        void start();
    };
}


#endif //VOICERECOGNITION_FILESTREAMTHREAD_H
