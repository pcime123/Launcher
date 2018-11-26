//
// Created by 임태일 on 2016. 1. 18..
//
#include "common.h"

#include <poll.h>
#include <stdio.h>

static jint
com_sscctv_seeeyes_SysFsMonitor_poll(JNIEnv *env, jobject thiz, jint exitFd, jint monitorFd, jint timeout)
{
    struct pollfd fds[2];
    int ret;

    fds[0].fd = exitFd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = monitorFd;
    fds[1].events = POLLPRI|POLLERR;
    fds[1].revents = 0;

    if ((ret = poll(fds, 2, timeout)) < 0 ) {
        return -1;
    } else if (ret == 0) {
        return 0;
    } else {
        int events = 0;

        while (ret > 0) {
            if (fds[0].revents & POLLIN) {
                events |= 1 << 0;
                ret--;
            } else if (fds[1].revents & POLLPRI|POLLERR) {
                events |= 1 << 1;
                ret--;
            }
        }
        return events;
    }
}

static JNINativeMethod method_table[] = {
        { "native_poll",    "(III)I", (void *)com_sscctv_seeeyes_SysFsMonitor_poll },
};

int register_com_sscctv_seeeyes_SysFsUtil(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/sscctv/seeeyes/SysFsMonitor");
    if (clazz == NULL) {
        LOGE("Can't find com/sscctv/seeeyes/SysFsMonitor");
        return -1;
    }

    return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/SysFsMonitor",
                                    method_table, sizeof(method_table)/sizeof(method_table[0]));
}
