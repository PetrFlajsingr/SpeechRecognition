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
#include "Voice_Activity_Detection/VoiceActivityDetector.h"
#include <unistd.h>

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, APPNAME, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, APPNAME, __VA_ARGS__))
#define LOGD(...) \
  ((void)__android_log_print(ANDOIRD_LOG_DEBUG, APPNAME, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, APPNAME, __VA_ARGS__))



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

std::string mapResultToString(float* array){
    float max = 0;
    int maxIndex = 0;
    for(int i = 0; i < 46;i++){
        if(array[i] > max){
            max = array[i];
            maxIndex = i;
        }
    }
    //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "max: %g", max);
    switch(maxIndex){
        case 0: return "AA";
        case 1: return "AE";
        case 2: return "AH";
        case 3: return "AO";
        case 4: return "AW";
        case 5: return "AY";
        case 6: return "B";
        case 7: return "BRH";
        case 8: return "CGH";
        case 9: return "CH";
        case 10: return "D";
        case 11: return "DH";
        case 12: return "EH";
        case 13: return "ER";
        case 14: return "EY";
        case 15: return "F";
        case 16: return "G";
        case 17: return "HH";
        case 18: return "IH";
        case 19: return "IY";
        case 20: return "JH";
        case 21: return "K";
        case 22: return "L";
        case 23: return "M";
        case 24: return "N";
        case 25: return "NG";
        case 26: return "NSN";
        case 27: return "OW";
        case 28: return "OY";
        case 29: return "P";
        case 30: return "R";
        case 31: return "S";
        case 32: return "SH";
        case 33: return "SIL";
        case 34: return "SMK";
        case 35: return "T";
        case 36: return "TH";
        case 37: return "UH";
        case 38: return "UHH";
        case 39: return "UM";
        case 40: return "UW";
        case 41: return "V";
        case 42: return "W";
        case 43: return "Y";
        case 44: return "Z";
        case 45: return "ZH";
        default: return "ERROR";
    }
}

void nntest(){
    const int FRAME_SIZE = (const int) (TARGET_SAMPLING_RATE * 0.025);
    const int FRAME_OVERLEAP = (const int) (TARGET_SAMPLING_RATE * 0.010);
    int recordingSize = 0;

    short* data = readAudioFromFile("/sdcard/AAA_ahojtest.raw", &recordingSize);

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

    RSNeuralNetwork nn("/sdcard/NNnew.bin", cacheDir);

    FeatureMatrix* result = nn.forwardAll(&rsMelBankResults);

    for(int i = 0;i < frameCount; i++){
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "result: %s", mapResultToString(result->getFeaturesMatrix()[i]).c_str());
    }

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

FeatureMatrix* nnFromTestFile(){
    const int FRAME_SIZE = (const int) (TARGET_SAMPLING_RATE * 0.025);
    const int FRAME_OVERLEAP = (const int) (TARGET_SAMPLING_RATE * 0.010);
    int recordingSize = 0;

    short* data = readAudioFromFile("/sdcard/AAA_numbertest.raw", &recordingSize);

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

    RSNeuralNetwork nn("/sdcard/NNnew.bin", cacheDir);

    return nn.forwardAll(&rsMelBankResults);
}

FeatureMatrix* melFromTestFile(){
    const int FRAME_SIZE = (const int) (TARGET_SAMPLING_RATE * 0.025);
    const int FRAME_OVERLEAP = (const int) (TARGET_SAMPLING_RATE * 0.010);
    int recordingSize = 0;

    short* data = readAudioFromFile("/sdcard/AAA_numbertest.raw", &recordingSize);

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
        } else if(active && VADResult[i] == false){
            decoder.decode(nnResult->getFeaturesMatrix()[i], true);
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RESULT: %s", decoder.getWinner().c_str());
            active = false;
            decoder.reset();
        }

        if(active){
            decoder.decode(nnResult->getFeaturesMatrix()[i], false);
        }
    }

    delete melResult;

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "VADtest() ended");
}

void AcousticTest(){
    FeatureMatrix* nnResult = nnFromTestFile();
    Decoder decoder("/sdcard/lexicon.txt", "/sdcard/LM.arpa");

    for(int i = 0; i < nnResult->getFramesNum(); i++) {
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "Decode %d", i);
        decoder.decode(nnResult->getFeaturesMatrix()[i], false);
    }

    std::string hahahaahahahaha = decoder.getOutput();

    std::ofstream out;
    out.open("/sdcard/AAA_decoder_result.txt");

    out.write(hahahaahahahaha.c_str(), hahahaahahahaha.length());
    out.close();

    delete nnResult;
}

extern "C"{

    JNIEXPORT jstring JNICALL Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_getJniString( JNIEnv* env, jobject obj){

        jstring jstr = env->NewStringUTF("This comes from jni.");
        jclass clazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/MainActivity");
        jmethodID messageMe = env->GetMethodID(clazz, "messageMe", "(Ljava/lang/String;)V");
        env->CallVoidMethod(obj, messageMe, jstr);

        return env->NewStringUTF("kokot");
    }

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
/*
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
         //createFrames();
        //nntest();

        //AcousticTest();

        //VADtest();

    }

    */
}
