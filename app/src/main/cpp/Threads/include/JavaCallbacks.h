//
// Created by Petr Flajsingr on 08/04/2018.
//

#ifndef VOICERECOGNITION_JAVACALLBACKS_H
#define VOICERECOGNITION_JAVACALLBACKS_H


#include <jni.h>
#include <string>
#include <vector>
namespace SpeechRecognition::Threads {
    /**
     * Java object registered as a listener
     */
    typedef struct RegisteredObject {
        jobject obj;
        jclass clazz;
    } T_registeredObject;

    /**
     * @brief Class forwarding callbacks from C++ to JVM
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class JavaCallbacks {
    private:
        std::vector<T_registeredObject> callbackObjects; //<< listeners

        static JavaVM *g_VM;
    public:
        /**
         * Attach to JVM. If the thread isn't attached the call of Java functions would fail.
         * @param attached result of attachement
         * @return env to use for other operations
         */
        static JNIEnv *AttachJava(bool *attached);

        static void DetachJava();

        /**
         * Notifies listeners about change of voice activity.
         */
        void notifyVADChanged(bool activity);

        /**
         * Notifies listeners about recognized sequence
         * @param sequence recognized sequence
         */
        void notifySequenceRecognized(std::string sequence);

        /**
         * Notifies listeners about all threads finishing.
         */
        void notifyRecognitionDone();

        void notifyMelDone(int percentage);

        void notifyNNDone(int percentage);

        void notifyDecoderDone(int percentage);

        void addListener(T_registeredObject obj);

        void setJavaVM(JavaVM *vm);
    };
}

#endif //VOICERECOGNITION_JAVACALLBACKS_H
