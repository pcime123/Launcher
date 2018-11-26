package com.sscctv.seeeyes.ptz;

import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Created by trlim on 2016. 3. 2..
 *
 * MCU로부터의 명령을 받아 분석하는 일을 한다.
 */
public abstract class McuCommandListener implements McuControl.OnReceiveBufferListener {
    private byte[] _buf;
    private int _len;

    public McuCommandListener() {
        _buf = null;
        _len = 0;
    }

    @Override
    public void onReceiveBuffer(ByteBuffer buffer, int length) throws InterruptedException {
        byte[] buf = new byte[_len + length];
        if (_buf != null) {
            System.arraycopy(_buf, 0, buf, 0, _len);
        }
        System.arraycopy(buffer.array(), 0, buf, _len, length);
        _buf = buf;
        _len += length;

        int ptr = 0;

        while (_len >= 5) {
            if (_buf[ptr] != (byte) 0xA0) {
                ptr++;
                _len--;
                continue;
            }
            if (_buf[ptr+1] != (byte) 0x00) {
                ptr++;
                _len--;
                continue;
            }

            int cmd  = _buf[ptr+2];
            if (cmd < 0) {
                cmd += 256;
            }
            int data = _buf[ptr+3];
            if (data < 0) {
                data += 256;
            }
            int csum = _buf[ptr+4];
            if (csum < 0) {
                csum += 256;
            }

            if (csum == ((cmd + data) & 0xFF)) {
                onMcuCommand((char) cmd, (char) data);
//                Log.i("MCU", "onMcuCommand: " + cmd + " ," + data + " ," + csum);
            } else {
//                Log.e("MCU", "invalid cmd " + cmd + " ," + data + " ," + csum);
            }

            ptr += 5;
            _len -= 5;
        }

        if (ptr > 0) {
            _buf = Arrays.copyOfRange(_buf, ptr, ptr + _len);
        }
    }

    public abstract void onMcuCommand(char cmd, char data);
}
