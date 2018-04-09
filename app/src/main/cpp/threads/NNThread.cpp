//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <NNThread.h>
#include <constants.h>
#include <JavaCallbacks.h>
#include <android/log.h>

NNThread::NNThread(const char* cacheDir): thread(&NNThread::threadNN, this) {
    neuralNetwork = new RSNeuralNetwork("/sdcard/NNnew.bin", cacheDir);
}

NNThread::~NNThread() {
    delete neuralNetwork;
}

/**
 * Function to be run in a thread. Takes data from inputQueue.
 */
void NNThread::threadNN() {
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN: START");
    Q_MelData* data;

    float nnInput[NN_INPUT_SIZE];
    std::vector<float*> buffer;

    float* result;
    bool active = false;

    unsigned long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    unsigned long totalTime = 0;
    unsigned long runTime = 0;


    while(inputQueue.dequeue(data)){
        unsigned long sTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(data->type == TERMINATE){
            decoderQueue->enqueue(new Q_NNData{TERMINATE, NULL});
            delete data;
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN: TERMINATE");
            break;
        }else if(data->type == SEQUENCE_DATA){
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

        unsigned long nTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        totalTime = nTime - startTime;
        runTime += nTime - sTime;
        callbacks->notifyNNDone(runTime/(double)totalTime*100);
    }

    JavaCallbacks::DetachJava();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "NN: END");
}

/**
 * Deletion of frame buffer. Called on finishing sequence.
 */
void NNThread::deleteBuffer(std::vector<float *> &data) {
    for(auto iterator = data.begin();
            iterator != data.end();){
        delete[] *iterator;
        data.erase(iterator);
    }
}

/**
 * Prepares the input for neural network
 */
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

