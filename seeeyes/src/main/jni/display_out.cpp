//
// Created by 임태일 on 2016. 3. 10..
//

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "drv_display.h"

bool tvout_stat = 0;

static struct parcel_file_descriptor_offsets_t
{
    jclass mClass;
    jmethodID mConstructor;
} gParcelFileDescriptorOffsets;

static jobject com_sscctv_seeeyes_VideoSource_display_open(JNIEnv *env, jobject thiz, jstring path)
{
    const char *pathStr = env->GetStringUTFChars(path, NULL);

    int fd = open(pathStr, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        LOGE("could not open %s", pathStr);
        env->ReleaseStringUTFChars(path, pathStr);
        return NULL;
    }
    env->ReleaseStringUTFChars(path, pathStr);

    jobject fileDescriptor = jniCreateFileDescriptor(env, fd);
    if (fileDescriptor == NULL) {
        return NULL;
    }
    return env->NewObject(gParcelFileDescriptorOffsets.mClass,
                          gParcelFileDescriptorOffsets.mConstructor, fileDescriptor);
}

static int com_sscctv_seeeyes_VideoSource_display_cvbs_out(JNIEnv *env, jobject thiz, jint fd, jboolean tv_out) {

    int ret1, ret2;
    unsigned long arg[4]={0};

    arg[0] = 1;
    ret1 = ioctl(fd, DISP_CMD_HDMI_GET_HPD_STATUS, (unsigned long)arg);
    //LOGD( "HDMI_HPD_STATUS = %d", ret1);

    arg[0] = 1;
    ret2 = ioctl(fd, DISP_CMD_GET_OUTPUT_TYPE, (unsigned long)arg);
    //LOGD( "DISP_OUTPUT_TYPE = %d", ret2);

    if(tv_out) {
        if((ret1 == 0) && (ret2 != DISP_OUTPUT_TYPE_TV) && (tvout_stat == 0)) {
            arg[0] = 1;
            arg[1] = DISP_TV_MOD_NTSC;
            ioctl(fd, DISP_CMD_TV_SET_MODE, (unsigned long) arg);

            arg[0] = 1;
            ret2 = ioctl(fd, DISP_CMD_TV_ON, (unsigned long) arg);

            LOGD( "TV_ON");
        }

        tvout_stat = 1;
    }
    else {
        if((ret1 == 0) && (ret2 == DISP_OUTPUT_TYPE_TV)) {//} && (tvout_stat == 1)) {
            arg[0] = 1;
            ret2 = ioctl(fd, DISP_CMD_TV_OFF, (unsigned long) arg);

            LOGD( "TV_OFF");
        }

        tvout_stat = 0;
    }

    return ret2;
}

static JNINativeMethod method_table[] = {
        { "native_display_open",  "(Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;",
                                   (void *)com_sscctv_seeeyes_VideoSource_display_open },
        { "native_display_cvbs_out",   "(IZ)I",
                                   (void *)com_sscctv_seeeyes_VideoSource_display_cvbs_out },
};

int register_com_sscctv_seeeyes_VideoSource(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/sscctv/seeeyes/VideoSource");
    if (clazz == NULL) {
        LOGE("Can't find com/sscctv/seeeyes/VideoSource");
        return -1;
    }

    clazz = env->FindClass("android/os/ParcelFileDescriptor");
    if (clazz == NULL) {
        LOGF("Unable to find class android.os.ParcelFileDescriptor");
        return -1;
    };
    gParcelFileDescriptorOffsets.mClass = (jclass) env->NewGlobalRef(clazz);
    gParcelFileDescriptorOffsets.mConstructor = env->GetMethodID(clazz, "<init>", "(Ljava/io/FileDescriptor;)V");
    if (gParcelFileDescriptorOffsets.mConstructor == NULL) {
        LOGF("Unable to find constructor for android.os.ParcelFileDescriptor");
        return -1;
    }

    return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/VideoSource",
                                    method_table, sizeof(method_table)/sizeof(method_table[0]));
}
