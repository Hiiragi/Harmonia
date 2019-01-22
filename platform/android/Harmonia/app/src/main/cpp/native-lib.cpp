#include <jni.h>
#include <string>
#include <sstream>


#include "../../../../../../../src/c/HarmoniaInterface.h"


extern "C"
JNIEXPORT jstring JNICALL Java_jp_hiiragi_harmonia_MainActivity_stringFromJNI(JNIEnv *env, jobject obj) {

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1initialize(JNIEnv *env, jobject instance,
                                                           jint bufferSize) {
    int size = (int) bufferSize;
    harmonia_initialize(size);
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1finalize(JNIEnv *env, jobject instance) {
    harmonia_finalize();
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1register_1sound(JNIEnv *env, jobject instance,
                                                                jstring id_, jbyteArray binaryData_,
                                                                jint size, jint loopStartPoint,
                                                                jint loopLength) {
    const char *id = env->GetStringUTFChars(id_, 0);
    jbyte *binaryData = env->GetByteArrayElements(binaryData_, NULL);

    harmonia_register_sound(id, (unsigned char*)binaryData, size, loopStartPoint, loopLength);

    env->ReleaseStringUTFChars(id_, id);
    env->ReleaseByteArrayElements(binaryData_, binaryData, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1unregister_1sound(JNIEnv *env, jobject instance,
                                                                  jstring id_) {
    const char *id = env->GetStringUTFChars(id_, 0);

    harmonia_unregister_sound(id);

    env->ReleaseStringUTFChars(id_, id);
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1pause_1all(JNIEnv *env, jobject instance) {

    harmonia_pause_all();
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1resume_1all(JNIEnv *env, jobject instance) {

    harmonia_resume_all();
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1stop_1all(JNIEnv *env, jobject instance) {

    harmonia_stop_all();
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1get_1master_1volume(JNIEnv *env, jobject instance) {

    return harmonia_get_master_volume();
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1set_1master_1volume(JNIEnv *env, jobject instance,
                                                                    jfloat volume) {

    harmonia_set_master_volume(volume);
}

extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1mute_1all(JNIEnv *env, jobject instance) {

    harmonia_mute_all();

}extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1unmute_1all(JNIEnv *env, jobject instance) {

    harmonia_unmute_all();

}extern "C"
JNIEXPORT void JNICALL
Java_jp_hiiragi_harmonia_MainActivity_harmonia_1sound_1play(JNIEnv *env, jobject instance,
                                                            jstring registeredId_, jstring soundId_,
                                                            jstring targetGroupId_) {
    const char *registeredId = env->GetStringUTFChars(registeredId_, 0);
    const char *soundId = env->GetStringUTFChars(soundId_, 0);
    const char *targetGroupId = env->GetStringUTFChars(targetGroupId_, 0);

    harmonia_sound_play(registeredId, soundId, targetGroupId);

    env->ReleaseStringUTFChars(registeredId_, registeredId);
    env->ReleaseStringUTFChars(soundId_, soundId);
    env->ReleaseStringUTFChars(targetGroupId_, targetGroupId);
}