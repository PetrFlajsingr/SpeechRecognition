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

class RawAudioRecorder {
private:
    static const int SAMPLING_RATE = 48000; //kHz
    static const int SMALL_RECORDER_FRAMES = SAMPLING_RATE * 0.015; // min length of recorded frame - 100ms

    static short *recorderBuffer; //< buffer for recorded data

    static int recorderSize; //< size of all recorded data;

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

    short* getRecording(int* size);
};


#endif //NATIVEAUDIO_RAWAUDIORECORDER_H
