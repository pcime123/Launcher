package com.sscctv.seeeyes.ptz;

/**
 * Created by trlim on 2016. 2. 24..
 *
 * PTZ 제어 공통 interface를 정의
 */
public interface PtzControl {
    char PAN_LEFT   = 1;
    char PAN_RIGHT  = (1 << 1);
    char TILT_UP    = (1 << 2);
    char TILT_DOWN  = (1 << 3);
    char ZOOM_IN    = (1 << 4);
    char ZOOM_OUT   = (1 << 5);
    char FOCUS_FAR  = (1 << 6);
    char FOCUS_NEAR = (1 << 7);

    char STOP       = 0x100;

    char MENU_UP    = 0x110;
    char MENU_DOWN  = 0x111;
    char MENU_LEFT  = 0x112;
    char MENU_RIGHT = 0x113;

    char MENU_ON    = 0x115;
    char MENU_ENTER = 0x116;
    char MENU_ESC   = 0x117;

    void stop();

    void sendCommand(char command);
}
