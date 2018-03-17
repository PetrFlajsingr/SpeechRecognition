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
#include <RSNeuralNetworkOld.h>
#include <thread>
#include <condition_variable>

#include <chrono>
#include <AudioSubsampler.h>

#include "RSMelFilterBank.h"

using namespace android::RSC;

RawAudioRecorder* recorder;

short* readAudioFromFile(std::string filepath, int* recordingSize);

const char* cacheDir;

std::condition_variable cv;

short* audioData;

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
void startRecording()
{
    audioData = new short[MAX_AUDIO_LENGTH];
    recorder->setSharedAudioData(audioData);
    recorder->setCv(&cv);
    recorder->startRecording();
}

/**
 * Function called from a thread to calculate mel banks on data provided by recorder thread.
 * It is being controlled by a condition_variable unlocking the thread from recorder thread.
 */
void calculateMelbanksThread(){
    const int ORIG_FRAME_OVERLAP = SAMPLING_RATE * 0.010;
    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);

    AudioFrame::calcHammingCoef();

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL); //< configuration for kissfft


    FeatureMatrix melBankResults;
    melBankResults.init(MAX_FRAME_COUNT, MEL_BANK_FRAME_LENGTH);

    RSMelFilterBank *rsMelBank = new RSMelFilterBank(cacheDir);

    int dataStart = 0; //< offset for data provided by recorder thread

    int frameCounter = 0;

    // thread continues to calculate mel banks as long as the recording is active or there are
    // data to work through
    while(recorder->isRecording() || recorder->getDataCounter() - ORIG_FRAME_OVERLAP > dataStart){

        if(recorder->isRecording() && recorder->getDataCounter() < dataStart + ORIG_FRAME_LENGTH){
            cv.wait(lock);
            continue;
        }
        short *subsampledData = AudioSubsampler::subsample48kHzto8kHz(audioData + dataStart,
                                                                      ORIG_FRAME_LENGTH);

        if(frameCounter == 0){
            std::ofstream out;
            out.open("/sdcard/AAA_AUDIOTEST1.raw", std::ios::out | std::ios::binary);

            out.write((char*)subsampledData, 400);
            out.close();
        }

        AudioFrame frame;
        frame.applyHammingWindow(subsampledData);

        // deletion of not needed memory
        delete[] subsampledData;

        frame.applyFFT(&cfg);

        kiss_fft_cpx *fftFrame = frame.getFftData();

        // saving the data into matrix
        melBankResults.getFeaturesMatrix()[frameCounter] = rsMelBank->calculateMelBank(fftFrame);

        //increasing offset
        dataStart += ORIG_FRAME_OVERLAP;

        frameCounter++;
    }

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "test: %d", (recorder->getDataCounter() - ORIG_FRAME_LENGTH) / ORIG_FRAME_OVERLAP);

    melBankResults.setFramesNum(frameCounter);

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "recording done, total: %d, mine: %d, frames: %d", recorder->getDataCounter(), dataStart, frameCounter);

    rsMelBank->substractMean(&melBankResults);

    std::ofstream out;
    out.open("/sdcard/AAA_AUDIOTEST.raw", std::ios::out | std::ios::binary);

    out.write((char*)AudioSubsampler::subsample48kHzto8kHz(audioData, recorder->getDataCounter()), recorder->getDataCounter()/6*2);
    out.close();

    melBankResults.dumpResultToFile("/sdcard/AAA_MELBANKRESULT.TXT");


    free(cfg);
}

void threadTest(){
    startRecording();
    std::thread testThread(calculateMelbanksThread);
    testThread.join();
}

void createFrames(){
   // int a;
    //recorder->getRecording(&a);
    const int FRAME_SIZE = (const int) (8000 * 0.025);
    const int FRAME_OVERLEAP = (const int) (8000 * 0.010);
    int recordingSize = 0;
    //short* data = recorder->getRecording(&recordingSize);
    short* data = readAudioFromFile("/sdcard/AAA_AUDIOTEST.raw", &recordingSize);

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
        fftFrames[i] = frames[i].getFftData();
    }
    free(cfg);

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "fft end");

    FeatureMatrix rsMelBankResults;
    RSMelFilterBank *rsMelBank = new RSMelFilterBank(cacheDir);
    rsMelBankResults.init(frameCount, MEL_BANK_FRAME_LENGTH);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank start");

    for(int i = 0; i < frameCount; ++i) {
        rsMelBankResults.getFeaturesMatrix()[i] = rsMelBank->calculateMelBank(fftFrames[i]);
    }
    rsMelBank->substractMean(&rsMelBankResults);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank end");
    rsMelBankResults.dumpResultToFile("/sdcard/___RES.txt");

    //RSNeuralNetwork RSNN("/sdcard/voicerecognition/nn.bin", cacheDir);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS NN start");
    //FeatureMatrix* NNoutput = RSNN.forwardAll(&rsMelBankResults);

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS NN end");
    //NNoutput->dumpResultToFile("/sdcard/AAAAANNNNNN.txt");

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "frames2: %d", frameCount);

//
//    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN load");
//    NeuralNetwork* nn = new NeuralNetwork("/sdcard/voicerecognition/nn.bin");
//    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN load done");
//
//    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN forward");
//
//    auto start = std::chrono::steady_clock::now();
//    for(int i = 0; i < 50; ++i){
//        nn->setFeatureMatrix(&rsMelBankResults);
//        nn->forward();
//    }
//    auto end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> elapsed = end - start;
//    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN: %g", elapsed.count());
//
//    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN forward done");

    //delete nn;
    delete[] fftFrames;
    delete[] frames;
    //delete melBank;
    //MelFilterBank::deleteStatic();
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

JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_threadTest(JNIEnv* env, jclass clazz){
    threadTest();
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

    JNIEXPORT void JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_startRecording(JNIEnv* env, jclass clazz){
        startRecording();
    }

    JNIEXPORT void Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_shutdown(JNIEnv* env, jclass clazz){
        shutdown();
    }

    JNIEXPORT void Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_createFrames(JNIEnv* env, jclass clazz){
         createFrames();
    }
}
