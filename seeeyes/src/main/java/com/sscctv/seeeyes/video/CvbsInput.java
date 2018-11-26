package com.sscctv.seeeyes.video;

import android.media.CamcorderProfile;
import android.view.SurfaceView;

/**
 * Created by trlim on 2015. 12. 7..
 *
 * CVBS 입력을 다루는 클래스
 */
public class CvbsInput
//        extends AnalogInput
{
    public static final int SYSTEM_UNKNOWN = -1;
    public static final int SYSTEM_NTSC = 0;
    public static final int SYSTEM_PAL = 1;

//    public CvbsInput(SurfaceView surfaceView, Listener listener) {
//        super(VideoInput.VIDEO_INPUT_CVBS, surfaceView, listener);
//    }

//    public static int getSystem(SignalInfo signalInfo) {
//        // NTSC/PAL을 해상도로 구분한다.
//        if (signalInfo.signal && signalInfo.width == 1920 && signalInfo.scan == 'i') {
//            if (signalInfo.height == 480) {
//                return SYSTEM_NTSC;
//            } else if (signalInfo.height == 576) {
//                return SYSTEM_PAL;
//            }
//        }
//        return SYSTEM_UNKNOWN;
//    }
//
//    private int getSystem() {
//        // signal, width, height, scan만 중요
//        return getSystem(new SignalInfo(true, getWidth(), getHeight(), 'i', 0.0f, 0,0));
//    }
//
//    @Override
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
}
