//
// Created by Petr Flajsingr on 07/04/2018.
//

#include <AudioFrame.h>
#include <AudioSubsampler.h>
#include <MelBankThread.h>
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

/**
 * Method to be run in thread. Buffers audio sent from recoder.
 */
void MelBankThread::threadMelBank() {
    AudioFrame::calcHammingCoef();
    Q_AudioData* data;
    AudioFrame frame;
    kiss_fft_cpx *fftFrame;

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);


    short* newAudioData = new short[SUBSAMPLED_OVERLAP_LENGTH * 3];

    AudioSubsampler subsampler;

    short dataCount = 0;
    while(inputQueue.dequeue(data)){
        if(data->type == TERMINATE){
            nnQueue->enqueue(new Q_MelData{TERMINATE, NULL});
            delete data;
            break;
        }
        short* subsampledAudio = subsampler.sample(data->data, SMALL_RECORDER_FRAMES);

        if(dataCount < 3){
            std::copy(subsampledAudio, subsampledAudio + SUBSAMPLED_OVERLAP_LENGTH,
                      newAudioData + dataCount * SUBSAMPLED_OVERLAP_LENGTH);
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
            melFilterBank->normalise(result);
            nnQueue->enqueue(new Q_MelData{SEQUENCE_DATA, result});
            callbacks->notifyVADChanged(true);
        } else {
            nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
            callbacks->notifyVADChanged(false);
            dataCount = 0;
        }
    }
    delete[] newAudioData;
    callbacks->DetachJava();
}

void MelBankThread::stopThread() {

}

/**
 * Moves data in memory to simulate frame creation
 */
void MelBankThread::prepareAudioData(short* data, short* newData) {
    for(int i = 0; i < SUBSAMPLED_OVERLAP_LENGTH * 2; i++){
        data[i] = data[i + SUBSAMPLED_OVERLAP_LENGTH];
    }
    int j = 0;
    for(int i = SUBSAMPLED_OVERLAP_LENGTH * 2; i < SUBSAMPLED_OVERLAP_LENGTH * 3; i++, j++){
        data[i] = newData[j];
    }
    return;
}
