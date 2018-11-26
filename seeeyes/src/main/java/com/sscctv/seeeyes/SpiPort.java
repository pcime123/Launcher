package com.sscctv.seeeyes;

import android.util.Log;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class SpiPort {

    private static final String TAG = "SpiPort";
    private FileDescriptor mFd1;
    private FileInputStream mFileInputStream1;
    private FileOutputStream mFileOutputStream1;
    private File device = new File("/dev/gp22");

    public void open() {

//        portPermission();

        openSerialPort(device.getAbsolutePath());

        mFileInputStream1 = new FileInputStream(mFd1);
        mFileOutputStream1 = new FileOutputStream(mFd1);

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

    private void openSerialPort(String path) {
        Log.i(TAG, "Path = " + path );
        mFd1 = open(device.getAbsolutePath());
//        mSerialPort = new SerialPort(path);
        if(mFd1 == null) {
            Log.e(TAG, "native open returns null");
        }

    }




    private native static FileDescriptor open(String path);
    public native void close();
    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: Rs485Port Could not load seeeyes library!");
        }
    }

}
