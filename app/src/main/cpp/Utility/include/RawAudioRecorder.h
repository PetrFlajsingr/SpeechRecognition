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
    /**
     * @brief Class implementing native audio recording using OPENSL library
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
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

        /**
         * Prepares interfaces necessary for recording audio
         */
        void createEngine();

        /**
         * Gets called when a queued buffer is full. Prepares the next part of a buffer or stops recording.
         * @param bq queue of buffers
         */
        static void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    public:
        /**
         * Prepares the engine for recording audio.
         */
        RawAudioRecorder();

        /**
         * Prepares audio recorder with OPENSL.
         * Settings: 48 kHz sample rate, little endian, 16 bit, raw data (PCM)
         * @return true if OK, otherwise false
         */
        bool createAudioRecorder();

        /**
         * Destroys all used audio objects and releases allocated memory.
         */
        virtual ~RawAudioRecorder();

        /**
         * Starts recording. Allocates the necessary memory, sets the OPENSL parameters.
         * @param max_length_sec maximum length of recorded audio
         */
        void startRecording();

        /**
         * Stops recording when buffer (recordBuffer) is full
         */
        void stopRecording();

        static bool isRecording();

        static SafeQueue<Q_AudioData *> *melQueue;
    };
}


#endif //NATIVEAUDIO_RAWAUDIORECORDER_H
