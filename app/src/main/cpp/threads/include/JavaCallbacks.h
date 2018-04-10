//
// Created by Petr Flajsingr on 08/04/2018.
//

#ifndef VOICERECOGNITION_JAVACALLBACKS_H
#define VOICERECOGNITION_JAVACALLBACKS_H


#include <jni.h>
#include <string>
#include <vector>
namespace SpeechRecognition::Threads {
    typedef struct registeredObject {
        jobject obj;
        jclass clazz;
    } T_registeredObject;

    class JavaCallbacks {
    private:
        std::vector<T_registeredObject> callbackObjects;

        static JavaVM *g_VM;
    public:
        static JNIEnv *AttachJava(bool *attached);

        static void DetachJava();

        void notifyVADChanged(bool activity);

        void notifySequenceRecognized(std::string sequence);

        void notifyRecognitionDone();

        void notifyMelDone(int percentage);

        void notifyNNDone(int percentage);

        void notifyDecoderDone(int percentage);

        void addListener(T_registeredObject obj);

        void setJavaVM(JavaVM *vm);
    };
}

#endif //VOICERECOGNITION_JAVACALLBACKS_H
