package com.sscctv.seeeyes.ptz;

import android.os.ParcelFileDescriptor;

import com.sscctv.seeeyes.video.AnalogInput;

import java.io.IOException;

/**
 * Created by trlim on 2016. 2. 24..
 *
 * UTC 모드 제어를 구현
 */
@SuppressWarnings("JniMissingFunction")
public class UtcWriter implements PtzControl {
    //private static final String TAG = "UtcWriter";

    private final int mProtocol;
    private final int mAddress;
    private final int mUtcType;
    private ParcelFileDescriptor mFileDescriptor;

    public UtcWriter(int address, int protocol, int utcType) {
        mProtocol = protocol;
        mAddress = address;
        mUtcType = utcType;
    }

    //private boolean isCvbs() {
    //    return mUtcType == UtcProtocol.TYPE_CVBS;
    //}

    //private boolean isAhd() {
    //    return mUtcType == UtcProtocol.TYPE_AHD;
    //}

    public void start() {
        mFileDescriptor = native_open("/dev/tp28xx");

    //    if (isCvbs()) {
    //        try {
    //            mMcuControl.setUtcProtocol(mProtocol);
    //       } catch (IOException | InterruptedException e) {
    //            e.printStackTrace();
    //        }
    //    }
    }

    @Override
    public void stop() {
        try {
            mFileDescriptor.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        mFileDescriptor = null;
    }

    @Override
    public void sendCommand(char command) {
//        Log.d(TAG, "sendCommand(" + (int) command + ")");

        sendIoctlKeyCommand(command);
    }
/*
    private void sendMcuKeyCommand(char command) {
        try {
            mMcuControl.sendCoaxKey(PtzProtocol.controlToCommand(command));
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
*/
    private void sendIoctlKeyCommand(char command) {
        int protocol = mProtocol;

        switch (mUtcType) {
            case UtcProtocol.TYPE_CVBS:
                protocol = UtcProtocol.AHD_A_CP_CVBS;
                break;

            case UtcProtocol.TYPE_TVI:
                break;

            case UtcProtocol.TYPE_AHD:
                protocol = UtcProtocol.AHD_A_CP;
                //protocol = UtcProtocol.AHD_A_CP_720P;
                break;

            case UtcProtocol.TYPE_CVI:
                protocol += 20;
                break;
        }

        try {
            native_send_command(mFileDescriptor.getFd(), AnalogInput.getMode(), mAddress, protocol, command);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private native ParcelFileDescriptor native_open(String path);
    private native int native_send_command(int fd, int mode, int address, int protocol, char command) throws IOException;

    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: UtcWriter ]Could not load seeeyes library!");
        }
    }
}
