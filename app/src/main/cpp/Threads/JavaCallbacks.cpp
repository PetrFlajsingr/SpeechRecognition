//
// Created by Petr Flajsingr on 08/04/2018.
//

#include <jni.h>
#include "JavaCallbacks.h"

JavaVM* SpeechRecognition::Threads::JavaCallbacks::g_VM = NULL;

JNIEnv* SpeechRecognition::Threads::JavaCallbacks::AttachJava(bool* attached) {
    if(g_VM == NULL) {
        *attached = false;
        return NULL;
    }
    JavaVMAttachArgs args = {JNI_VERSION_1_2, 0, 0};
    JNIEnv* java;
    if(g_VM->AttachCurrentThread(&java, &args) == JNI_FALSE)
        *attached = false;
    else
        *attached = true;
    return java;
}

void SpeechRecognition::Threads::JavaCallbacks::DetachJava(){
    if(g_VM == NULL)
        return;
    g_VM->DetachCurrentThread();
}

void SpeechRecognition::Threads::JavaCallbacks::notifyVADChanged(bool activity){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
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

void SpeechRecognition::Threads::JavaCallbacks::notifySequenceRecognized(std::string sequence){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "sequenceRecognized", "(Ljava/lang/String;)V");
        env->CallVoidMethod(iterator->obj, methodID, env->NewStringUTF(sequence.c_str()));
    }
    if(attached)
        DetachJava();
}

void SpeechRecognition::Threads::JavaCallbacks::notifyRecognitionDone(){
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "recognitionDone", "()V");
        env->CallVoidMethod(iterator->obj, methodID);
    }
    if(attached)
        DetachJava();
}

void SpeechRecognition::Threads::JavaCallbacks::addListener(T_registeredObject obj) {
    callbackObjects.push_back(obj);
}

void SpeechRecognition::Threads::JavaCallbacks::setJavaVM(JavaVM* vm) {
    g_VM = vm;
}

void SpeechRecognition::Threads::JavaCallbacks::notifyMelDone(int percentage) {
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "melDone", "(I)V");
        env->CallVoidMethod(iterator->obj, methodID, percentage);
    }
    if(attached)
        DetachJava();
}

void SpeechRecognition::Threads::JavaCallbacks::notifyNNDone(int percentage) {
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "nnDone", "(I)V");
        env->CallVoidMethod(iterator->obj, methodID, percentage);
    }
    if(attached)
        DetachJava();
}

void SpeechRecognition::Threads::JavaCallbacks::notifyDecoderDone(int percentage) {
    bool attached;
    JNIEnv* env = AttachJava(&attached);
    if(env == NULL)
        return;
    for(auto iterator = callbackObjects.begin();
        iterator != callbackObjects.end();
        iterator++){
        jmethodID methodID = env->GetMethodID(iterator->clazz, "decoderDone", "(I)V");
        env->CallVoidMethod(iterator->obj, methodID, percentage);
    }
    if(attached)
        DetachJava();
}
