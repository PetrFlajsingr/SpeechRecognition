//
// Created by Petr Flajsingr on 05/04/2018.
//

#ifndef VOICERECOGNITION_SPEECHRECOGNITIONAPI_H
#define VOICERECOGNITION_SPEECHRECOGNITIONAPI_H

#include <string>
#include <jni.h>
#include <RawAudioRecorder.h>
#include <JavaCallbacks.h>
#include <MelBankThread.h>
#include <NNThread.h>
#include <DecoderThread.h>
#include <FileStreamThread.h>


namespace SpeechRecognition {

    using namespace Threads;
    using namespace Utility;

    class SpeechRecognitionAPI {
    private:
        bool recorderCreated = false;

        const char* cacheDir;

        bool recording = false;

        RawAudioRecorder recorder;

        void startRecognitionThreads();

        ViterbiDecoder* decoder;
    public:
        SpeechRecognitionAPI(const char* cacheDir) : cacheDir(cacheDir) {
            decoder = new ViterbiDecoder("/sdcard/devel/1.5Kbig/lexicon.bin", "/sdcard/devel/1.5Kbig/lm.arpa");
        }

        virtual ~SpeechRecognitionAPI();

        bool startRecording();

        void stopRecording();

        std::string recognizeWav(std::string filePath);

        bool isRecording(){
            return recording;
        }

        JavaCallbacks callbacks;
    };
}


#endif //VOICERECOGNITION_SPEECHRECOGNITIONAPI_H
