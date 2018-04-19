//
// Created by Petr Flajsingr on 05/04/2018.
//

#include "SpeechRecognitionAPI.h"

bool SpeechRecognition::SpeechRecognitionAPI::startRecording() {
    if(!recorderCreated){
        if(!recorder.createAudioRecorder())
            return false;

        recorderCreated = true;
    }
    recording = true;

    std::thread thread(&SpeechRecognitionAPI::startRecognitionThreads, this);

    thread.detach();

    return true;
}

void SpeechRecognition::SpeechRecognitionAPI::stopRecording() {
    recorder.stopRecording();
    recording = false;
}

std::string SpeechRecognition::SpeechRecognitionAPI::recognizeWav(std::string filePath) {
    std::ifstream filestream;
    filestream.open(filePath);
    if(filestream.is_open()) {
        FileStreamThread fileStreamThread(filestream);
        MelBankThread melBankThread(cacheDir, callbacks, false);
        NNThread nnThread(cacheDir);
        DecoderThread decoderThread(callbacks, decoder);

        nnThread.callbacks = &callbacks;

        fileStreamThread.melQueue = &melBankThread.inputQueue;
        melBankThread.nnQueue = &nnThread.inputQueue;
        nnThread.decoderQueue = &decoderThread.inputQueue;

        fileStreamThread.start();

        fileStreamThread.thread.join();
        melBankThread.thread.join();
        nnThread.thread.join();
        decoderThread.thread.join();

        return decoderThread.getResult();
    }
    return "";
}

void SpeechRecognition::SpeechRecognitionAPI::startRecognitionThreads() {
    MelBankThread melBankThread(cacheDir, callbacks, true);
    NNThread nnThread(cacheDir);
    DecoderThread decoderThread(callbacks, decoder);

    nnThread.callbacks = &callbacks;

    recorder.melQueue = &melBankThread.inputQueue;
    melBankThread.nnQueue = &nnThread.inputQueue;
    nnThread.decoderQueue = &decoderThread.inputQueue;

    recorder.startRecording();

    melBankThread.thread.join();
    nnThread.thread.join();
    decoderThread.thread.join();

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "THREADS END");
}

SpeechRecognition::SpeechRecognitionAPI::~SpeechRecognitionAPI() {
    delete decoder;
}
