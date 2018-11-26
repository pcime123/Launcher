package com.sscctv.seeeyes.ptz;

import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by trlim on 2016. 1. 7..
 *
 * PTZ 프로토콜 RX 처리기
 */
public class PtzReader extends PtzMode implements McuControl.OnReceiveBufferListener {
    private static final String TAG = "PtzReader";

    public interface OnReceiveDataHandler {
        void onReceiveBuffer(PtzReader reader, byte[] bytes);
    }

    private OnReceiveDataHandler mDataHandler;

    public PtzReader(McuControl control, int baudRate) {
        super(control, baudRate);
    }

    public static String buildHexString(byte[] bytes) {
        return PtzProtocol.bytesToHex(bytes);
    }

    @Override
    public void sendCommand(char command) {
        // 지원하지 않음
    }
    public void start(OnReceiveDataHandler handler) {
        mDataHandler = handler;

        mControl.addReceiveBufferListener(this);

        try {
            mControl.setPtzMode(PtzMode.RX);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {
        super.stop();

        mControl.removeReceiveBufferListener(this);

        mDataHandler = null;
    }

    @Override
    public void onReceiveBuffer(ByteBuffer buffer, int length) throws InterruptedException {
        byte[] bytes = new byte[length];

//        byte a = buffer.get();
//        byte b = buffer.get();
//        byte c = buffer.get();
//        byte d = buffer.get();
//        byte e = buffer.get();
//                                Log.d(TAG, "1: " + a + " 2: " + b + " 3: " + c + " 4: " + d + " 5: " + e);

        if (mDataHandler != null) {
            buffer.get(bytes, 0, length);

            mDataHandler.onReceiveBuffer(this, bytes);
//            Log.d(TAG, "onReceiveBuffer(" + bytes.length + ")");

        } else {
            Log.d(TAG, "onReceiveBuffer(" + bytes.length + ")");
        }
    }
}
