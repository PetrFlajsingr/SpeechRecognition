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

using namespace SpeechRecognition;
using namespace Feature_Extraction;
using namespace Threads;
using namespace VoiceActivityDetection;
using namespace Decoder;
using namespace Utility;

SpeechRecognitionAPI* speechRecognitionAPI = NULL;

void test(char* cachedir, std::string filePath){
    WavReader reader;

    std::ifstream ifstream;
    ifstream.open(filePath);

    AudioFrame frame;

    kiss_fft_cpx *fftFrame;

    AudioFrame::calcHammingCoef();
    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);

    RSMelFilterBank melFilterBank(cachedir);

    if(ifstream.is_open()) {
        std::vector<std::vector<float*>> results;

        std::vector<float*> tmp;
        results.push_back(tmp);
        short *audioData = reader.wavToPcm(ifstream);
        // MEL TEST

        VoiceActivityDetector VADetector;
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start mel");
        bool notified = false;

        unsigned int segments = 0;
        unsigned long melStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for(unsigned int i = 0; i < reader.getDataSize(); i += SUBSAMPLED_OVERLAP_LENGTH) {
            short data[AUDIO_FRAME_LENGTH];

            std::copy(audioData + i, audioData + AUDIO_FRAME_LENGTH + i, data);

            frame.applyHammingWindow(data);

            frame.applyFFT(&cfg);

            fftFrame = frame.getFftData();

            float *result = melFilterBank.calculateMelBank(fftFrame);

            delete[] fftFrame;

            VADetector.checkData(result);

            if(VADetector.isActive()){
                for(auto iterator = VADetector.getBuffer().begin();
                    iterator != VADetector.getBuffer().end();){
                    result = *iterator;
                    melFilterBank.normalise(result);
                    results[segments].push_back(result);
                    VADetector.getBuffer().erase(iterator);
                }
                if(!notified) {
                    notified = true;
                }
            } else {
                if(notified) {
                    notified = false;
                    std::vector<float*> newVector;
                    results.push_back(newVector);
                    segments++;
                }
            }
        }
        unsigned long melEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


        FeatureMatrix matrix[segments];

        for(int i = 0; i < results.size(); i++){
            matrix[i].init(results[i].size(), 24);
            for(int j = 0; j < results[i].size(); j++) {
                matrix[i].setFeatureMatrixFrame(j, results[i][j]);
            }
        }

        //NN TEST
        RSNeuralNetwork neuralNetwork("/sdcard/devel/NNnew.bin", cachedir);
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start nn");
        unsigned long nnStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        FeatureMatrix* nnResults[segments];

        for(int i = 0; i < segments; i++){
            nnResults[i] = neuralNetwork.forwardAll(&matrix[i]);
        }

        unsigned long nnEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


        ViterbiDecoder decoder("/sdcard/devel/lexicon.bin", "/sdcard/devel/lm.arpa");
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start decoder");
        unsigned long decoderStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for(int i = 0; i < segments; i++){
            for(int j = 0; j < nnResults[i]->getFramesNum(); j++)
                decoder.decode(nnResults[i]->getFeaturesMatrix()[j]);
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", decoder.getWinner().c_str());
            decoder.reset();
        }
        unsigned long decoderEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "MEL: %lu, NN: %lu, DECODER: %lu",
                melEndTime - melStartTime, nnEndTime - nnStartTime, decoderEndTime - decoderStartTime);
    }
}


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
    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_setCacheDirNative
            (JNIEnv* env, jobject obj, jstring pathObj){
        const char* cacheDir = env->GetStringUTFChars(pathObj, NULL);
        if(speechRecognitionAPI != NULL){
            delete speechRecognitionAPI;
        }
        speechRecognitionAPI = new SpeechRecognitionAPI(cacheDir);
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

    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_testNative
            (JNIEnv* env, jobject obj, jstring str){

        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test1.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test2.wav");
        speechRecognitionAPI->recognizeWav("/sdcard/Audio/test3.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test4.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test5.wav");
        /*test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test1.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test2.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test3.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test4.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test5.wav");*/
    }
}
