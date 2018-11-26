package com.sscctv.seeeyes.ptz;

import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by trlim on 2015. 12. 26..
 * <p>
 * PTZ 프로토콜 RX 처리기
 */
@SuppressWarnings("JniMissingFunction")
public class PtzAnalyzer extends PtzMode implements McuControl.OnReceiveBufferListener {
    private static final String TAG = "PtzAnalyzer";

    public interface OnReceivePacketHandler {
        void onReceivePacket(PtzAnalyzer analyzer, char ch, char key, byte[] bytes);
    }

    private int mProtocol;

    private OnReceivePacketHandler mPacketHandler;

    // JNI에서 씀
    @SuppressWarnings("unused")
    private long mContext;

    public PtzAnalyzer(McuControl control, int protocol, int baudRate) throws RuntimeException {
        super(control, baudRate);

        Log.d(TAG, "Protocol " + protocol + ", baudRate " + baudRate);

        mProtocol = protocol;

        native_init(protocol);
    }

    @Override
    protected void finalize() throws Throwable {
        native_exit();

        super.finalize();
    }

    @Override
    public void sendCommand(char command) {
        // 지원하지 않음
    }

    public int getProtocol() {
        return mProtocol;
    }

    public synchronized void setProtocol(int protocol) {
        Log.d(TAG, "setProtocol(" + protocol + ")");

        native_exit();

        mProtocol = protocol;
        native_init(protocol);
    }

    public static String buildCommandString(char command) {
        return PtzProtocol.commandToString(command);
    }

    public String buildPacketString(byte[] packet) {
        return PtzProtocol.packetToString(getProtocol(), packet);
    }

    public void start(OnReceivePacketHandler handler) {
        mPacketHandler = handler;

        mControl.addReceiveBufferListener(this);

        try {
            mControl.setPtzMode(PtzMode.ANALYZER);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void stop() {
        super.stop();

        mControl.removeReceiveBufferListener(this);

        mPacketHandler = null;
    }

    @Override
    public synchronized void onReceiveBuffer(ByteBuffer buffer, int length) throws InterruptedException {
        byte a = buffer.get();
        ByteBuffer _buffer = ByteBuffer.allocateDirect(1);
        _buffer.put(a);
//        Log.d(TAG, "Buffer: " + _buffer + " length: " + length);

        native_decode_data(_buffer, length);
    }

    /**
     * Native에서 유효한 하나의 패킷을 수신했을 때마다 호출한다.
     *
     * @param address 주소
     * @param command 키 코드. 항상 0보다 크며 PtzCommand에 정의되어 있다.
     * @param packet  패킷 데이터
     */
    @SuppressWarnings("unused")
    private void onReceivePacket(char address, char command, byte[] packet) {
        if (mPacketHandler != null) {
            mPacketHandler.onReceivePacket(this, address, command, packet);
        } else {
//            Log.d(TAG, "onReceivePacket(" + address + ", " + command + ", " + packet.length + ")");
        }
    }

    private native void native_init(int protocol) throws RuntimeException;

    private native int native_decode_data(ByteBuffer buffer, int length) throws InterruptedException;

    private native void native_exit();
}
