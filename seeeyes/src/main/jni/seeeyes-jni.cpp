//
// Created by 임태일 on 2015. 12. 25..
//

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static struct CachedFields {
//    jclass fileDescriptorClass;
//    jmethodID fileDescriptorCtor;
//    jfieldID descriptorField;
//} gCachedFields;

int register_com_sscctv_seeeyes_SysFsUtil(JNIEnv *env);

int register_com_sscctv_seeeyes_ptz_PtzWriter(JNIEnv *env);
//int register_com_sscctv_seeeyes_ptz_PtzUcc(JNIEnv *env);
int register_com_sscctv_seeeyes_ptz_PtzAnalyzer(JNIEnv *env);

int register_com_sscctv_seeeyes_ptz_UtcWriter(JNIEnv *env);

int register_com_sscctv_seeeyes_VideoSource(JNIEnv *env);

/**
 * Java에서 라이브러리를 load할 때 호출한다.
 */
extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("GetEnv failed!");
        return result;
    }

    gCachedFields.fileDescriptorClass =
        reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("java/io/FileDescriptor")));
    if (gCachedFields.fileDescriptorClass == NULL) {
        abort();
    }
    gCachedFields.fileDescriptorCtor =
            env->GetMethodID(gCachedFields.fileDescriptorClass, "<init>", "()V");
    if (gCachedFields.fileDescriptorCtor == NULL) {
        abort();
    }
    gCachedFields.descriptorField =
            env->GetFieldID(gCachedFields.fileDescriptorClass, "descriptor", "I");
    if (gCachedFields.descriptorField == NULL) {
        abort();
    }

    register_com_sscctv_seeeyes_SysFsUtil(env);

    register_com_sscctv_seeeyes_ptz_PtzWriter(env);
    register_com_sscctv_seeeyes_ptz_PtzAnalyzer(env);

    register_com_sscctv_seeeyes_ptz_UtcWriter(env);
//    register_com_sscctv_seeeyes_ptz_PtzUcc(env);

    register_com_sscctv_seeeyes_VideoSource(env);

    return JNI_VERSION_1_4;
}

// 아래는 AOSP의 JNIHelp.cpp에서 가져온 것

/**
 * Equivalent to ScopedLocalRef, but for C_JNIEnv instead. (And slightly more powerful.)
 */
template<typename T>
class scoped_local_ref {
public:
    scoped_local_ref(C_JNIEnv* env, T localRef = NULL)
            : mEnv(env), mLocalRef(localRef)
    {
    }

    ~scoped_local_ref() {
        reset();
    }

    void reset(T localRef = NULL) {
        if (mLocalRef != NULL) {
            (*mEnv)->DeleteLocalRef(reinterpret_cast<JNIEnv*>(mEnv), mLocalRef);
            mLocalRef = localRef;
        }
    }

    T get() const {
        return mLocalRef;
    }

private:
    C_JNIEnv* mEnv;
    T mLocalRef;

    // Disallow copy and assignment.
    scoped_local_ref(const scoped_local_ref&);
    void operator=(const scoped_local_ref&);
};

static jclass findClass(C_JNIEnv* env, const char* className) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
    return (*env)->FindClass(e, className);
}

extern "C" int jniRegisterNativeMethods(C_JNIEnv* env, const char* className,
                                        const JNINativeMethod* gMethods, int numMethods)
{
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);

    scoped_local_ref<jclass> c(env, findClass(env, className));
    if (c.get() == NULL) {
        LOGE("Native registration unable to find class '%s', aborting", className);
        abort();
    }

    if ((*env)->RegisterNatives(e, c.get(), gMethods, numMethods) < 0) {
        LOGE("RegisterNatives failed for '%s', aborting", className);
        abort();
    }

    return 0;
}

/*
 * Returns a human-readable summary of an exception object.  The buffer will
 * be populated with the "binary" class name and, if present, the
 * exception message.
 */
static char* getExceptionSummary0(C_JNIEnv* env, jthrowable exception) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);

    /* get the name of the exception's class */
    scoped_local_ref<jclass> exceptionClass(env, (*env)->GetObjectClass(e, exception)); // can't fail
    scoped_local_ref<jclass> classClass(env,
                                        (*env)->GetObjectClass(e, exceptionClass.get())); // java.lang.Class, can't fail
    jmethodID classGetNameMethod =
            (*env)->GetMethodID(e, classClass.get(), "getName", "()Ljava/lang/String;");
    scoped_local_ref<jstring> classNameStr(env,
                                           (jstring) (*env)->CallObjectMethod(e, exceptionClass.get(), classGetNameMethod));
    if (classNameStr.get() == NULL) {
        return NULL;
    }

    /* get printable string */
    const char* classNameChars = (*env)->GetStringUTFChars(e, classNameStr.get(), NULL);
    if (classNameChars == NULL) {
        return NULL;
    }

    /* if the exception has a detail message, get that */
    jmethodID getMessage =
            (*env)->GetMethodID(e, exceptionClass.get(), "getMessage", "()Ljava/lang/String;");
    scoped_local_ref<jstring> messageStr(env,
                                         (jstring) (*env)->CallObjectMethod(e, exception, getMessage));
    if (messageStr.get() == NULL) {
        return strdup(classNameChars);
    }

    char* result = NULL;
    const char* messageChars = (*env)->GetStringUTFChars(e, messageStr.get(), NULL);
    if (messageChars != NULL) {
        asprintf(&result, "%s: %s", classNameChars, messageChars);
        (*env)->ReleaseStringUTFChars(e, messageStr.get(), messageChars);
    } else {
        (*env)->ExceptionClear(e); // clear OOM
        asprintf(&result, "%s: <error getting message>", classNameChars);
    }

    (*env)->ReleaseStringUTFChars(e, classNameStr.get(), classNameChars);
    return result;
}

static char* getExceptionSummary(C_JNIEnv* env, jthrowable exception) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
    char* result = getExceptionSummary0(env, exception);
    if (result == NULL) {
        (*env)->ExceptionClear(e);
        result = strdup("<error getting class name>");
    }
    return result;
}

extern "C" int jniThrowException(C_JNIEnv* env, const char* className, const char* msg) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);

    if ((*env)->ExceptionCheck(e)) {
        /* TODO: consider creating the new exception with this as "cause" */
        scoped_local_ref<jthrowable> exception(env, (*env)->ExceptionOccurred(e));
        (*env)->ExceptionClear(e);

        if (exception.get() != NULL) {
            char* text = getExceptionSummary(env, exception.get());
            LOGW("Discarding pending exception (%s) to throw %s", text, className);
            free(text);
        }
    }

    scoped_local_ref<jclass> exceptionClass(env, findClass(env, className));
    if (exceptionClass.get() == NULL) {
        LOGE("Unable to find exception class %s", className);
        /* ClassNotFoundException now pending */
        return -1;
    }

    if ((*env)->ThrowNew(e, exceptionClass.get(), msg) != JNI_OK) {
        LOGE("Failed throwing '%s' '%s'", className, msg);
        /* an exception, most likely OOM, will now be pending */
        return -1;
    }

    return 0;
}

extern "C" jobject jniCreateFileDescriptor(C_JNIEnv* env, int fd) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
    jobject fileDescriptor = (*env)->NewObject(e,
        gCachedFields.fileDescriptorClass, gCachedFields.fileDescriptorCtor);
    jniSetFileDescriptorOfFD(env, fileDescriptor, fd);
    return fileDescriptor;
}

extern "C" void jniSetFileDescriptorOfFD(C_JNIEnv* env, jobject fileDescriptor, int value) {
    JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
    (*env)->SetIntField(e, fileDescriptor, gCachedFields.descriptorField, value);
}
