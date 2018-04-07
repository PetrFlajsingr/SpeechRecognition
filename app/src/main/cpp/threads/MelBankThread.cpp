//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <AudioFrame.h>
#include <AudioSubsampler.h>
#include "MelBankThread.h"

MelBankThread::MelBankThread(const char* cacheDir): thread(&MelBankThread::threadMelBank, this){
    this->melFilterBank = new RSMelFilterBank(cacheDir);

    this->VADetector = new VoiceActivityDetector();

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);
}

MelBankThread::~MelBankThread() {
    delete this->melFilterBank;
    delete this->VADetector;
}

void MelBankThread::threadMelBank() {
    AudioFrame::calcHammingCoef();

    Q_AudioData* data;
    AudioFrame frame;
    kiss_fft_cpx *fftFrame;
    while(run){
        inputQueue.dequeue(data);
        data->data = AudioSubsampler::subsample48kHzto8kHz(data->data,
                                                           ORIG_FRAME_LENGTH);
        frame.applyHammingWindow(data->data);
        delete[] data;

        frame.applyFFT(&cfg);

        fftFrame = frame.getFftData();
        float* result = this->melFilterBank->calculateMelBank(fftFrame);

        delete[] fftFrame;

        VADetector->checkData(result);

        if(VADetector->isActive()){
            nnQueue->enqueue(new Q_MelData{SEQUENCE_DATA, result});
            //TODO callback VADchanged
        } else {
            nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
            //TODO callback VADchanged
        }
    }
}

void MelBankThread::stopThread() {
    run = false;
}
