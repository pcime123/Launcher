package com.sscctv.seeeyes;

import android.os.ParcelFileDescriptor;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class SerialPort {
    private static final String TAG = "SerialPort";
    private int mNativeContext;
    private final String mName;
    private ParcelFileDescriptor mFileDescriptor;

    public SerialPort(String name) {
        this.mName = name;
    }

    public void open(ParcelFileDescriptor pfd, int speed, int timeout) throws IOException {
        this.native_open(pfd.getFileDescriptor(), speed, timeout);
        this.mFileDescriptor = pfd;
    }

    public void close() throws IOException {
        if (this.mFileDescriptor != null) {
            this.mFileDescriptor.close();
            this.mFileDescriptor = null;
        }

        this.native_close();
    }

    public String getName() {
        return this.mName;
    }

    public void setSpeed(int speed) throws IOException {
        this.native_set_speed(speed);
    }

    public int read(ByteBuffer buffer) throws IOException {
        if (buffer.isDirect()) {
            return this.native_read_direct(buffer, buffer.remaining());
        } else if (buffer.hasArray()) {
            return this.native_read_array(buffer.array(), buffer.remaining());
        } else {
            throw new IllegalArgumentException("buffer is not direct and has no array");
        }
    }

    public void write(ByteBuffer buffer, int length) throws IOException {
        if (buffer.isDirect()) {
            this.native_write_direct(buffer, length);
        } else {
            if (!buffer.hasArray()) {
                throw new IllegalArgumentException("buffer is not direct and has no array");
            }

            this.native_write_array(buffer.array(), length);
        }

    }

    public FileInputStream getInputStream() {
        return new ParcelFileDescriptor.AutoCloseInputStream(this.mFileDescriptor);
    }

    public FileOutputStream getOutputStream() {
        return new ParcelFileDescriptor.AutoCloseOutputStream(this.mFileDescriptor);
    }

    private native void native_open(FileDescriptor var1, int var2, int var3) throws IOException;

    private native void native_close();

    private native void native_set_speed(int var1);

    private native int native_read_array(byte[] var1, int var2) throws IOException;

    private native int native_read_direct(ByteBuffer var1, int var2) throws IOException;

    private native void native_write_array(byte[] var1, int var2) throws IOException;

    private native void native_write_direct(ByteBuffer var1, int var2) throws IOException;

    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError var1) {
            System.err.println("WARNING: Could not load boardservice library!");
        }

    }
}
