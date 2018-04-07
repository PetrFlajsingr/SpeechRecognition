//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <DecoderThread.h>

DecoderThread::DecoderThread(): thread(&DecoderThread::threadDecoder, this)  {
    this->decoder = new Decoder("/sdcard/lexicon.txt", "/sdcard/LM.arpa");
}

DecoderThread::~DecoderThread() {
    delete decoder;
}

void DecoderThread::threadDecoder(){
    Q_NNData* data;

    std::string result;
    while(run){
        inputQueue.dequeue(data);

        if(data->type == SEQUENCE_DATA){
            decoder->decode(data->data, false);
        } else{
            result = decoder->getWinner();
            //TODO callback sequenceDone
        }
    }
    //TODO callback recognitionDone
}
