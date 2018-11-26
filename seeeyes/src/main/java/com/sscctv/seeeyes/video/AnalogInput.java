package com.sscctv.seeeyes.video;

import android.os.Bundle;
//import android.util.Log;
import android.util.Log;
import android.view.SurfaceView;

import com.sscctv.seeeyes.SysFsMonitor;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

/**
 * Created by trlim on 2016. 2. 26..
 *
 * TV 입력을 다루는 클래스
 */
public class AnalogInput extends CameraInput {
    //private static final String TAG = "AnalogInput";

    private final SysFsMonitor mSignalMonitor;
    private String TAG = "AnalogInput";
    //private boolean mForceNotifySignalChange = false;

        public AnalogInput(SurfaceView surfaceView, Listener listener) {
        super(VideoInput.VIDEO_INPUT_AUTO, surfaceView, listener);

        mSignalMonitor = new SysFsMonitor("/sys/class/misc/tp28xx/signal");

        //setMode(VIDEO_INPUT_NONE);
        HdmiInput.disable();
        SdiInput.disable();
    }

    // CVBS, AHD, TVI, CVI는 모두 cameraId = 2를 공유한다.
    @Override
    protected int getCameraId() { return VIDEO_INPUT_AUTO; }

    @Override
    public void start(Bundle args) {
        super.start(args);

        setMode(getInput());
//        Log.d(TAG, "Set Mode - GetInput = " + getInput());

        try {
            //mForceNotifySignalChange = true;

            mSignalMonitor.start(new SysFsMonitor.AttributeChangeHandler() {
                @Override
                public void onAttributeChange(RandomAccessFile file) {
//                    Log.d(TAG, "mSignalMonitor Start");

                    byte[] value = new byte[16];
                    int length = 0;
                    try {
                        length = file.read(value);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (length > 0) {
                        String signal = new String(value).substring(0, length);
//                        Log.d(TAG, "What Signal = " + signal);

                        switch (signal) {
                            case "nosignal":
                                notifySignalChange(false, 0, 0, '?', 0.0F, 0xff, 0xff);
                                if(!isRecording()) {
                                    //stopPreview();
                                    stopCameraInput();
                                }

                                //mForceNotifySignalChange = false;
                                break;

                            case "changed":
                                //Log.d(TAG, "signal = " + signal);

                                //if (getCamera() != null)    stopCameraInput();
                                startCameraInput();
                                startPreview();
                                notifySignalChange(true, getWidth(), getHeight(), getScan(), getRate(), getMode(), getStd());

                                //mForceNotifySignalChange = false;
                                break;

                            default:
                                //if (mForceNotifySignalChange) {
                                    notifySignalChange(true, getWidth(), getHeight(), getScan(), getRate(), getMode(), getStd());

                                    //stopAndStartPreview();

                                    //mForceNotifySignalChange = false;
                                //}
                                break;
                        }
                    }
                }
            });
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

//    public static int getSystem(int mode) {
//        int height = getHeight();
//
//        if(getMode() == 9) {
//            if(getHeight() == 480) {
//
//            }
//        }
//    }

    @Override
    public void stop() {
        try {
            mSignalMonitor.stop();
        } catch (IOException e) {
            e.printStackTrace();
        }

        disable();
        super.stop();
    }

    public static void disable() {
        setMode(VIDEO_INPUT_NONE);
    }

    private static void setMode(int input) {
        String value;
        switch (input) {
            case VIDEO_INPUT_AUTO:
                value = "1";
                break;
            default:
                value = "0";
                break;
        }
            try {
            FileOutputStream file = new FileOutputStream("/sys/class/misc/tp28xx/run");
            file.write(value.getBytes());
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public int getWidth() {
        int width = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/width");
            byte[] value = new byte[16];
            int length = file.read(value);
            if (length > 0) {
                width = Integer.parseInt(new String(value).substring(0, length));
            }
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return width;
    }


    public int getHeight() {
        int height = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/height");
            byte[] value = new byte[16];
            int length = file.read(value);
            if (length > 0) {
                height = Integer.parseInt(new String(value).substring(0, length));
            }
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return height;
    }

    public char getScan() {
        char scan = '?';
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/rate");
            scan = (char) file.read();
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return Character.toLowerCase(scan);
    }

    public float getRate() {
        float rate = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/rate");
            byte[] value = new byte[16];
            int length = file.read(value);
            if (length > 1) {
                rate = Float.parseFloat(new String(value).substring(1, length));
            }
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return rate;
    }

    public static int getMode() {
        int mode = 0xff;
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/mode");
            byte[] value = new byte[16];
            int length = file.read(value);
            if (length > 0) {
                mode = Integer.parseInt(new String(value).substring(0, length));
            }
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return Character.toLowerCase(mode);
    }

     public int getStd() {
        int std = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/misc/tp28xx/std");
            byte[] value = new byte[16];
            int length = file.read(value);
            if (length > 0) {
                std = Integer.parseInt(new String(value).substring(0, length));
//                Log.e(TAG, "Get STD: " + std);
            }

            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return std;
    }
}
