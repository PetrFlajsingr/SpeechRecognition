//
// Created by Petr Flajsingr on 10/11/2017.
//

#include <assert.h>
#include <RawAudioRecorder.h>
#include <algorithm>
#include <android/log.h>

bool SpeechRecognition::Utility::RawAudioRecorder::recordingStopBool = false;
pthread_mutex_t SpeechRecognition::Utility::RawAudioRecorder::audioEngineLock = PTHREAD_MUTEX_INITIALIZER;
SLAndroidSimpleBufferQueueItf SpeechRecognition::Utility::RawAudioRecorder::recorderBufferQueue;
short* SpeechRecognition::Utility::RawAudioRecorder::recorderBuffer;
SLRecordItf SpeechRecognition::Utility::RawAudioRecorder::recorderRecord;
bool SpeechRecognition::Utility::RawAudioRecorder::recording = false;
SafeQueue<Q_AudioData*>* SpeechRecognition::Utility::RawAudioRecorder::melQueue;

/**
 * Prepares the engine for recording audio.
 */
SpeechRecognition::Utility::RawAudioRecorder::RawAudioRecorder() {
    createEngine();
}

/**
 * Prepares interfaces necessary for recording audio
 */
void SpeechRecognition::Utility::RawAudioRecorder::createEngine() {
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
void SpeechRecognition::Utility::RawAudioRecorder::bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    assert(bq == recorderBufferQueue);
    assert(NULL == context);
    SLresult result;
    // recording stopped by user
    if(recordingStopBool) {
        // stopping the recording engine
        result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RECORDER: stopbool set");
        recording = false;
    }else {
        result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recorderBuffer,
                                                     SMALL_RECORDER_FRAMES * sizeof(short));
    }

    short* data = new short[SMALL_RECORDER_FRAMES];
    std::copy(recorderBuffer, recorderBuffer + SMALL_RECORDER_FRAMES,
                data);
    melQueue->enqueue(new Q_AudioData{SEQUENCE_DATA, data});

    if(!recording){
        melQueue->enqueue(new Q_AudioData{TERMINATE, data});
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RECORDER: TERMINATE");
    }

    pthread_mutex_unlock(&audioEngineLock);
}

/**
 * Prepares audio recorder with OPENSL.
 * Settings: 48 kHz sample rate, little endian, 16 bit, raw data (PCM)
 * @return true if OK, otherwise false
 */
bool SpeechRecognition::Utility::RawAudioRecorder::createAudioRecorder() {
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
void SpeechRecognition::Utility::RawAudioRecorder::stopRecording() {
    recordingStopBool = true;
}

/**
 * Starts recording. Allocates the necessary memory, sets the OPENSL parameters.
 * @param max_length_sec maximum length of recorded audio
 */
void SpeechRecognition::Utility::RawAudioRecorder::startRecording() {
    if(recorderBuffer != NULL){
        delete[] recorderBuffer;
    }

    recorderBuffer = new short[SMALL_RECORDER_FRAMES];
    SLresult result;

    if (pthread_mutex_trylock(&audioEngineLock)) {
        return;
    }
    recordingStopBool = false;
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

    recording = true;
}

/**
 * Destroys all used audio objects and releases allocated memory.
 */
SpeechRecognition::Utility::RawAudioRecorder::~RawAudioRecorder() {
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

bool SpeechRecognition::Utility::RawAudioRecorder::isRecording() {
    return recording;
}
