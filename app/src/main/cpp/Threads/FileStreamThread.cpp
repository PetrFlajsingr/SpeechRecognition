//
// Created by Petr Flajsingr on 18/04/2018.
//

#include "FileStreamThread.h"
#include <constants.h>
#include <android/log.h>

SpeechRecognition::Threads::FileStreamThread::FileStreamThread(std::ifstream& fileStream)
        : thread(&FileStreamThread::threadFileStream, this)  {
    this->fileStream = &fileStream;
}


void SpeechRecognition::Threads::FileStreamThread::threadFileStream() {
    std::unique_lock<std::mutex> lock(startMutex);
    conditionVariable.wait(lock);

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "FILESTREAM: START");
    short* data = wavReader.wavToPcm(*fileStream);

    Q_AudioData* audioData;

    if(data == NULL) {
        audioData = new Q_AudioData{TERMINATE, NULL};
        melQueue->enqueue(audioData);

        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "FILESTREAM: BAD END");
        return;
    }

    int framesSent = 0;
    for(int i = 0; i < wavReader.getDataSize(); i += SUBSAMPLED_OVERLAP_LENGTH){
        short* dataToSend = new short[SUBSAMPLED_OVERLAP_LENGTH];
        std::copy(data + i, data + i + SUBSAMPLED_OVERLAP_LENGTH,
                  dataToSend);

        audioData = new Q_AudioData{SEQUENCE_DATA, dataToSend};
        melQueue->enqueue(audioData);

        framesSent++;
    }

    audioData = new Q_AudioData{TERMINATE, NULL};
    melQueue->enqueue(audioData);

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "FILESTREAM: END");
}

void SpeechRecognition::Threads::FileStreamThread::start() {
    conditionVariable.notify_all();
}
