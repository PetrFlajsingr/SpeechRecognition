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
#include <android/log.h>

SpeechRecognition::Threads::MelBankThread::MelBankThread(const char* cacheDir, JavaCallbacks& callbacks, bool subsample): thread(&MelBankThread::threadMelBank, this){
    this->melFilterBank = new RSMelFilterBank(cacheDir);

    this->VADetector = new VoiceActivityDetector();
    this->callbacks = &callbacks;

    this->subsample = subsample;
}

SpeechRecognition::Threads::MelBankThread::~MelBankThread() {
    delete this->melFilterBank;
    delete this->VADetector;
}

/**
 * Method to be run in thread. Buffers audio sent from recoder.
 */
void SpeechRecognition::Threads::MelBankThread::threadMelBank() {
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "MEL: START");
    AudioFrame::calcHammingCoef();
    Q_AudioData* data;
    AudioFrame frame;
    kiss_fft_cpx *fftFrame;

    kiss_fftr_cfg cfg = kiss_fftr_alloc(FFT_FRAME_LENGTH, 0, NULL, NULL);


    short* newAudioData = new short[SUBSAMPLED_OVERLAP_LENGTH * 3];

    AudioSubsampler subsampler;

    unsigned long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    unsigned long totalTime = 0;
    unsigned long runTime = 0;

    short dataCount = 0;

    unsigned long counter = 0;

    bool notified = false;
    short* subsampledAudio;
    while(inputQueue.dequeue(data)){
        unsigned long sTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(data->type == TERMINATE){
            if(VADetector->isActive())
                nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
            nnQueue->enqueue(new Q_MelData{TERMINATE, NULL});
            delete data;
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "MEL: TERMINATE");
            break;
        }
        if(subsample)
            subsampledAudio = subsampler.sample(data->data, SMALL_RECORDER_FRAMES);
        else
            subsampledAudio = data->data;

        if(dataCount < 3){
            std::copy(subsampledAudio, subsampledAudio + SUBSAMPLED_OVERLAP_LENGTH,
                      newAudioData + dataCount * SUBSAMPLED_OVERLAP_LENGTH);
            dataCount++;
            if(subsample)
                delete[] subsampledAudio;
            delete[] data->data;
            delete data;
            continue;
        }

        prepareAudioData(newAudioData, subsampledAudio);
        if(subsample)
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
            for(auto iterator = VADetector->getBuffer().begin();
                    iterator != VADetector->getBuffer().end();){
                result = *iterator;
                melFilterBank->normalise(result);
                nnQueue->enqueue(new Q_MelData{SEQUENCE_DATA, result});
                VADetector->getBuffer().erase(iterator);
            }
            if(!notified) {
                if(subsample)
                    callbacks->notifyVADChanged(true);
                notified = true;
            }
        } else {
            if(notified) {
                notified = false;
                nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
                if(subsample)
                    callbacks->notifyVADChanged(false);
            }

            dataCount = 0;
        }

        unsigned long nTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        totalTime = nTime - startTime;
        runTime += nTime - sTime;
        counter++;
        if(counter % 500 == 0)
            callbacks->notifyMelDone(runTime/(double)totalTime*100);
    }
    delete[] newAudioData;
    callbacks->DetachJava();
    __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "MEL: END");
}

void SpeechRecognition::Threads::MelBankThread::stopThread() {

}

/**
 * Moves data in memory to simulate frame creation
 */
void SpeechRecognition::Threads::MelBankThread::prepareAudioData(short* data, short* newData) {
    for(int i = 0; i < SUBSAMPLED_OVERLAP_LENGTH * 2; i++){
        data[i] = data[i + SUBSAMPLED_OVERLAP_LENGTH];
    }
    int j = 0;
    for(int i = SUBSAMPLED_OVERLAP_LENGTH * 2; i < SUBSAMPLED_OVERLAP_LENGTH * 3; i++, j++){
        data[i] = newData[j];
    }
    return;
}
