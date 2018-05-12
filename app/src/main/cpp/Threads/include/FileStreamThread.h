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
    using namespace Utility;
    /**
     * @brief Class representing a thread providing data from file
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class FileStreamThread {
    private:
        WavReader wavReader;

        /**
         * Method to be run in thread
         */
        void threadFileStream();

        std::ifstream* fileStream;

        std::mutex startMutex;

        std::condition_variable conditionVariable;
    public:
        /**
         * Starts the thread on creation
         * @param fileStream input file stream
         */
        FileStreamThread(std::ifstream &fileStream);

        std::thread thread;

        SafeQueue<Q_AudioData *> *melQueue;

        /**
         * Unlocks the thread
         */
        void start();
    };
}


#endif //VOICERECOGNITION_FILESTREAMTHREAD_H
