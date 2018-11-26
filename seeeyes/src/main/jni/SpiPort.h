//
// Created by JinseopKim on 2018-07-11.
//
#include <jni.h>
#ifndef _Included_com_sscctv_seeeyes_SpiPort_
#define _Included_com_sscctv_seeeyes_SpiPort_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     android_serialport_api_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_com_sscctv_seeeyes_SpiPort_open
        (JNIEnv *, jclass, jstring);

/*
 * Class:     android_serialport_api_SerialPort
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_sscctv_seeeyes_SpiPort_close
(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
