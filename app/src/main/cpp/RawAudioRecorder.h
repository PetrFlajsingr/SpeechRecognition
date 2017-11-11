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

#define ALLOW_AUDIO_LOG_TO_FILE 1

class RawAudioRecorder {
private:
    //TODO remove
    static FILE* rawFile;

    static const int SAMPLING_RATE = 8000; //kHz
    static const int SMALL_RECORDER_FRAMES = SAMPLING_RATE; // min length of recorded frame - 100ms

    static short *recorderBuffer; //< buffer for recorded data
    static int max_recording_length_sec; //< maximum allow length for a recording

    static size_t recorderSize; //< size of all recorded data;

    // engine interfaces
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;

    //stops the recording when user requests it
    static bool recordingStopBool;

    static int capacityCounter;

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

    void startRecording(int max_length_sec);

    void stopRecording();

    short* getRecording(size_t* size);
};


#endif //NATIVEAUDIO_RAWAUDIORECORDER_H
