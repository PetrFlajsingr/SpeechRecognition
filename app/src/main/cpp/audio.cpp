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

#include "RSMelFilterBank.h"

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



    /*Element::Builder* elBuilderin = new Element::Builder(rs);
    sp<const Element> floatElement = Element::F32(rs);
    elBuilderin->add(floatElement, "inputFrame", FFT_FRAME_LENGTH/2);
    sp<const Type> melInputType = Type::create(rs, elBuilderin->create(), frameCount, 0, 0);
    sp<Allocation> melInputAlloc = Allocation::createTyped(rs, melInputType);

    Element::Builder* elBuilderOut = new Element::Builder(rs);
    elBuilderOut->add(floatElement, "outputFrame", MEL_BANK_FRAME_LENGTH);
    elBuilderOut->add(floatElement, "")
    sp<const Type> melOutputType = Type::create(rs, elBuilderOut->create(), frameCount, 0, 0);

    sp<Allocation> melOutputAlloc = Allocation::createTyped(rs, melOutputType);*/


    //sc->forEach_root(melInputAlloc, melOutputAlloc);

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

    /*sp<RS> rs = new RS();
    rs->init(cacheDir);

    ScriptC_RSmelfilterbank* sc = new ScriptC_RSmelfilterbank(rs);


    Element::Builder* elBuilder = new Element::Builder(rs);
    elBuilder->add(Element::F32(rs), "", FFT_FRAME_LENGTH);
    sp<const Type> fftFrameType = Type::create(rs, elBuilder->create(), 1, 0, 0);
    sp<Allocation> fftFrameAllocation = Allocation::createTyped(rs, fftFrameType);

    sp<Allocation> melIterationAllocation = Allocation::createSized(rs, Element::U32(rs), MEL_BANK_FRAME_LENGTH);

    uint32_t iter[MEL_BANK_FRAME_LENGTH];
    for(uint32_t i = 0; i < MEL_BANK_FRAME_LENGTH; ++i)
        iter[i] = i;*/

    //melIterationAllocation->copy1DFrom(iter);


    //sp<Allocation> melBankAllocation = Allocation::createSized(rs, Element::F32(rs), MEL_BANK_FRAME_LENGTH);

    kiss_fft_cpx** fftFrames = new kiss_fft_cpx*[frameCount];
    /*FeaturesMatrixFloat rsMelMatrix;
    rsMelMatrix.init(frameCount, MEL_BANK_FRAME_LENGTH);*/

    for(unsigned int i = 0; i < frameCount; ++i){
        frames[i].applyFFT(&cfg);
        //fftFrameAllocation->copy1DFrom(frames[i].getFftData());

        //sc->set_fftFrame(fftFrameAllocation);

        //sc->forEach_melBank(melIterationAllocation, melBankAllocation);

        //rs->finish();

        //melBankAllocation->copy1DTo(rsMelMatrix.getFeaturesMatrix()[i]);
        fftFrames[i] = frames[i].getFftData();
    }
    free(cfg);

    /*sc->set_frameCount(frameCount);

    sp<Allocation> melCalcFramesAllocation = Allocation::createSized(rs, Element::F32(rs), frameCount * MEL_BANK_FRAME_LENGTH);

    for(int i = 0; i < rsMelMatrix.getFramesNum(); i++)
        melCalcFramesAllocation->copy1DRangeFrom(i * rsMelMatrix.getFrameSize(),
                                                 rsMelMatrix.getFrameSize(),
                                                 rsMelMatrix.getFeaturesMatrix()[i]);
    //melCalcFramesAllocation->copy1DFrom(rsMelMatrix.getFeaturesMatrix()[0]);

    sc->set_melCalculatedFrames(melCalcFramesAllocation);

    //sc->invoke_checkAllocation();

    rs->finish();

    sc->forEach_substractMean(melIterationAllocation);

    rs->finish();

    for(int i = 0; i < rsMelMatrix.getFramesNum(); i++)
        melCalcFramesAllocation->copy1DRangeTo(i * rsMelMatrix.getFrameSize(),
                                                 rsMelMatrix.getFrameSize(),
                                                 rsMelMatrix.getFeaturesMatrix()[i]);

    rsMelMatrix.dumpResultToFile("/sdcard/AAAAAAwat.txt");*/

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "fft end");

    for(int i = 0; i < 10; ++i) {
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank start");
        RSMelFilterBank *rsMelBank = new RSMelFilterBank(cacheDir);
        FeaturesMatrixFloat rsMelBankResults;
        rsMelBankResults.init(frameCount, MEL_BANK_FRAME_LENGTH);
        for(unsigned int i = 0; i < frameCount; ++i) {
            rsMelBankResults.getFeaturesMatrix()[i] = rsMelBank->calculateMelBank(fftFrames[i]);
        }
        rsMelBank->substractMean(&rsMelBankResults);

        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RS mel bank end");
        //rsMelBankResults.dumpResultToFile("/sdcard/AAAmelbankRS.txt");

        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "mel bank start");
        MelFilterBank::initStatic();

        MelFilterBank *melBank = new MelFilterBank();

        melBank->calculateMelBanks(frameCount, fftFrames);
        __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "mel bank end");
    }

    //melBank->dumpResultToFile("/sdcard/AAAmelbank.txt");

    return;

    /*__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN load");
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
    MelFilterBank::deleteStatic();*/
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
