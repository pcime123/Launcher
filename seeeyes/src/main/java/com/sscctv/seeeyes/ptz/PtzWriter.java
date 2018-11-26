package com.sscctv.seeeyes.ptz;

import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by trlim on 2015. 12. 26..
 *
 * PTZ 프로토콜 TX 처리기
 */
@SuppressWarnings("JniMissingFunction")
public class PtzWriter extends PtzMode {
    private static final String TAG = "PtzWriter";

    public static final int MODE_ZF = 0;
    public static final int MODE_PT = 1;
    public static final int MODE_MENU = 2;

    // JNI에서 씀
    @SuppressWarnings("unused")
    private long mContext;

    public PtzWriter(McuControl control, int protocol, char address, int baudRate) throws RuntimeException {
        super(control, baudRate);

        native_init(protocol, address);
    }

    @Override
    protected void finalize() throws Throwable {
        native_exit();

        super.finalize();
    }

    public void startTx() {
        try {
            mControl.setPtzMode(PtzMode.TX);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void startUcc() {
        try {
            mControl.setPtzMode(PtzMode.UCC);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void setMode(int mode) {
        native_set_mode(mode);
    }

    @Override
    public void sendCommand(char command) {
        Log.d(TAG, "sendCommand(" + (int) command + ")");

        try {
            sendCommandImpl(PtzProtocol.controlToCommand(command));
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void sendCommandImpl(char command) throws InterruptedException, IOException {
        // 최대 패킷 크기만큼의 버퍼를 할당한다.
        ByteBuffer buffer = ByteBuffer.allocateDirect(PtzProtocol.PACKET_SIZE_MAX);
        Log.d(TAG, "sendCommandImpl: " + buffer + " , " + command);
        // Native에 버퍼와 명령을 알려주면 패킷을 버퍼에 기록하고 길이를 반환한다.
        native_encode_command(buffer, command);
    }

    @SuppressWarnings("unused")
    private void sendBuffer(ByteBuffer buffer, int length, int rxTimeout) {
        //Log.d(TAG, "sendBuffer(" + length + ")");

        /*
        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < length; i++) {
            builder.append((char) buffer.get(i));
            builder.append(' ');
        }
        Log.d(TAG, builder.toString());
        */

        if (rxTimeout > 0) {
            /*
            rx1.recv_flag = 0;
            rx1.buff[0] = 0;
            */
        }

        try {
            mControl.sendRs485Packet(buffer, length);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }

        /* TODO
        cnt = 50;									// 두바이트 보내는 시간
        while(cnt && tx1.busy_flag){
            cnt--;
            delay_us(100);
        }
        */

        if (rxTimeout > 0) {
            // TODO 한 바이트라도 수신을 할 때까지 대기
        }
    }

    private native void native_init(int protocol, char address) throws RuntimeException;

    private native void native_set_mode(int mode);

    /**
     * 버퍼에 지정한 명령에 대한 패킷을 만들어 준다.
     *
     * @param buffer 패킷을 만들 ByteBuffer. ByteBuffer.allocateDirect로 할당한 것이어야 한다.
     * @param command 버퍼에 인코딩할 PTZ 명령 코드
     * @return 버퍼에 기록한 패킷 데이터의 길이
     * @throws IOException
     */
    private native int native_encode_command(ByteBuffer buffer, char command) throws IOException;

    private native void native_exit();
}
