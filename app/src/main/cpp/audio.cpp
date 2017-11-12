//
// Created by Petr Flajsingr on 10/11/2017.
//

#include <jni.h>
#include "RawAudioRecorder.h"
#include "AudioFrame.h"
#include <android/log.h>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"

RawAudioRecorder* recorder;

// create the engine and output mix objects
void createEngine()
{
    recorder = new RawAudioRecorder();
}

// create audio recorder: recorder is not in fast path
//    like to avoid excessive re-sampling while playing back from Hello & Android clip
jboolean createAudioRecorder()
{
    return (jboolean) (recorder->createAudioRecorder() ? JNI_TRUE : JNI_FALSE);
}

void stopRecording(){
    recorder->stopRecording();
}


// set the recording state for the audio recorder
void startRecording(jint max_length_sec)
{
    recorder->startRecording(max_length_sec);
}

void createFrames(){
    const size_t FRAME_SIZE = 8000*0.025;
    const size_t FRAME_OVERLAP = FRAME_SIZE/25*15;
    size_t recordingSize = 0;
    short* data = recorder->getRecording(&recordingSize);
    size_t frameCount = recordingSize/FRAME_OVERLAP;

    unsigned int offset = 0;

    AudioFrame::openFile();

    AudioFrame* frames = new AudioFrame[frameCount];

    //first frame is special
    frames[0].applyHammingWindow(data);
    /*for(unsigned int j = 0; j < FRAME_SIZE; ++j) {
        frames[0].data[j] = data[j];
    }*/


    for(unsigned int i = 1; i < frameCount; ++i){
        offset += FRAME_OVERLAP;
        frames[i].applyHammingWindow(data + offset);
        //__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "offset: %d, recordSize: %d, frameCount: %d", offset, recordingSize, frameCount);
    }

    AudioFrame::closeFile();
}


// shut down the native audio system
void shutdown()
{
    delete recorder;
}

extern "C"{
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_createEngine(JNIEnv* env, jclass clazz){
        createEngine();
    }

    JNIEXPORT jboolean JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_createAudioRecorder(JNIEnv* env, jclass clazz){
        return createAudioRecorder();
    }

    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_stopRecording(JNIEnv* env, jclass clazz){
        stopRecording();
    }

    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_startRecording(JNIEnv* env, jclass clazz, jint max_length_sec){
        startRecording(max_length_sec);
    }

    JNIEXPORT void Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_shutdown(JNIEnv* env, jclass clazz){
        shutdown();
    }

    JNIEXPORT void Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_createFrames(JNIEnv* env, jclass clazz){
         createFrames();
    }
}
