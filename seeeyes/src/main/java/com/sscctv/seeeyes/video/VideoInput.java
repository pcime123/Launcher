package com.sscctv.seeeyes.video;

import android.hardware.Camera;
import android.media.CamcorderProfile;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by trlim on 2015. 12. 7..
 * <p>
 * 비디오 입력 클래스를 위한 기반 클래스
 */
public abstract class VideoInput implements SurfaceHolder.Callback {
    private static final String TAG = "VideoInput";

    public final static int VIDEO_INPUT_NONE = -1;
    public final static int VIDEO_INPUT_HDMI = 0;
    public final static int VIDEO_INPUT_SDI = 0;
    public final static int VIDEO_INPUT_AUTO = 0;

    private static final int SYSTEM_UNKNOWN = -1;
    private static final int SYSTEM_NTSC = 0;
    private static final int SYSTEM_PAL = 1;

    private final int _input;
    public static int _value = -1;

    private final SurfaceHolder _surfaceHolder;

    public static class SignalInfo {
        public final boolean signal;
        public final int width;
        public final int height;
        public final char scan;
        public final float rate;
        public final int mode;
        public final int std;

        public SignalInfo() {
            this.signal = false;
            this.width = 0;
            this.height = 0;
            this.scan = '?';
            this.rate = 0.0f;
            this.mode = 0;
            this.std = 0;
        }

        SignalInfo(boolean signal, int width, int height, char scan, float rate, int mode, int std) {
            this.signal = signal;
            this.width = width;
            this.height = height;
            this.scan = scan;
            this.rate = rate;
            this.mode = mode;
            this.std = std;
            Log.d(TAG, "Signal: " + signal + " Width: " + width + " height: " +
                    height + " scan: " + scan + " rate: " + rate + " mode: " + mode + " std: " + std);


        }

    }


    public static int getSystem(SignalInfo signalInfo) {
        // NTSC/PAL을 해상도로 구분한다.
        if (signalInfo.signal && signalInfo.width == 1920 && signalInfo.scan == 'i') {
            if (signalInfo.height == 480) {
                return SYSTEM_NTSC;
            } else if (signalInfo.height == 576) {
                return SYSTEM_PAL;
            }
        }
        return SYSTEM_UNKNOWN;
    }

//    private int getSystem() {
//        // signal, width, height, scan만 중요
//        return getSystem(new SignalInfo(true, getWidth(), getHeight(), 'i', 0.0f, 0,0));
//    }

//
//    protected CamcorderProfile getCamcorderProfile() {
//        CamcorderProfile profile = CamcorderProfile.get(CamcorderProfile.QUALITY_480P);
//
//        switch (getSystem()) {
//            case SYSTEM_NTSC:
////                profile.videoFrameWidth = 640;
//                profile.videoFrameWidth = 720;
//                profile.videoFrameHeight = 480;
//                break;
//
//            case SYSTEM_PAL:
////                profile.videoFrameWidth = 704;
//                profile.videoFrameHeight = 576;
//                profile.videoFrameWidth = 720;
//                break;
//        }
//
//        return profile;
//    }


    public interface Listener {
        void onSignalChange(SignalInfo signalInfo);
    }

    private final Listener _listner;

    VideoInput(int input, SurfaceView surfaceView, Listener listener) {
        _input = input;
        _surfaceHolder = surfaceView.getHolder();

        _surfaceHolder.addCallback(this);

        _listner = listener;
    }

    public int getInput() {

        return _input;
    }

    SurfaceHolder getSurfaceHolder() {
//        Log.i(TAG, "getSurfaceHolder" + _surfaceHolder);
        return _surfaceHolder;
    }

    private void removeSurfaceHolder() {
//        Log.i(TAG, "removeSurfaceHolder");
        _surfaceHolder.removeCallback(this);
    }

    public abstract void start(Bundle args);

    public abstract void stop();

    public void restart() {
    }

    public abstract void startCameraInput();

    public abstract void stopCameraInput();

    public abstract void startPreview();

    public abstract void stopPreview();

    public interface SnapshotCallback {
        int SNAPSHOT_RAW = 1;
        int SNAPSHOT_POSTVIEW = 2;
        int SNAPSHOT_JPEG = 3;

        void onShutter();

        void onSnapshotTaken(int type, byte[] data, VideoInput videoInput);
    }

    public abstract void takeSnapshot(SnapshotCallback callback);

    public abstract boolean isRecording();

    public abstract boolean startRecording(String path);

    public abstract String stopRecording(boolean stat);

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
//        Log.i(TAG, "Surface Created");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
//        Log.i(TAG, "Surface Destroyed");
        removeSurfaceHolder();
    }

    void notifySignalChange(boolean signal, int width, int height, char scan, float rate, int mode, int std) {
        if (_listner != null) {
            _listner.onSignalChange(new SignalInfo(signal, width, height, scan, rate, mode, std));
        }
    }
}
