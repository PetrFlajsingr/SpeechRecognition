//
// Created by Petr Flajsingr on 08/04/2018.
//

#ifndef VOICERECOGNITION_JAVACALLBACKS_H
#define VOICERECOGNITION_JAVACALLBACKS_H


#include <jni.h>
#include <string>
#include <vector>

typedef struct registeredObject{
    jobject obj;
    jclass clazz;
}T_registeredObject;

class JavaCallbacks {
private:
    std::vector<T_registeredObject> callbackObjects;

    static JavaVM *g_VM;
public:
    JNIEnv* AttachJava(bool* attached);

    void DetachJava();

    void notifyVADChanged(bool activity);

    void notifySequenceRecognized(std::string sequence);

    void notifyRecognitionDone();

    void addListener(T_registeredObject obj);

    void setJavaVM(JavaVM* vm);
};


#endif //VOICERECOGNITION_JAVACALLBACKS_H
