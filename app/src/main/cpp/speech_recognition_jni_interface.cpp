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
#include <ViterbiDecoder.h>
#include <Utils.h>
#include <VoiceActivityDetector.h>
#include <unistd.h>
#include <MelBankThread.h>
#include <NNThread.h>
#include <DecoderThread.h>
#include <JavaCallbacks.h>
#include <WavReader.h>
#include <FileStreamThread.h>
#include <SpeechRecognitionAPI.h>
#include <sstream>
#include <Exceptions.h>

using namespace SpeechRecognition;
using namespace Feature_Extraction;
using namespace Threads;
using namespace VoiceActivityDetection;
using namespace Decoder;
using namespace Utility;

SpeechRecognitionAPI* speechRecognitionAPI = NULL;

extern "C"{
    // CALLBACKS
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_registerCallbacksNative
            ( JNIEnv* env, jobject obj){
        JavaVM *g_VM;

        T_registeredObject object;
        object.obj = env->NewGlobalRef(obj);
        object.clazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
        object.clazz = (jclass)env->NewGlobalRef(object.clazz);
        env->GetJavaVM(&g_VM);

        speechRecognitionAPI->callbacks.setJavaVM(g_VM);
        speechRecognitionAPI->callbacks.addListener(object);
    }
    //\ CALLBACKS

    // CONTROL FUCTIONS
    //setCacheDir
    JNIEXPORT jstring JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_setCacheDirNative
            (JNIEnv* env, jobject obj, jstring pathObj){
        const char* cacheDir = env->GetStringUTFChars(pathObj, NULL);
        if(speechRecognitionAPI != NULL){
            delete speechRecognitionAPI;
        }

        try{
        speechRecognitionAPI = new SpeechRecognitionAPI(cacheDir);
        }catch(Exceptions::ASRFilesMissingException& e){
            return env->NewStringUTF(e.what());
        }
        return env->NewStringUTF("");
    }

    //startRecording
    JNIEXPORT jboolean JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_startRecordingNative
            (JNIEnv* env, jobject obj){
       // startRecording();
        return (speechRecognitionAPI->startRecording() ? JNI_TRUE : JNI_FALSE);
    }

    //stopRecording
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_stopRecordingNative
            (JNIEnv* env, jobject obj){
        speechRecognitionAPI->stopRecording();
    }

    //shutdown
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_shutdownNative
            (JNIEnv* env, jobject obj){
        //shutdown();
        if(speechRecognitionAPI != NULL)
            delete speechRecognitionAPI;
    }

    JNIEXPORT jboolean JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_isRecordingNative
            (JNIEnv* env, jobject obj){
        return (speechRecognitionAPI->isRecording() ? JNI_TRUE : JNI_FALSE);
    }

    JNIEXPORT jstring JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_recognizeWAVNative
            (JNIEnv* env, jobject obj, jstring path){
        return env->NewStringUTF(speechRecognitionAPI->recognizeWav(env->GetStringUTFChars(path, NULL)).c_str());
     }
}
