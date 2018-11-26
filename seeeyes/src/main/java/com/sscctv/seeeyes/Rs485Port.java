package com.sscctv.seeeyes;

import android.os.ParcelFileDescriptor;
import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.ClosedByInterruptException;

/**
 * Created by trlim on 2015. 12. 28..
 *
 * RS-485 통신을 위한 API
 */
@SuppressWarnings("JniMissingFunction")
public class Rs485Port {

    private static final String TAG = "Rs485Port";
    private FileDescriptor mFd;
    private FileInputStream mFileInputStream;
    private FileOutputStream mFileOutputStream;
    private File device = new File("/dev/ttyS2");


    public void open(int baudRate) {

        portPermission();

        openSerialPort(device.getAbsolutePath(), baudRate);


        mFileInputStream = new FileInputStream(mFd);
        mFileOutputStream = new FileOutputStream(mFd);

    }

    private void openSerialPort(String path, int baudRate) {
        Log.i(TAG, "Path = " + path + " Baudrate = " + baudRate);
        mFd = open(device.getAbsolutePath(), baudRate, 0);
//        mSerialPort = new SerialPort(path);
        if(mFd == null) {
            Log.e(TAG, "native open returns null");
        }

    }

    private void portPermission () {
       if (!device.canRead() || !device.canWrite()) {
            try {
                /* Missing read/write permission, trying to chmod the file */
                Process su;
                su = Runtime.getRuntime().exec("/system/bin/su");
                String cmd = "chmod 777 " + device.getAbsolutePath() + "\n"
                        + "exit\n";
                su.getOutputStream().write(cmd.getBytes());
                if ((su.waitFor() != 0) || !device.canRead()
                        || !device.canWrite()) {
                    throw new SecurityException();
                }
            } catch (Exception e) {
                e.printStackTrace();
                throw new SecurityException();
            }
        }
    }

    public InputStream getInputStream() {
        return mFileInputStream;
    }

    public OutputStream getOutputStream() {
        return mFileOutputStream;
    }


    public void closePort() throws IOException {
        close();
//        stopReader();
    }



    private native static FileDescriptor open(String path, int baudrate, int flags);
    public native void close();
    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: Rs485Port Could not load seeeyes library!");
        }
    }

}
