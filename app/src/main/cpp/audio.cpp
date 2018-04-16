//
// Created by Petr Flajsingr on 10/11/2017.
//


#include <rsCppStructs.h>
#include <RawAudioRecorder.h>
#include <AudioFrame.h>
#include <android/log.h>
#include <FeatureMatrix.h>
#include <RSMelFilterBank.h>
#include <RSNeuralNetwork.h>
#include <LanguageModel.h>
#include <AcousticModel.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <AudioSubsampler.h>
#include <Decoder.h>
#include <Utils.h>
#include <VoiceActivityDetector.h>
#include <unistd.h>
#include <MelBankThread.h>
#include <NNThread.h>
#include <DecoderThread.h>
#include <JavaCallbacks.h>
#include <WavReader.h>

using namespace SpeechRecognition;
using namespace Feature_Extraction;
using namespace Threads;
using namespace VoiceActivityDetection;
using namespace Decoder;
using namespace Utility;

static JavaVM *g_VM;

JavaCallbacks callbacks;

std::vector<T_registeredObject> callbackObjects;

using namespace android::RSC;

RawAudioRecorder* recorder;

short* readAudioFromFile(std::string filepath, int* recordingSize);

const char* cacheDir;

// create the engine and clearOutputNode mix objects
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

void threads(){
    MelBankThread melBankThread(cacheDir, callbacks);
    NNThread nnThread(cacheDir);
    DecoderThread decoderThread(callbacks);

    nnThread.callbacks = &callbacks;

    recorder->melQueue = &melBankThread.inputQueue;
    melBankThread.nnQueue = &nnThread.inputQueue;
    nnThread.decoderQueue = &decoderThread.inputQueue;

    recorder->startRecording();

    melBankThread.thread.join();
    nnThread.thread.join();
    decoderThread.thread.join();
}

// set the recording state for the audio recorder
void startRecording()
{
   std::thread thread(threads);
    thread.detach();
}

//\ threads

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

void vawtest(){
    WavReader reader;

    std::ifstream file;

    file.open("/sdcard/Audio/test.wav", std::ios::in|std::ios::binary);
    if(file.is_open()){

        short* audioData = reader.wavToPcm(file);
        if(audioData == NULL){
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "error audio read: %s", reader.getErrorMessage().c_str());
            return;
        }


        std::ofstream out;
        out.open("/sdcard/Audio/test.pcm", std::ios::out|std::ios::binary);

        if(out.is_open()) {
            out.write((char*)audioData, reader.getDataSize() * 2);

            out.close();
        }

        delete[] audioData;


        file.close();
    }

}

extern "C"{
    // CALLBACKS
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_registerCallbacksNative
            ( JNIEnv* env, jobject obj){
        T_registeredObject object;
        object.obj = env->NewGlobalRef(obj);
        object.clazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
        object.clazz = (jclass)env->NewGlobalRef(object.clazz);
        env->GetJavaVM(&g_VM);
        callbacks.setJavaVM(g_VM);
        callbacks.addListener(object);

        callbackObjects.push_back(object);
    }
    //\ CALLBACKS

    // CONTROL FUCTIONS
    //setCacheDir
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_setCacheDirNative
            (JNIEnv* env, jobject obj, jstring pathObj){
        setCacheDir(env->GetStringUTFChars(pathObj, NULL));
    }

    //createEngine
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_createEngineNative
            (JNIEnv* env, jobject obj){
        createEngine();
    }

    //createAudioRecorder
    JNIEXPORT jboolean JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_createAudioRecorderNative
            (JNIEnv* env, jobject obj){
        return createAudioRecorder();
    }

    //startRecording
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_startRecordingNative
            (JNIEnv* env, jobject obj){
        startRecording();
    }

    //stopRecording
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_stopRecordingNative
            (JNIEnv* env, jobject obj){
        stopRecording();
    }

    //shutdown
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_shutdownNative
            (JNIEnv* env, jobject obj){
        shutdown();
    }

    //test
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_testNative
            (JNIEnv* env, jobject obj){
        vawtest();
    }
}
