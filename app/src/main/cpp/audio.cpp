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

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, APPNAME, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, APPNAME, __VA_ARGS__))
#define LOGD(...) \
  ((void)__android_log_print(ANDROID_LOG_DEBUG, APPNAME, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, APPNAME, __VA_ARGS__))

static JavaVM *g_VM;

JavaCallbacks callbacks;

std::vector<T_registeredObject> callbackObjects;

void notifyVADChanged(bool activity);

void notifySequenceRecognized(std::string sequence);

void notifyRecognitionDone();


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

    recorder->melQueue = &melBankThread.inputQueue;
    melBankThread.nnQueue = &nnThread.inputQueue;
    nnThread.decoderQueue = &decoderThread.inputQueue;

    recorder->startRecording();

    decoderThread.thread.join();
    JavaCallbacks::DetachJava();
}

// set the recording state for the audio recorder
void startRecording()
{
   std::thread thread(threads);
    thread.detach();
}

//\ threads

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

    //RSNeuralNetworkOld RSNN("/sdcard/voicerecognition/nn.bin", cacheDir);
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


FeatureMatrix* melFromTestFile(){
    const int FRAME_SIZE = (const int) (TARGET_SAMPLING_RATE * 0.025);
    const int FRAME_OVERLEAP = (const int) (TARGET_SAMPLING_RATE * 0.010);
    int recordingSize = 0;

    short* data = readAudioFromFile("/sdcard/aacheck/audio.raw", &recordingSize);

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

    FeatureMatrix* rsMelBankResults = new FeatureMatrix();
    RSMelFilterBank *rsMelBank = new RSMelFilterBank(cacheDir);
    rsMelBankResults->init(frameCount, MEL_BANK_FRAME_LENGTH);
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank start");

    for(int i = 0; i < frameCount; ++i) {
        rsMelBankResults->getFeaturesMatrix()[i] = rsMelBank->calculateMelBank(fftFrames[i]);
    }
    rsMelBank->substractMean(rsMelBankResults);

    dumpToFile("/sdcard/aacheck/rsMelBankResults.txt", rsMelBankResults->getFeaturesMatrix(),
               rsMelBankResults->getFramesNum(), rsMelBankResults->getFrameSize());
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank end");

   return rsMelBankResults;
}

void VADtest(){
    FeatureMatrix* melResult = melFromTestFile();

    VoiceActivityDetector detector;

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "VAD start");
    std::vector<bool> VADResult;
    for(int i = 0; i < melResult->getFramesNum(); i++){
        detector.checkData(melResult->getFeaturesMatrix()[i]);
        VADResult.push_back(detector.isActive());
    }
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "VAD end");


    RSNeuralNetwork nn("/sdcard/NNnew.bin", cacheDir);

    FeatureMatrix* nnResult = nn.forwardAll(melResult);

    Decoder decoder("/sdcard/lexicon.txt", "/sdcard/LM.arpa");

    bool active = false;

    for(int i = 0; i < nnResult->getFramesNum(); i++){
        if(!active && VADResult[i] == true){
            active = true;
            notifyVADChanged(active);
        } else if(active && VADResult[i] == false){
            decoder.decode(nnResult->getFeaturesMatrix()[i]);
            std::string winnerWord = decoder.getWinner();
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RESULT: %s", winnerWord.c_str());
            notifySequenceRecognized(winnerWord);
            active = false;
            notifyVADChanged(active);
            decoder.reset();
        }

        if(active){
            decoder.decode(nnResult->getFeaturesMatrix()[i]);
        }
    }

    delete melResult;

    notifyRecognitionDone();

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "VADtest() ended");
}

JNIEnv* AttachJava(bool* attached) {
    JavaVMAttachArgs args = {JNI_VERSION_1_2, 0, 0};
    JNIEnv* java;
    if(g_VM->AttachCurrentThread(&java, &args) == JNI_FALSE)
        *attached = false;
    else
        *attached = true;
    return java;
}

void DetachJava(){
    g_VM->DetachCurrentThread();
}

void notifyVADChanged(bool activity){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    jclass tmpClazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
    tmpClazz = (jclass)env->NewGlobalRef(tmpClazz);
    for(auto iterator = callbackObjects.begin();
            iterator != callbackObjects.end();
            iterator++){
        jmethodID methodID = env->GetMethodID(tmpClazz, "VADChanged", "(Z)V");
        iterator->obj = env->NewGlobalRef(iterator->obj);
        env->CallVoidMethod(iterator->obj, methodID, activity ? JNI_TRUE : JNI_FALSE);
    }
    if(attached)
        DetachJava();
}

void notifySequenceRecognized(std::string sequence){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    jclass tmpClazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
    tmpClazz = (jclass)env->NewGlobalRef(tmpClazz);
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(tmpClazz, "sequenceRecognized", "(Ljava/lang/String;)V");
        env->CallVoidMethod(iterator->obj, methodID, env->NewStringUTF(sequence.c_str()));
    }
    if(attached)
        DetachJava();
}

void notifyRecognitionDone(){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    jclass tmpClazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
    tmpClazz = (jclass)env->NewGlobalRef(tmpClazz);
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(tmpClazz, "recognitionDone", "()V");
        env->CallVoidMethod(iterator->obj, methodID);
    }
    if(attached)
        DetachJava();
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
        VADtest();
    }
}
