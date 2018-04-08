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

void DecoderThread::threadDecoder(){
    Q_NNData* data;
    std::string result;
    bool first = true;
    while(inputQueue.dequeue(data)){
        if(data->type == SEQUENCE_DATA){
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
            //TODO callback sequenceDone
            callbacks->notifySequenceRecognized(result);
        }
        delete data;
    }
    callbacks->notifyRecognitionDone();
    //TODO callback recognitionDone
}
