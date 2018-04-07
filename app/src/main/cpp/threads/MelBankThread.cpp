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

MelBankThread::MelBankThread(const char* cacheDir): thread(&MelBankThread::threadMelBank, this){
    this->melFilterBank = new RSMelFilterBank(cacheDir);

    this->VADetector = new VoiceActivityDetector();
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

    //short audioData[SMALL_RECORDER_FRAMES*3] = {0};
    //short audioData[3][SMALL_RECORDER_FRAMES] = {0};

    short* audioData = new short[3 * SMALL_RECORDER_FRAMES];

    std::vector<short> audioToSave;
    int myCounter = 0;

    short dataCount = 0;
    bool notSaved = true;
    while(run){
        inputQueue.dequeue(data);
        //__android_log_print(ANDROID_LOG_DEBUG, APPNAME, "mel got from queue");
        if(dataCount < 3){
            std::copy(data->data, data->data + SMALL_RECORDER_FRAMES,
                      audioData + dataCount * SMALL_RECORDER_FRAMES);
            dataCount++;
            continue;
        }

        prepareAudioData(audioData, data->data);

        short* subsampledAudio = AudioSubsampler::subsample48kHzto8kHz(audioData,
                                                           ORIG_FRAME_LENGTH);
        myCounter++;
        if(myCounter < 1000){
            for(int i = 0; i < 80; i++){
                audioToSave.push_back(subsampledAudio[i]);
            }
        } else if(notSaved){
            notSaved = false;
            std::ofstream out;
            out.open("/sdcard/AAAA_NEWAUDIO.raw", std::ios::out | std::ios::binary);
            out.write((char*)audioToSave.data(), 1000*80*2);
            __android_log_print(ANDROID_LOG_DEBUG, APPNAME, "saved!!!!");
        }

        frame.applyHammingWindow(subsampledAudio);
        delete[] subsampledAudio;
        delete[] data->data;
        delete data;

        frame.applyFFT(&cfg);

        fftFrame = frame.getFftData();
        float* result = this->melFilterBank->calculateMelBank(fftFrame);

        delete[] fftFrame;

        VADetector->checkData(result);

        if(VADetector->isActive()){
            nnQueue->enqueue(new Q_MelData{SEQUENCE_DATA, result});
            //TODO callback VADchanged(true)
        } else {
            nnQueue->enqueue(new Q_MelData{SEQUENCE_INACTIVE, NULL});
            //TODO callback VADchanged(false)
        }
    }
}

void MelBankThread::stopThread() {
    run = false;
}

void MelBankThread::prepareAudioData(short* data, short* newData) {
    for(int i = 0; i < SMALL_RECORDER_FRAMES * 2; i++){
        data[i] = data[i + SMALL_RECORDER_FRAMES];
    }
    int j = 0;
    for(int i = SMALL_RECORDER_FRAMES * 2; i < SMALL_RECORDER_FRAMES * 3; i++, j++){
        data[i] = newData[j];
    }
    return;
    std::copy(data + SMALL_RECORDER_FRAMES, data + SMALL_RECORDER_FRAMES * 3, data);
    std::copy(newData, newData + SMALL_RECORDER_FRAMES, data + 2 * SMALL_RECORDER_FRAMES);
}
