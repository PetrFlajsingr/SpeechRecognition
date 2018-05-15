//
// Created by Petr Flajsingr on 05/04/2018.
//

#include <Utils.h>
#include <SpeechRecognitionAPI.h>
#include <Exceptions.h>

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
    std::ifstream fileStream;
    fileStream.open(filePath);
    if(fileStream.is_open()) {
        FileStreamThread fileStreamThread(fileStream);
        MelBankThread melBankThread(callbacks, melFilterBank, false);
        NNThread nnThread(neuralNetwork);
        DecoderThread decoderThread(callbacks, decoder);

        decoderThread.setSequenceDone(false);

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
    MelBankThread melBankThread(callbacks, melFilterBank, true);
    NNThread nnThread(neuralNetwork);
    DecoderThread decoderThread(callbacks, decoder);

    nnThread.callbacks = &callbacks;

    recorder.melQueue = &melBankThread.inputQueue;
    melBankThread.nnQueue = &nnThread.inputQueue;
    nnThread.decoderQueue = &decoderThread.inputQueue;

    recorder.startRecording();

    melBankThread.thread.join();
    nnThread.thread.join();
    decoderThread.thread.join();
}

SpeechRecognition::SpeechRecognitionAPI::~SpeechRecognitionAPI() {
    delete decoder;
    delete neuralNetwork;
    delete melFilterBank;
}

void SpeechRecognition::SpeechRecognitionAPI::checkNecessaryFiles() {
    if(!doesFileExist(LM_PATH.c_str()))
        throw Exceptions::ASRFilesMissingException("Language model is not on device. Path: " + LM_PATH);
    if(!doesFileExist(LEXICON_PATH.c_str()))
        throw Exceptions::ASRFilesMissingException("Lexicon is not on device. Path: " + LEXICON_PATH);
    if(!doesFileExist(NN_PATH.c_str()))
        throw Exceptions::ASRFilesMissingException("Neural network is not on device. Path: " + NN_PATH);
}
