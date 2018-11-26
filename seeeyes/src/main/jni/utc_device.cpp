//
// Created by 임태일 on 2016. 3. 10..
//

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "tp2802.h"

static struct parcel_file_descriptor_offsets_t
{
    jclass mClass;
    jmethodID mConstructor;
} gParcelFileDescriptorOffsets;

static jobject com_sscctv_seeeyes_ptz_UtcWriter_open(JNIEnv *env, jobject thiz, jstring path)
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

#define	KEY_PAN_LEFT        (1 << 0)
#define	KEY_PAN_RIGHT       (1 << 1)
#define	KEY_TILT_UP         (1 << 2)
#define	KEY_TILT_DOWN       (1 << 3)
#define	KEY_ZOOM_TELE       (1 << 4)
#define	KEY_ZOOM_WIDE       (1 << 5)
#define	KEY_FOCUS_FAR       (1 << 6)
#define	KEY_FOCUS_NEAR      (1 << 7)
#define	KEY_PTZ_STOP        0x100
#define	KEY_MENU_UP         0x110
#define	KEY_MENU_DOWN       0x111
#define	KEY_MENU_LEFT       0x112
#define	KEY_MENU_RIGHT      0x113
#define	KEY_MENU_ON         0x115
#define	KEY_MENU_ENTER      0x116
#define	KEY_MENU_ESC        0x117

/**
 * UtcProtocol 클래스의 정의와 같아야 한다.
 */
enum {
    PROTOCOL_TVI_HIK_VISION0 = 0,
    PROTOCOL_TVI_HIK_VISION1 = 1,
    PROTOCOL_TVI_PELCO_C = 2,

    PROTOCOL_AHD_A_CPC = 10,
    PROTOCOL_AHD_A_CPC_720P = 11,
    PROTOCOL_AHD_A_CPC_CVBS = 12,

    PROTOCOL_CVI_DAHUA0 = 20,
    PROTOCOL_CVI_DAHUA1 = 21,
    PROTOCOL_CVI_DAHUA2 = 22,
};

int Cmd_buff = KEY_PTZ_STOP;

static int com_sscctv_seeeyes_ptz_UtcWriter_send_command(JNIEnv *env, jobject thiz, jint fd, jint mode, jint address, jint protocol, jchar command) {
    tp2802_PTZ_data PTZ_data = { 0, };
    unsigned char *txd = PTZ_data.data;
    int i, ret;

    //if((protocol == PROTOCOL_TVI_HIK_VISION0) || (protocol == PROTOCOL_TVI_HIK_VISION1) || (protocol == PROTOCOL_TVI_PELCO_C)){
    //    if(mode >= TP2802_4M30){
    //        protocol = PROTOCOL_AHD_A_CPC;
    //    }
    //}
    if(protocol == PROTOCOL_AHD_A_CPC){
        if((mode == TP2802_720P30) || (mode == TP2802_720P25)){
            protocol = PROTOCOL_AHD_A_CPC_720P;
        }
    }

    // id[]에 대한 index. 한 개의 chip만 지원할 것이므로 0으로 고정.
    PTZ_data.chip = 0;
    PTZ_data.ch = 0;

    switch (protocol) {
        case PROTOCOL_TVI_HIK_VISION0:   // HIK-VISION
        case PROTOCOL_TVI_HIK_VISION1:
            PTZ_data.mode = PTZ_TVI;

            txd[0] = txd[7] = 0xb5;
            txd[1] = 0x00;

            switch(command){
                case KEY_PTZ_STOP:		txd[2] = 0x14;	break;
                case KEY_MENU_ENTER:	//break;
                case KEY_MENU_ON:
                    if(protocol == PROTOCOL_TVI_HIK_VISION1){ txd[2] = 0x0f; }		// iris open
                    else{ txd[2] = 0x17;	txd[3] = 0x5f; }
                    break;

                case KEY_MENU_ESC:		break;

                case KEY_MENU_UP://		txd[2] = 0x06;	txd[3] = 0x01;	break;
                case KEY_TILT_UP:		txd[2] = 0x06;	txd[3] = 0x2d;	break;

                case KEY_MENU_DOWN://	txd[2] = 0x07;	txd[3] = 0x01;	break;
                case KEY_TILT_DOWN:		txd[2] = 0x07;	txd[3] = 0x2d;	break;

                case KEY_MENU_RIGHT://	txd[2] = 0x08;	txd[4] = 0x01;	break;
                case KEY_PAN_RIGHT:		txd[2] = 0x08;	txd[4] = 0x2d;	txd[5] = 0x2d;	break;

                case KEY_MENU_LEFT://	txd[2] = 0x09;	txd[4] = 0x01;	break;
                case KEY_PAN_LEFT:		txd[2] = 0x09;	txd[4] = 0x2d;	txd[5] = 0x2d;	break;

                case KEY_FOCUS_NEAR:	txd[2] = 0x10;	break;
                case KEY_FOCUS_FAR:		txd[2] = 0x11;	break;
                case KEY_ZOOM_WIDE:		txd[2] = 0x12;	txd[3] = 0x01;	break;
                case KEY_ZOOM_TELE:		txd[2] = 0x13;	txd[3] = 0x01;	break;

                default:	break;
            }

            for(i=2;i<7;i++)	txd[7] += txd[i];
            break;

        case PROTOCOL_TVI_PELCO_C:   // CNB :PELCO-C
            PTZ_data.mode = PTZ_TVI_PELCO;

            txd[0] = 0xFF;							// Data Packet의 시작
            txd[1] = 0x00;//Ptz_Cmd.Addr;			// receive device Ptz_Cmd.Address

            switch (command) {
                case KEY_PTZ_STOP:	break;
                case KEY_MENU_ENTER:
                case KEY_MENU_ON:	txd[3] = 0x07; txd[5] = 0x5f;	break;
                case KEY_FOCUS_NEAR:txd[2] = 0x01;	break;
                case KEY_MENU_ESC:
                case KEY_FOCUS_FAR:	txd[3] = 0x80;	break;
//                case FOCUS_AUTO:	break;
                default:
                    if(command&KEY_PAN_RIGHT)		{txd[3] |= 0x02; txd[4] = 0x30;}
                    else if(command&KEY_PAN_LEFT)	{txd[3] |= 0x04; txd[4] = 0x30;}
                    if(command&KEY_TILT_UP)			{txd[3] |= 0x08; txd[5] = 0x30;}
                    else if(command&KEY_TILT_DOWN)	{txd[3] |= 0x10; txd[5] = 0x30;}
                    if(command&KEY_ZOOM_TELE)		{txd[3] |= 0x20;}
                    else if(command&KEY_ZOOM_WIDE)	{txd[3] |= 0x40;}
                    break;
            }

            for(i=1;i<6;i++) txd[6] += txd[i];		 // Byte2~Byte6을 더한 값(overflow 무시)
            break;

        case PROTOCOL_AHD_A_CPC:
            if(mode == TP2802_QXGA18)   PTZ_data.mode = PTZ_HDA_3M18;
            else if(mode == TP2802_QXGA25 || mode == TP2802_QXGA30)  PTZ_data.mode = PTZ_HDA_3M25;
            else if(mode == TP2802_QHD25 || mode == TP2802_QHD30)    PTZ_data.mode = PTZ_HDA_4M25;
            else if(mode == TP2802_QHD15)    PTZ_data.mode = PTZ_HDA_4M15;
            else    PTZ_data.mode = PTZ_HDA_1080P;

            switch(command){
                case KEY_PTZ_STOP:		txd[0] = 0x00; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_MENU_ON:		txd[0] = 0x00; txd[1] = 0x03; txd[2] = 0x00; txd[3] = 0x3f;	break;
                case KEY_MENU_ESC:		break;
                case KEY_MENU_ENTER:	txd[0] = 0x02; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
//              case KEY_FOCUS_AUTO:	break;
                case KEY_MENU_UP:
                case KEY_TILT_UP:		txd[0] = 0x00; txd[1] = 0x08; txd[2] = 0x00; txd[3] = 0x32;	break;
                case KEY_MENU_DOWN:
                case KEY_TILT_DOWN:		txd[0] = 0x00; txd[1] = 0x10; txd[2] = 0x00; txd[3] = 0x32;	break;
                case KEY_MENU_LEFT:
                case KEY_PAN_LEFT:		txd[0] = 0x00; txd[1] = 0x04; txd[2] = 0x32; txd[3] = 0x00;	break;
                case KEY_MENU_RIGHT:
                case KEY_PAN_RIGHT:		txd[0] = 0x00; txd[1] = 0x02; txd[2] = 0x32; txd[3] = 0x00;	break;
                case KEY_FOCUS_NEAR:	txd[0] = 0x01; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_FOCUS_FAR:		txd[0] = 0x00; txd[1] = 0x80; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_ZOOM_WIDE:		txd[0] = 0x00; txd[1] = 0x40; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_ZOOM_TELE:		txd[0] = 0x00; txd[1] = 0x20; txd[2] = 0x00; txd[3] = 0x00;	break;
            }

            if((command == KEY_PTZ_STOP) && ((Cmd_buff&0x100) == 0x100)){		// 메뉴모드 키는 stop을 보내지 않음
				Cmd_buff = command;
            	return ret;
            }
			Cmd_buff = command;
            break;

        case PROTOCOL_AHD_A_CPC_720P:
            PTZ_data.mode = PTZ_HDA_720P;
			switch(command){
				case KEY_PTZ_STOP:		txd[0] = 0x00; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
				case KEY_MENU_ON:		txd[0] = 0x00; txd[1] = 0x03; txd[2] = 0x03; txd[3] = 0x5f;	break;
				case KEY_MENU_ESC:		break;
				case KEY_MENU_ENTER:	txd[0] = 0x02; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
				case KEY_MENU_UP:
				case KEY_TILT_UP:		txd[0] = 0x00; txd[1] = 0x08; txd[2] = 0x08; txd[3] = 0x32;	break;
				case KEY_MENU_DOWN:
				case KEY_TILT_DOWN:		txd[0] = 0x00; txd[1] = 0x10; txd[2] = 0x10; txd[3] = 0x32;	break;
				case KEY_MENU_LEFT:
				case KEY_PAN_LEFT:		txd[0] = 0x00; txd[1] = 0x04; txd[2] = 0x04; txd[3] = 0x00;	break;
				case KEY_MENU_RIGHT:
				case KEY_PAN_RIGHT:		txd[0] = 0x00; txd[1] = 0x02; txd[2] = 0x02; txd[3] = 0x00;	break;
				case KEY_FOCUS_NEAR:	txd[0] = 0x01; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
				case KEY_FOCUS_FAR:		txd[0] = 0x00; txd[1] = 0x80; txd[2] = 0x80; txd[3] = 0x00;	break;
				case KEY_ZOOM_WIDE:		txd[0] = 0x00; txd[1] = 0x40; txd[2] = 0x40; txd[3] = 0x00;	break;
				case KEY_ZOOM_TELE:		txd[0] = 0x00; txd[1] = 0x20; txd[2] = 0x20; txd[3] = 0x00;	break;
			}
            break;
        case PROTOCOL_AHD_A_CPC_CVBS:
            PTZ_data.mode = PTZ_PELCO_C;
			switch(command){
                case KEY_PTZ_STOP:		txd[0] = 0x00; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_MENU_ON:		txd[0] = 0x00; txd[1] = 0x03; txd[2] = 0x00; txd[3] = 0x5f;	break;
                case KEY_MENU_ESC:		txd[0] = 0x04; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_MENU_ENTER:	txd[0] = 0x02; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_MENU_UP:
                case KEY_TILT_UP:		txd[0] = 0x00; txd[1] = 0x08; txd[2] = 0x00; txd[3] = 0x30;	break;
                case KEY_MENU_DOWN:
                case KEY_TILT_DOWN:		txd[0] = 0x00; txd[1] = 0x10; txd[2] = 0x00; txd[3] = 0x30;	break;
                case KEY_MENU_LEFT:
                case KEY_PAN_LEFT:		txd[0] = 0x00; txd[1] = 0x04; txd[2] = 0x30; txd[3] = 0x00;	break;
                case KEY_MENU_RIGHT:
                case KEY_PAN_RIGHT:		txd[0] = 0x00; txd[1] = 0x02; txd[2] = 0x30; txd[3] = 0x00;	break;
                case KEY_FOCUS_NEAR:	txd[0] = 0x01; txd[1] = 0x00; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_FOCUS_FAR:		txd[0] = 0x00; txd[1] = 0x80; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_ZOOM_WIDE:		txd[0] = 0x00; txd[1] = 0x40; txd[2] = 0x00; txd[3] = 0x00;	break;
                case KEY_ZOOM_TELE:		txd[0] = 0x00; txd[1] = 0x20; txd[2] = 0x00; txd[3] = 0x00;	break;
			}
            break;
        case PROTOCOL_CVI_DAHUA0:
        case PROTOCOL_CVI_DAHUA1:
        case PROTOCOL_CVI_DAHUA2:
            if(mode == TP2802_QHD25 || mode == TP2802_QHD30)    PTZ_data.mode = PTZ_HDC_QHD;
            else    PTZ_data.mode = PTZ_HDC;

            txd[0] = 0xa5;
            txd[1] = address;

            if(protocol == PROTOCOL_CVI_DAHUA0)
            {
                switch(command){
                    case KEY_PTZ_STOP:		break;//txd[2] = 0x14;	break;
                    case KEY_MENU_ENTER:	//txd[2] = 0x89;	txd[4] = 0x03;	break;
                    case KEY_MENU_ON:		txd[2] = 0x89;	txd[4] = 0x01;	break;
                    case KEY_MENU_ESC:		txd[2] = 0x89;	txd[4] = 0x02;	break;
                        //  case KEY_MENU_ENTER:	txd[2] = 0x89;	txd[4] = 0x03;	break;
                    case KEY_MENU_UP:		txd[2] = 0x89;	txd[4] = 0x04;	break;
                    case KEY_MENU_DOWN:		txd[2] = 0x89;	txd[4] = 0x05;	break;
                    case KEY_MENU_LEFT:		txd[2] = 0x89;	txd[4] = 0x06;	break;
                    case KEY_MENU_RIGHT:	txd[2] = 0x89;	txd[4] = 0x07;	break;
                    case KEY_TILT_UP:		txd[2] = 0x08;	txd[4] = 0x80;	break;
                    case KEY_TILT_DOWN:		txd[2] = 0x04;	txd[4] = 0x80;	break;
                    case KEY_PAN_LEFT:		txd[2] = 0x02;	txd[3] = 0x80;	break;
                    case KEY_PAN_RIGHT:		txd[2] = 0x01;	txd[3] = 0x80;	break;
                    case KEY_FOCUS_NEAR:	txd[2] = 0x48;	break;
                    case KEY_FOCUS_FAR:		txd[2] = 0x44;	break;
                    case KEY_ZOOM_WIDE:		txd[2] = 0x42;	break;
                    case KEY_ZOOM_TELE:		txd[2] = 0x41;	break;
                }
            }
            else if(protocol == PROTOCOL_CVI_DAHUA1)
            {
                switch(command){
                    case KEY_PTZ_STOP:		break;//txd[2] = 0x14;	break;
                    case KEY_MENU_ENTER:
                    case KEY_MENU_ON:		txd[2] = 0x50;	break;
                    case KEY_MENU_ESC:		txd[2] = 0x89;	txd[4] = 0x02;	break;
                        //  case KEY_MENU_ENTER:	txd[2] = 0x89;	txd[4] = 0x03;	break;
                    case KEY_MENU_UP:		txd[2] = 0x08;	txd[4] = 0x80;	break;
                    case KEY_MENU_DOWN:		txd[2] = 0x04;	txd[4] = 0x80;	break;
                    case KEY_MENU_LEFT:		txd[2] = 0x02;	txd[3] = 0x80;	break;
                    case KEY_MENU_RIGHT:	txd[2] = 0x01;	txd[3] = 0x80;	break;
                    case KEY_TILT_UP:		txd[2] = 0x08;	txd[4] = 0x80;	break;
                    case KEY_TILT_DOWN:		txd[2] = 0x04;	txd[4] = 0x80;	break;
                    case KEY_PAN_LEFT:		txd[2] = 0x02;	txd[3] = 0x80;	break;
                    case KEY_PAN_RIGHT:		txd[2] = 0x01;	txd[3] = 0x80;	break;
                    case KEY_FOCUS_NEAR:	txd[2] = 0x48;	break;
                    case KEY_FOCUS_FAR:		txd[2] = 0x44;	break;
                    case KEY_ZOOM_WIDE:		txd[2] = 0x42;	break;
                    case KEY_ZOOM_TELE:		txd[2] = 0x41;	break;
                }
            }
            else if(protocol == PROTOCOL_CVI_DAHUA2)
            {
                switch(command){
                    case KEY_PTZ_STOP:		break;
                    case KEY_MENU_ON:		txd[2] = 0x89;	txd[3] = 0x01;	break;
                    case KEY_MENU_ENTER:	txd[2] = 0x89;	txd[3] = 0x08;	break;
                    case KEY_MENU_ESC:		txd[2] = 0x89;	txd[3] = 0x00;	break;
                    case KEY_MENU_UP:		txd[2] = 0x89;	txd[3] = 0x04;	break;
                    case KEY_MENU_DOWN:		txd[2] = 0x89;	txd[3] = 0x05;	break;
                    case KEY_MENU_LEFT:		txd[2] = 0x89;	txd[3] = 0x06;	break;
                    case KEY_MENU_RIGHT:	txd[2] = 0x89;	txd[3] = 0x07;	break;

                    case KEY_TILT_UP:		txd[2] = 0x08;	txd[4] = 0x80;	break;		// speed : 1=0x20, 2=0x40, 3=0x60, 4=0x80, 5=0x9f
                    case KEY_TILT_DOWN:		txd[2] = 0x04;	txd[4] = 0x80;	break;
                    case KEY_PAN_LEFT:		txd[2] = 0x02;	txd[3] = 0x80;	break;
                    case KEY_PAN_RIGHT:		txd[2] = 0x01;	txd[3] = 0x80;	break;
                    case KEY_FOCUS_NEAR:	txd[2] = 0x48;	break;
                    case KEY_FOCUS_FAR:		txd[2] = 0x44;	break;
                    case KEY_ZOOM_WIDE:		txd[2] = 0x42;	break;
                    case KEY_ZOOM_TELE:		txd[2] = 0x41;	break;
                }
            }

            if(command == KEY_PTZ_STOP){
                if((Cmd_buff == KEY_FOCUS_NEAR) || (Cmd_buff == KEY_FOCUS_FAR) || (Cmd_buff == KEY_ZOOM_WIDE) || (Cmd_buff == KEY_ZOOM_TELE)){
                    txd[2] = 0x40;
                }
            }

            Cmd_buff = command;

            txd[6] = txd[0] + txd[1] + txd[2] + txd[3] + txd[4] + txd[5];
            break;

        default:
            return -1;
    }

	if((command&0x100) == 0)	txd[8] = 0x10;			// PTZF 모드에서는 repeat 출력 on을 위한 정보 추가

	ret = ioctl(fd, TP2802_SET_PTZ_DATA, &PTZ_data);

//    LOGD("PTZ_data.mode: %d\n", PTZ_data.mode);
//    LOGD("PTZ_data.data: %02x %02x %02x %02x %02x %02x %02x %02x %02x"
//    	, PTZ_data.data[0], PTZ_data.data[1], PTZ_data.data[2], PTZ_data.data[3]
//    	, PTZ_data.data[4], PTZ_data.data[5], PTZ_data.data[6], PTZ_data.data[7], PTZ_data.data[8]);

    return ret;
}

static JNINativeMethod method_table[] = {
        { "native_open",            "(Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;",
                                    (void *)com_sscctv_seeeyes_ptz_UtcWriter_open },
        { "native_send_command",    "(IIIIC)I",
                                    (void *)com_sscctv_seeeyes_ptz_UtcWriter_send_command },
};

int register_com_sscctv_seeeyes_ptz_UtcWriter(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/sscctv/seeeyes/ptz/UtcWriter");
    if (clazz == NULL) {
        LOGE("Can't find com/sscctv/seeeyes/ptz/UtcWriter");
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

    return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/ptz/UtcWriter",
                                    method_table, sizeof(method_table)/sizeof(method_table[0]));
}
