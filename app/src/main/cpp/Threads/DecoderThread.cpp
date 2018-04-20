//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <DecoderThread.h>
#include <constants.h>
#include <android/log.h>

SpeechRecognition::Threads::DecoderThread::DecoderThread(JavaCallbacks& callbacks, ViterbiDecoder* decoder): thread(&DecoderThread::threadDecoder, this),
decoder(decoder){
    //this->decoder = new ViterbiDecoder("/sdcard/big/lexicon.bin", "/sdcard/big/lm.arpa");

    this->callbacks = &callbacks;
}

SpeechRecognition::Threads::DecoderThread::~DecoderThread() {
}

/**
 * Method to be run in thread. Decodes data given by neural network thread and notifies listeners.
 */
void SpeechRecognition::Threads::DecoderThread::threadDecoder(){
    recognitionResult.clear();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: START");
    Q_NNData* data;
    std::string result = "";
    bool first = true;

    unsigned long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    unsigned long totalTime = 0;
    unsigned long runTime = 0;

    unsigned long counter = 0;

    while(inputQueue.dequeue(data)){
        //unsigned long sTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if(data->type == TERMINATE){
            delete data;
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: TERMINATE");
            break;
        }else if(data->type == SEQUENCE_DATA){
            if(first){
                first = false;
            }
            decoder->decode(data->data);
            delete[] data->data;
        } else{
            first = true;
            result = decoder->getWinner();
            recognitionResult.append(result);
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "RESULT: %s", result.c_str());
            callbacks->notifySequenceRecognized(result);
            decoder->reset();
        }
        delete data;

        //unsigned long nTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        //totalTime = nTime - startTime;
        //runTime += nTime - sTime;
        //counter++;
        //if(counter % 100 == 0)
        //    callbacks->notifyDecoderDone(runTime/(double)totalTime*100);
    }

    callbacks->notifyRecognitionDone();

    callbacks->DetachJava();
    unsigned long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "DECODER: END, TIME: %lu",  endTime - startTime);
}

std::string SpeechRecognition::Threads::DecoderThread::getResult() {
    return recognitionResult;
}
