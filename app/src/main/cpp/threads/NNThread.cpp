//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <NNThread.h>
#include <constants.h>
#include <android/log.h>

NNThread::NNThread(const char* cacheDir): thread(&NNThread::threadNN, this) {
    neuralNetwork = new RSNeuralNetwork("/sdcard/NNnew.bin", cacheDir);
}

NNThread::~NNThread() {
    delete neuralNetwork;
}


void NNThread::threadNN() {
    Q_MelData* data;

    float nnInput[NN_INPUT_SIZE];
    std::vector<float*> buffer;

    float* result;
    bool active = false;
    while(inputQueue.dequeue(data)){
        if(data->type == SEQUENCE_DATA){
            active = true;
            buffer.push_back(data->data);
            if(buffer.size() < 7){
                continue;
            }
            prepareInput(nnInput, buffer);

            result = neuralNetwork->forward(nnInput);
            decoderQueue->enqueue(new Q_NNData{SEQUENCE_DATA, result});
        } else if(active){
            active = false;
            decoderQueue->enqueue(new Q_NNData{SEQUENCE_INACTIVE, NULL});

            deleteBuffer(buffer);
        }
        delete data;
    }
}

void NNThread::deleteBuffer(std::vector<float *> &data) {
    for(auto iterator = data.begin();
            iterator != data.end();){
        delete[] *iterator;
        data.erase(iterator);
    }
}

void NNThread::prepareInput(float *result, std::vector<float *> &data) {
    const int ROLLING_WINDOW_SIZE = 7;
    const int OFFSET = ROLLING_WINDOW_SIZE * 2 + 1;
    int frameNum = 0;
    int index = data.size() - ROLLING_WINDOW_SIZE;
    for(int i = 0; i < OFFSET; i++){
        for(int j = 0; j < MEL_BANK_FRAME_LENGTH; j++){
            frameNum = index + i - ROLLING_WINDOW_SIZE;
            if(frameNum < 0){
                frameNum = 0;
            }
            if(frameNum > data.size() - 1){
                frameNum = data.size() - 1;
            }
            result[i + OFFSET * j] = data[frameNum][j];
        }
    }
    if(data.size() > 14){
        delete[] data.at(0);
        data.erase(data.begin());
    }
}

