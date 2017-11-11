//
// Created by Petr Flajsingr on 10/11/2017.
//

#include "RawAudioRecorder.h"
#include <assert.h>

#include "AudioSubsampler.h"

bool RawAudioRecorder::recordingStopBool = false;
int RawAudioRecorder::capacityCounter = 0;
pthread_mutex_t RawAudioRecorder::audioEngineLock = PTHREAD_MUTEX_INITIALIZER;
FILE* RawAudioRecorder::rawFile;
SLAndroidSimpleBufferQueueItf RawAudioRecorder::recorderBufferQueue;
short* RawAudioRecorder::recorderBuffer;
SLRecordItf RawAudioRecorder::recorderRecord;
int RawAudioRecorder::max_recording_length_sec;
size_t RawAudioRecorder::recorderSize;

/**
 * Prepares the engine for recording audio.
 */
RawAudioRecorder::RawAudioRecorder() {
    createEngine();
}

/**
 * Prepares interfaces necessary for recording audio
 */
void RawAudioRecorder::createEngine() {
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

/**
 * Gets called when a queued buffer is full. Prepares the next part of a buffer or stops recording.
 * @param bq queue of buffers
 */
void RawAudioRecorder::bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    assert(bq == recorderBufferQueue);
    assert(NULL == context);
    SLresult result;
    // recording stopped by user
    if(recordingStopBool) {
        // stopping the recording engine
        result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
        // saving the length of recorded data
        if(SL_RESULT_SUCCESS == result) {
            recorderSize = SAMPLING_RATE * capacityCounter;
        }
        #if ALLOW_AUDIO_LOG_TO_FILE
            //TODO remove this
            short* toWrite = AudioSubsampler::subsample48kHzto8kHz(recorderBuffer, recorderSize);
            fwrite(toWrite, recorderSize/6, 1, rawFile);
            fclose(rawFile);
        #endif
    }else {
        capacityCounter++;
        // checking for max allowed length of the recording
        if(capacityCounter < max_recording_length_sec) {
            // queueing the next part of the buffer
            result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recorderBuffer + (capacityCounter * SAMPLING_RATE),
                                                     SMALL_RECORDER_FRAMES * sizeof(short));
        }else{
            // ending the recording
            result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
            // saving the size of the recorded data
            if(SL_RESULT_SUCCESS == result)
                recorderSize = max_recording_length_sec * SAMPLING_RATE;
        }
    }
    pthread_mutex_unlock(&audioEngineLock);
}

/**
 * Prepares audio recorder with OPENSL.
 * Settings: 44.1 kHz sample rate, little endian, 16 bit, raw data (PCM)
 * @return true if OK, otherwise false
 */
bool RawAudioRecorder::createAudioRecorder() {
    SLresult result;

    // configure audio source
    SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT,
                                      SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
    SLDataSource audioSrc = {&loc_dev, NULL};

    // configure audio sink
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_48,
                                   SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    // create audio recorder
    // (requires the RECORD_AUDIO permission)
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioRecorder(engineEngine, &recorderObject, &audioSrc,
                                                  &audioSnk, 1, id, req);
    if (SL_RESULT_SUCCESS != result) {
        return false;
    }

    // realize the audio recorder
    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return false;
    }

    // get the record interface
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecord);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // get the buffer queue interface
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                             &recorderBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // register callback on the buffer queue
    result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue, RawAudioRecorder::bqRecorderCallback,
                                                      NULL);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    return true;
}

/**
 * Stops current recording
 */
void RawAudioRecorder::stopRecording() {
    recordingStopBool = true;
}

/**
 * Starts recording. Allocates the necessary memory, sets the OPENSL parameters.
 * @param max_length_sec maximum length of recorded audio
 */
void RawAudioRecorder::startRecording(int max_length_sec) {
    #if ALLOW_AUDIO_LOG_TO_FILE
        rawFile = fopen("/sdcard/AAA.pcm", "wb");
    #endif
    if(recorderBuffer != NULL){
        delete[] recorderBuffer;
    }
    max_recording_length_sec = max_length_sec;

    recorderBuffer = new short[max_recording_length_sec * SAMPLING_RATE];
    SLresult result;

    if (pthread_mutex_trylock(&audioEngineLock)) {
        return;
    }
    recordingStopBool = false;
    capacityCounter = 0;
    // in case already recording, stop recording and clear buffer queue
    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
    result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // queues the first part of a buffer
    result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recorderBuffer,
                                             SMALL_RECORDER_FRAMES * sizeof(short));


    assert(SL_RESULT_SUCCESS == result);
    (void)result;

    // start recording
    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_RECORDING);
    assert(SL_RESULT_SUCCESS == result);
    (void)result;
}

/**
 * Destroys all used audio objects and releases allocated memory.
 */
RawAudioRecorder::~RawAudioRecorder() {
    // destroy audio recorder object, and invalidate all associated interfaces
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if(recorderBuffer != NULL)
        delete[] recorderBuffer;

    pthread_mutex_destroy(&audioEngineLock);
}

short *RawAudioRecorder::getRecording(size_t *size) {
    *size = recorderSize/6;
    return  AudioSubsampler::subsample48kHzto8kHz(recorderBuffer, recorderSize);
}
