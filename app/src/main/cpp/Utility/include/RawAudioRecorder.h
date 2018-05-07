//
// Created by Petr Flajsingr on 10/11/2017.
// Creates necessary objects for recording audio using OPENSL.
// Usage: Constructor, createAudioRecorder(), startRecording(<length of a buffer>)
//

#ifndef NATIVEAUDIO_RAWAUDIORECORDER_H
#define NATIVEAUDIO_RAWAUDIORECORDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>
#include <pthread.h>
#include <condition_variable>
#include <constants.h>
#include <QueueData.h>
#include <SafeQueue.h>

namespace SpeechRecognition::Utility {
    class RawAudioRecorder {
    private:
        static short *recorderBuffer; //< buffer for recorded data

        static bool recording;

        // engine interfaces
        SLObjectItf engineObject = NULL;
        SLEngineItf engineEngine;

        //stops the recording when user requests it
        static bool recordingStopFlag;

        // recorder interfaces
        SLObjectItf recorderObject = NULL;
        static SLRecordItf recorderRecord;
        static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

        static pthread_mutex_t audioEngineLock;

        void createEngine();

        static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    public:
        RawAudioRecorder();

        bool createAudioRecorder();

        virtual ~RawAudioRecorder();

        void startRecording();

        void stopRecording();

        static bool isRecording();

        static SafeQueue<Q_AudioData *> *melQueue;
    };
}


#endif //NATIVEAUDIO_RAWAUDIORECORDER_H
