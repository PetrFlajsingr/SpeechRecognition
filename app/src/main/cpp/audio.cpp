//
// Created by Petr Flajsingr on 10/11/2017.
//

#include <jni.h>
#include "RawAudioRecorder.h"
#include "AudioFrame.h"
#include "kissfft/kiss_fftr.h"
#include "kissfft/kiss_fft.h"
#include <android/log.h>
#include <vector>
#include <iostream>
#include <fstream>

#define APPNAME "cz.vutbr.fit.xflajs00.voicerecognition"

RawAudioRecorder* recorder;

// create the engine and output mix objects
void createEngine()
{
    recorder = new RawAudioRecorder();
}

// create audio recorder
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
    const int FRAME_SIZE = (const int) (8000 * 0.025);
    const int FRAME_OVERLAP = FRAME_SIZE/25*15;
    int recordingSize = 0;
    short* data = recorder->getRecording(&recordingSize);

    int frameCount = recordingSize/FRAME_OVERLAP;

    unsigned int offset = 0;

    AudioFrame* frames = new AudioFrame[frameCount];

    for(unsigned int i = 0; i < frameCount; ++i){
        frames[i].applyHammingWindow(data + offset);
        offset += FRAME_OVERLAP;
    }

    std::ofstream out;
    out.open("/sdcard/AAAafterham.txt");
    for(int i = 0; i < frameCount; ++i){
        for(int j = 0; j < 200; ++j){
            out << frames[i].hammingData[j] << ",";
        }
        out << std::endl;
    }
    out.close();

    const int FFT_LEN = 200;

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_LEN, 0, NULL, NULL);

    //std::vector<kiss_fft_cpx*> fftFrames;
    for(unsigned int i = 0; i < frameCount; ++i){
        //TODO zero fill FFT input?
        frames[i].applyFFT(&cfg);
    }
    free(cfg);
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
