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
#include "constants.h"

class RawAudioRecorder {
private:
    static const int SMALL_RECORDER_FRAMES = SAMPLING_RATE * 0.010; // min length of recorded frame - 100ms

    static short *recorderBuffer; //< buffer for recorded data

    static short *sharedAudioData;

    static unsigned int dataCounter;
public:
    static unsigned int getDataCounter();

private:

    static std::condition_variable* cv;

    static bool recording;
public:
    static bool isRecording();

public:
    static void setCv(std::condition_variable *cv);

public:
    static void setSharedAudioData(short *sharedAudioData);

private:

    // engine interfaces
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;

    //stops the recording when user requests it
    static bool recordingStopBool;

    // recorder interfaces
    SLObjectItf recorderObject = NULL;
    static SLRecordItf recorderRecord;
    static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

    static pthread_mutex_t  audioEngineLock;

    void createEngine();

    static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

public:
    RawAudioRecorder();

    bool createAudioRecorder();

    virtual ~RawAudioRecorder();

    void startRecording();

    void stopRecording();
};


#endif //NATIVEAUDIO_RAWAUDIORECORDER_H
