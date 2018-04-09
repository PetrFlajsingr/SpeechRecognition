//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <DecoderThread.h>
#include <constants.h>
#include <android/log.h>

DecoderThread::DecoderThread(JavaCallbacks& callbacks): thread(&DecoderThread::threadDecoder, this)  {
    this->decoder = new Decoder("/sdcard/lexicon.txt", "/sdcard/LM.arpa");

    this->callbacks = &callbacks;
}

DecoderThread::~DecoderThread() {
    delete decoder;
}

/**
 * Method to be run in thread. Decodes data given by neural network thread and notifies listeners.
 */
void DecoderThread::threadDecoder(){
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: START");
    Q_NNData* data;
    std::string result;
    bool first = true;
    while(inputQueue.dequeue(data)){
        if(data->type == TERMINATE){
            delete data;
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: TERMINATE");
            break;
        }else if(data->type == SEQUENCE_DATA){
            if(first){
                first = false;
            }
            decoder->decode(data->data);
            delete [] data->data;
        } else{
            first = true;
            result = decoder->getWinner();
            decoder->reset();
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RESULT: %s", result.c_str());
            callbacks->notifySequenceRecognized(result);
        }
        delete data;
    }
    callbacks->notifyRecognitionDone();

    callbacks->DetachJava();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: END");
}
