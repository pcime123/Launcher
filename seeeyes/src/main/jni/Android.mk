LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
TARGET_PLATFORM := android-22
LOCAL_MODULE    := seeeyes-lib
LOCAL_SRC_FILES := \
    seeeyes-jni.cpp \
    sysfs_util.cpp \
    ptz_protocol.cpp \
    ptz_analyze.cpp \
    utc_device.cpp \
    display_out.cpp \
    SerialPort.c    \
    SpiPort.c

LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)