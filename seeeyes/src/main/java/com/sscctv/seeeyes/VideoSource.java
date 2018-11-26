package com.sscctv.seeeyes;

import android.os.ParcelFileDescriptor;
import android.util.Log;

//import com.iotok.android.board.service.lib.BoardManager;
//import com.iotok.android.board.service.lib.GpioPin;

import com.sscctv.seeeyes.video.VideoInput;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by trlim on 2016. 2. 25..
 * <p>
 * Allows Apps to notify its use of specific video input device.
 * Every video input apps must call methods of this API in their onResume/onPause handlers
 * for proper operation.
 */
@SuppressWarnings("JniMissingFunction")
public class VideoSource {

    public static final String SDI = "SDI";
    public static final String HDMI = "HDMI";
    public static final String AUTO = "AUTO";

    public static final String AHD = "AHD";
    public static final String TVI = "TVI";
    public static final String CVI = "CVI";
    public static final String CVBS = "CVBS";
    public static final String LOSS = "LOSS";

    public static final String IPC = "IPC";

    private String _sourceId;
    private String videoType;
    private ParcelFileDescriptor dispFileDescriptor;



//    private static final int VP_CTRL = 167 + 6;
//    private static final int PSE_EN = 167 + 8;
//    private static final int PD_GOOD = 167 + 13;
//    private static final int ADC_SEL = 167 + 26;

//    private static boolean pd_Status = false;


//    private GpioPin _vpCtrlPin;
//    private GpioPin _pseEnPin;
//    private GpioPin _pdGoodPin;
//    private GpioPin _adcSelPin;



    public VideoSource(final String sourceId) {
            _sourceId = sourceId;
    }

    public String getType() {
        return videoType;
    }

    public String getSourceId() {
        return _sourceId;
    }

    public boolean is(final String sourceId) {
        return _sourceId.equals(sourceId);
    }


    public void setCvbsOut(boolean tv_out) {
//        Log.d(TAG, "sendCommand(" + (int) command + ")");

        sendCvbsOutIoctlKeyCommand(tv_out);
    }

    private void sendCvbsOutIoctlKeyCommand(boolean tv_out) {
        try {
            native_display_cvbs_out(dispFileDescriptor.getFd(), tv_out);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private native ParcelFileDescriptor native_display_open(String path);

    private native int native_display_cvbs_out(int fd, boolean tv_out) throws IOException;

    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: VideoSource Could not load seeeyes library!");
        }
    }

}
