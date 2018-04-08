//
// Created by Petr Flajsingr on 08/04/2018.
//

#include <jni.h>
#include "JavaCallbacks.h"

JavaVM* JavaCallbacks::g_VM = NULL;


JNIEnv* JavaCallbacks::AttachJava(bool* attached) {
    JavaVMAttachArgs args = {JNI_VERSION_1_2, 0, 0};
    JNIEnv* java;
    if(g_VM->AttachCurrentThread(&java, &args) == JNI_FALSE)
        *attached = false;
    else
        *attached = true;
    return java;
}

void JavaCallbacks::DetachJava(){
    g_VM->DetachCurrentThread();
}

void JavaCallbacks::notifyVADChanged(bool activity){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    //jclass tmpClazz = env->FindClass("cz/vutbr/fit/xflajs00/voicerecognition/SpeechRecognitionAPI");
    //tmpClazz = (jclass)env->NewGlobalRef(tmpClazz);
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "VADChanged", "(Z)V");
        iterator->obj = env->NewGlobalRef(iterator->obj);
        env->CallVoidMethod(iterator->obj, methodID, activity ? JNI_TRUE : JNI_FALSE);
    }
    if(attached)
        DetachJava();
}

void JavaCallbacks::notifySequenceRecognized(std::string sequence){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "sequenceRecognized", "(Ljava/lang/String;)V");
        env->CallVoidMethod(iterator->obj, methodID, env->NewStringUTF(sequence.c_str()));
    }
    if(attached)
        DetachJava();
}

void JavaCallbacks::notifyRecognitionDone(){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "recognitionDone", "()V");
        env->CallVoidMethod(iterator->obj, methodID);
    }
    if(attached)
        DetachJava();
}

void JavaCallbacks::addListener(T_registeredObject obj) {
    callbackObjects.push_back(obj);
}

void JavaCallbacks::setJavaVM(JavaVM* vm) {
    if(g_VM == NULL)
        g_VM = vm;
}
