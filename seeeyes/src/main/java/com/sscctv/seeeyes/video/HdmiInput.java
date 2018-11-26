package com.sscctv.seeeyes.video;

import android.media.CamcorderProfile;
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
 * Created by trlim on 2015. 12. 7..
 *
 * HDMI 입력을 다루는 클래스
 */
public class HdmiInput extends CameraInput {
    private static final String TAG = "HdmiInput";

    private final SysFsMonitor mSignalMonitor;
    //private boolean mForceNotifySignalChange = false;

    public HdmiInput(SurfaceView surfaceView, Listener listener) {
        super(VideoInput.VIDEO_INPUT_HDMI, surfaceView, listener);

        mSignalMonitor = new SysFsMonitor("/sys/class/mhl/sii9293/signal");

//        setMode(true);
        AnalogInput.disable();
        SdiInput.disable();
    }

    @Override
    protected boolean hasAudio() {
        return true;
    }

    @Override
    public void start(Bundle args) {
        super.start(args);

        enable();

        try {
            //mForceNotifySignalChange = true;

            mSignalMonitor.start(new SysFsMonitor.AttributeChangeHandler() {
                @Override
                public void onAttributeChange(RandomAccessFile file) {
                    byte[] value = new byte[16];
                    int length = 0;
                    try {
                        length = file.read(value);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if (length > 0) {
                        String signal = new String(value).substring(0, length);

                        if (!hasSignal()) {
//                            Log.d(TAG, "Completely No Signal");
                            signal = "nosignal";
                        }

                        switch (signal) {
                            case "nosignal":
                                Log.d(TAG, "No Signal");
                                notifySignalChange(false, 0, 0, '?', 0.0F, 0,253);
                                if(!isRecording()) {
                                    //stopPreview();
                                    stopCameraInput();
                                }

                                //mForceNotifySignalChange = false;
                                break;

                            case "changed":
                                Log.d(TAG, "signal = " + signal);

                                //if (getCamera() != null)    stopCameraInput();
                                startCameraInput();
                                startPreview();
                                notifySignalChange(true, getWidth(), getHeight(), getScan(), getRate(), 0,253);

                                //mForceNotifySignalChange = false;
                                break;

                            default:
                                //if (mForceNotifySignalChange) {
                                    notifySignalChange(true, getWidth(), getHeight(), getScan(), getRate(), 0,253);

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

    @Override
    public void stop() {
//        Log.d(TAG, "HDMI Mode Stop");

        try {
            mSignalMonitor.stop();
        } catch (IOException e) {
            e.printStackTrace();
        }

        disable();

        super.stop();
    }

    private static void enable() {
        setMode(true);
    }

    public static void disable() {
        setMode(false);
    }

    private static void setMode(boolean value) {
//        Log.d(TAG, "SetMode: " + value);
        try {
        //  FileOutputStream file = new FileOutputStream("/sys/class/mhl/sii9293/buffer");	// set buffer ic enable/disable pin
            FileOutputStream file = new FileOutputStream("/sys/class/mhl/sii9293/run");		// rx check on/off
            file.write((value ? "1" : "0").getBytes());
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean hasSignal() {
        boolean signal = false;
        try {
            FileInputStream file = new FileInputStream("/sys/class/mhl/sii9293/connection");
            signal = file.read() == '1';
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return signal;
    }

    public int getWidth() {
        int width = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/mhl/sii9293/width");
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

    @Override
    public int getHeight() {
        int height = 0;
        try {
            FileInputStream file = new FileInputStream("/sys/class/mhl/sii9293/height");
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
            FileInputStream file = new FileInputStream("/sys/class/mhl/sii9293/rate");
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
            FileInputStream file = new FileInputStream("/sys/class/mhl/sii9293/rate");
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

    @Override
    protected CamcorderProfile getCamcorderProfile() {
        return CamcorderProfile.get(getCameraId(), CamcorderProfile.QUALITY_HIGH);
    }
}
