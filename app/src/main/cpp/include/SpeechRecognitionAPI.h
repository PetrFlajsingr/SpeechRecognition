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

    /**
     * @brief Class providing simple programming interface to use speech recognizer
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class SpeechRecognitionAPI {
    private:
        bool recorderCreated = false;

        const char* cacheDir;

        bool recording = false;

        RawAudioRecorder recorder;

        void startRecognitionThreads();

        ViterbiDecoder* decoder;

        RSMelFilterBank* melFilterBank;

        RSNeuralNetwork* neuralNetwork;

        void checkNecessaryFiles();
    public:
        /**
         * Allocates decoder.
         * @param cacheDir applications cache directory for Renderscript
         */
        SpeechRecognitionAPI(const char* cacheDir) : cacheDir(cacheDir) {
            checkNecessaryFiles();
            decoder = new ViterbiDecoder(LEXICON_PATH, LM_PATH);

            melFilterBank = new RSMelFilterBank(cacheDir);

            neuralNetwork = new RSNeuralNetwork(NN_PATH, cacheDir);
        }

        virtual ~SpeechRecognitionAPI();

        bool startRecording();

        void stopRecording();

        /**
         * Recognizes wav provided via filepath
         * @param filePath path to file
         * @return recognized sequence
         */
        std::string recognizeWav(std::string filePath);

        bool isRecording(){
            return recording;
        }

        JavaCallbacks callbacks;
    };
}


#endif //VOICERECOGNITION_SPEECHRECOGNITIONAPI_H
