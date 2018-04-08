//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <AudioFrame.h>
#include <AudioSubsampler.h>
#include <MelBankThread.h>
#include <android/log.h>
#include <constants.h>
#include <Utils.h>
#include <fstream>
#include <JavaCallbacks.h>

MelBankThread::MelBankThread(const char* cacheDir, JavaCallbacks& callbacks): thread(&MelBankThread::threadMelBank, this){
    this->melFilterBank = new RSMelFilterBank(cacheDir);

    this->VADetector = new VoiceActivityDetector();
    this->callbacks = &callbacks;
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

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);


    short* newAudioData = new short[240];

    AudioSubsampler subsampler;

    short dataCount = 0;
    while(inputQueue.dequeue(data)){
        short* subsampledAudio = subsampler.sample(data->data, SMALL_RECORDER_FRAMES);

        if(dataCount < 3){
            std::copy(subsampledAudio, subsampledAudio + 80,
                      newAudioData + dataCount * 80);
            dataCount++;
            continue;
        }

        prepareAudioData(newAudioData, subsampledAudio);
        delete[] subsampledAudio;

        frame.applyHammingWindow(newAudioData);
        delete[] data->data;
        delete data;

        frame.applyFFT(&cfg);

        fftFrame = frame.getFftData();
        float* result = this->melFilterBank->calculateMelBank(fftFrame);

        delete[] fftFrame;

        VADetector->checkData(result);

        if(VADetector->isActive()){
            nnQueue->enqueue(new Q_MelData{SEQUENCE_DATA, result});
            callbacks->notifyVADChanged(true);
            //TODO callback VADchanged(true)
        } else {
            nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
            callbacks->notifyVADChanged(false);
            //TODO callback VADchanged(false)
        }
    }
}

void MelBankThread::stopThread() {

}

void MelBankThread::prepareAudioData(short* data, short* newData) {
    for(int i = 0; i < 80 * 2; i++){
        data[i] = data[i + 80];
    }
    int j = 0;
    for(int i = 80 * 2; i < 80 * 3; i++, j++){
        data[i] = newData[j];
    }
    return;
}
