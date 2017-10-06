#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_cz_vutbr_fit_xflajs00_voicerecognition_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
