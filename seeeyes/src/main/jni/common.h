//
// Created by 임태일 on 2015. 12. 25..
//

#ifndef SEEEYESMONITOR_COMMON_H
#define SEEEYESMONITOR_COMMON_H

#include <jni.h>
#include <android/log.h>

#define LOG_TAG "SeeEyesJNI"
#define  LOGUNK(...)  __android_log_print(ANDROID_LOG_UNKNOWN,LOG_TAG,__VA_ARGS__)
#define  LOGDEF(...)  __android_log_print(ANDROID_LOG_DEFAULT,LOG_TAG,__VA_ARGS__)
#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define  LOGS(...)  __android_log_print(ANDROID_SILENT_ERROR,LOG_TAG,__VA_ARGS__)

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Register one or more native methods with a particular class.
 * "className" looks like "java/lang/String". Aborts on failure.
 * TODO: fix all callers and change the return type to void.
 */
int jniRegisterNativeMethods(C_JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods);

/*
 * Throw an exception with the specified class and an optional message.
 *
 * The "className" argument will be passed directly to FindClass, which
 * takes strings with slashes (e.g. "java/lang/Object").
 *
 * If an exception is currently pending, we log a warning message and
 * clear it.
 *
 * Returns 0 on success, nonzero if something failed (e.g. the exception
 * class couldn't be found, so *an* exception will still be pending).
 *
 * Currently aborts the VM if it can't throw the exception.
 */
int jniThrowException(C_JNIEnv* env, const char* className, const char* msg);

/*
 * Create a java.io.FileDescriptor given an integer fd
 */
jobject jniCreateFileDescriptor(C_JNIEnv* env, int fd);

/*
 * Sets the int fd in a java.io.FileDescriptor.
 */
void jniSetFileDescriptorOfFD(C_JNIEnv* env, jobject fileDescriptor, int value);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
inline int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* gMethods, int numMethods) {
    return jniRegisterNativeMethods(&env->functions, className, gMethods, numMethods);
}

inline int jniThrowException(JNIEnv* env, const char* className, const char* msg) {
    return jniThrowException(&env->functions, className, msg);
}

inline jobject jniCreateFileDescriptor(JNIEnv* env, int fd) {
    return jniCreateFileDescriptor(&env->functions, fd);
}

inline void jniSetFileDescriptorOfFD(JNIEnv* env, jobject fileDescriptor, int value) {
    jniSetFileDescriptorOfFD(&env->functions, fileDescriptor, value);
}
#endif

#endif //SEEEYESMONITOR_COMMON_H
