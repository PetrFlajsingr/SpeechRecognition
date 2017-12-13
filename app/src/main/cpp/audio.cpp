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
#include <sstream>
#include "constants.h"
#include "MelFilterBank.h"
#include "NeuralNetwork.h"
#include <RenderScript.h>

//#include "ScriptC_test.h"
#include "../../../build/generated/source/rs/debug/ScriptC_test.h"

using namespace android::RSC;

RawAudioRecorder* recorder;

short* readAudioFromFile(std::string filepath, int* recordingSize);

const char* cacheDir;

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
    sp<RS> rs = new RS();
    rs->init(cacheDir);

    uint32_t arr1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32_t arr2[10];

    sp<const Element> e = Element::U32(rs);
    sp<const Type> t = Type::create(rs, e, 10, 0, 0);

    sp<Allocation> inputAlloc = Allocation::createTyped(
            rs, t);
    sp<Allocation> outputAlloc = Allocation::createTyped(
            rs, t);

    inputAlloc->copy1DFrom(arr1);

    ScriptC_test* sc = new ScriptC_test(rs);

    sc->forEach_root(inputAlloc, outputAlloc);

    outputAlloc->copy1DTo(arr2);

    for(int i = 0; i < 10; ++i){
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "I: %d", arr2[i]);
    }
    return;
   // int a;
    //recorder->getRecording(&a);
    const int FRAME_SIZE = (const int) (8000 * 0.025);
    const int FRAME_OVERLEAP = (const int) (8000 * 0.010);
    int recordingSize = 0;
    //short* data = recorder->getRecording(&recordingSize);
    short* data = readAudioFromFile("/sdcard/AAAaudiofile.pcm", &recordingSize);

    std::ofstream out;
    out.open("/sdcard/AAAaudiofile.txt");
    for(int i = 0; i < recordingSize; ++i){
        std::stringstream ss;
        ss << (data[i]/SHORT_MAX_FLOAT);
        out.write((ss.str() + ",").c_str(), ss.str().size()+1);
    }
    out.close();

    int frameCount = (recordingSize - FRAME_SIZE) / FRAME_OVERLEAP;

    AudioFrame::calcHammingCoef();

    AudioFrame* frames = new AudioFrame[frameCount];

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "hamming window start");
    for(unsigned int frame = 0; frame < frameCount; ++frame){
        frames[frame].applyHammingWindow(data + frame*FRAME_OVERLEAP);
    }
    free(data);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "hamming window end");

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "fft start");
    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);

    kiss_fft_cpx** fftFrames = new kiss_fft_cpx*[frameCount];

    for(unsigned int i = 0; i < frameCount; ++i){
        frames[i].applyFFT(&cfg);
        if(i == 0){
            std::ofstream out;
            out.open("/sdcard/AAAfft.txt");
            for(int j = 0; j < FFT_FRAME_LENGTH / 2 + 1; ++j){
                std::stringstream ss;
                ss << frames[i].getFftData()[j].r;
                out.write((ss.str() + ",").c_str(), ss.str().size()+1);
            }
            out.write("\n", 1);
            for(int j = 0; j < FFT_FRAME_LENGTH / 2 + 1; ++j){
                std::stringstream ss;
                ss << frames[i].getFftData()[j].i;
                out.write((ss.str() + ",").c_str(), ss.str().size()+1);
            }
            out.close();
        }
        fftFrames[i] = frames[i].getFftData();
    }
    free(cfg);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "fft end");

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "mel bank start");
    MelFilterBank::initStatic();

    MelFilterBank* melBank = new MelFilterBank();

    melBank->calculateMelBanks(frameCount, fftFrames);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "mel bank end");

    melBank->dumpResultToFile("/sdcard/AAAmelbank.txt");

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN load");
    NeuralNetwork* nn = new NeuralNetwork("/sdcard/voicerecognition/nn.bin");
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN load done");

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN forward");
    nn->setFeatureMatrix(melBank->getMelBankFrames());
    nn->forward();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN forward done");

    delete nn;
    delete[] fftFrames;
    delete[] frames;
    delete melBank;
    MelFilterBank::deleteStatic();
}

/**
 * Reads raw audio file from storage. Expects little endian 16 bit signed audio
 * @param filepath
 * @param recordingSize
 * @return
 */
short* readAudioFromFile(std::string filepath, int* recordingSize){
    std::ifstream in(filepath.c_str(), std::ifstream::binary);

    in.seekg(0, std::ios::end);
    long fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    short* data = new short[fileSize];
    in.read((char*) data, fileSize);
    *recordingSize = (int) (fileSize / 2);
    return data;
}


// shut down the native audio system
void shutdown()
{
    delete recorder;
}

void setCacheDir(const char* cDir){
    cacheDir = cDir;
}

extern "C"{
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_setCacheDir(JNIEnv* env, jclass clazz, jstring pathObj){
        setCacheDir(env->GetStringUTFChars(pathObj, NULL));
    }

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
