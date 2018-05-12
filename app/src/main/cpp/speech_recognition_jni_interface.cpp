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

std::string test(char* cachedir, std::string filePath){
    /*std::ifstream filestream;
    filestream.open(filePath);
    JavaCallbacks callbacks;

    FileStreamThread fileStreamThread(filestream);
    MelBankThread melBankThread(cachedir, callbacks, false);
    NNThread nnThread(cachedir);

    std::thread thread(&saveshit, filePath + ".result");

    nnThread.callbacks = &callbacks;

    fileStreamThread.melQueue = &melBankThread.inputQueue;
    melBankThread.nnQueue = &nnThread.inputQueue;
    nnThread.decoderQueue = &queue;

    fileStreamThread.start();

    fileStreamThread.thread.join();
    melBankThread.thread.join();
    nnThread.thread.join();
    thread.join();

    return;
    */
    WavReader reader;

    std::ifstream ifstream;
    ifstream.open(filePath);

    AudioFrame frame;

    kiss_fft_cpx *fftFrame;

    AudioFrame::calculateHammingCoefficients();
    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);

    RSMelFilterBank melFilterBank(cachedir);

    if(ifstream.is_open()) {

        std::vector<float*> results;

        short *audioData = reader.wavToPcm(ifstream);
        // MEL TEST

        VoiceActivityDetector VADetector;
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start mel");
        bool notified = false;
        unsigned long melStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for(unsigned int i = 0; i < reader.getDataSize(); i += SUBSAMPLED_OVERLAP_LENGTH) {
            short data[AUDIO_FRAME_LENGTH];

            std::copy(audioData + i, audioData + AUDIO_FRAME_LENGTH + i, data);

            frame.applyHammingWindow(data);

            frame.applyFFT(&cfg);

            fftFrame = frame.getFftData();

            float *result = melFilterBank.calculateMelBank(fftFrame);

            delete[] fftFrame;

            melFilterBank.normalise(result);

            results.push_back(result);

           /* VADetector.checkData(result);

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
            }*/
        }
        unsigned long melEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        FeatureMatrix matrix;

            matrix.initialize(results.size(), 24);
            for(int i = 0; i < results.size(); i++) {
                matrix.setFeaturesMatrixFrame(i, results[i]);
            }

        //NN TEST
        RSNeuralNetwork neuralNetwork("/sdcard/devel/nn.bin", cachedir);
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start nn");
        unsigned long nnStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        FeatureMatrix* nnResults;

        //for(int i = 0; i < results.size(); i++){
            nnResults = neuralNetwork.forwardAll(&matrix);
        //}

        unsigned long nnEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        std::ostringstream ostringstream;

        ostringstream << "MEL: " << melEndTime - melStartTime <<" NN: " <<nnEndTime - nnStartTime;

        return ostringstream.str();
        /*
       ViterbiDecoder decoder("/sdcard/devel/2K/lexicon.bin", "/sdcard/devel/2K/lm.arpa");
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "start decoder");
        unsigned long decoderStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        for(int i = 0; i < segments; i++){
            for(int j = 0; j < nnResults[i]->getHeight(); j++)
                decoder.decode(nnResults[i]->getFeaturesMatrix()[j]);
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "%s", decoder.getWinner().c_str());
            decoder.reset();
        }
        unsigned long decoderEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
*/

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

    JNIEXPORT jstring JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_SpeechRecognitionAPI_testNative
            (JNIEnv* env, jobject obj, jstring str){

        return env->NewStringUTF(test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/devel/test.wav").c_str());
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test2.wav");
       // test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test3.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test4.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test5.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test6.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test7.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test8.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test9.wav");
        //test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test10.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test1.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test2.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test3.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test4.wav");
        //speechRecognitionAPI->recognizeWav("/sdcard/Audio/test5.wav");
        /*test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test1.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test2.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test3.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test4.wav");
        test(const_cast<char *>(env->GetStringUTFChars(str, NULL)), "/sdcard/Audio/test5.wav");*/
    }
}
