//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <NNThread.h>

NNThread::NNThread(const char* cacheDir): thread(&NNThread::threadNN, this) {
    neuralNetwork = new RSNeuralNetwork("/sdcard/NNnew.bin", cacheDir);
}

NNThread::~NNThread() {
    delete neuralNetwork;
}


void NNThread::threadNN() {
    Q_MelData* data;

    float* result;
    while(run){
        inputQueue.dequeue(data);

        if(data->type == SEQUENCE_DATA){
            result = neuralNetwork->forward(data->data);
            delete[] data;

            decoderQueue->enqueue(new Q_NNData{SEQUENCE_DATA, result});
        } else{
            decoderQueue->enqueue(new Q_NNData{SEQUENCE_INACTIVE, NULL});
        }
    }
}

