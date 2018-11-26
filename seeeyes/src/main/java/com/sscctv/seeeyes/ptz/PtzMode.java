package com.sscctv.seeeyes.ptz;

import android.util.Log;

import java.io.IOException;

/**
 * Created by trlim on 2016. 2. 24..
 *
 * PTZ 모드 처리기의 추상 베이스 클래스
 */
public abstract class PtzMode implements PtzControl {
    public static final int OFF = -1;
    public static final int TX = 0;
    public static final int RX = 1;
    public static final int ANALYZER = 2;
    public static final int UTC = 3;
    public static final int UCC = 4;

    protected final McuControl mControl;

    public PtzMode(McuControl control, int baudRate) {
        mControl = control;

        setBaudRate(baudRate);
    }

    public void setBaudRate(int baudRate) {
        try {
            mControl.setRs485BaudRate(baudRate);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {
        try {
            mControl.setPtzMode(OFF);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: PtzMode Could not load seeeyes library!");
        }
    }
}
