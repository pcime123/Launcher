package com.sscctv.seeeyes;

import android.os.ParcelFileDescriptor;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;

/**
 * Created by trlim on 2016. 1. 18..
 *
 * sysfs attribute의 변화를 감지하기 위한 클래스
 */
public class SysFsMonitor {
    private final String mPath;

    private RandomAccessFile mFile;
    private ParcelFileDescriptor mFd;

    private ParcelFileDescriptor mReadPipe;
    private ParcelFileDescriptor mWritePipe;

    private Thread mMonitorThread;

    public interface AttributeChangeHandler {
        void onAttributeChange(RandomAccessFile file);
    }

    public SysFsMonitor(String path) {
        mPath = path;
    }

    public void start(AttributeChangeHandler handler) throws IOException {
        mFile = new RandomAccessFile(mPath, "r");
        mFd = ParcelFileDescriptor.dup(mFile.getFD());

        ParcelFileDescriptor[] pipes = ParcelFileDescriptor.createPipe();
        mReadPipe = pipes[0];
        mWritePipe = pipes[1];

        startMonitor(handler);
    }

    public void stop() throws IOException {
        try {
            stopMonitor();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (mWritePipe != null) {
            mWritePipe.close();
            mWritePipe = null;
        }
        if (mReadPipe != null) {
            mReadPipe.close();
            mReadPipe = null;
        }
        if (mFd != null) {
            mFd.close();
            mFd = null;
        }
        if (mFile != null) {
            mFile.close();
            mFile = null;
        }
    }

    private void startMonitor(final AttributeChangeHandler handler) {
        mMonitorThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int event = 0;

                while (event >= 0) {
                    event = native_poll(mReadPipe.getFd(), mFd.getFd(), 1000);

                    if ((event & 1) == 1) {
                        // pipe 이벤트가 발생. Thread를 종료.
                        break;
                    }
                    if ((event & 2) == 2) {
                        if (handler != null) {
                            handler.onAttributeChange(mFile);
                        }
                    }

                    try {
                        mFile.seek(0);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        mMonitorThread.start();
    }

    private void stopMonitor() throws IOException, InterruptedException {
        if (mWritePipe != null) {
            FileOutputStream pipe = new FileOutputStream(mWritePipe.getFileDescriptor());

            pipe.write(0);
        }

        if (mMonitorThread != null) {
            mMonitorThread.join();
            mMonitorThread = null;
        }
    }

    @SuppressWarnings("JniMissingFunction")
    private native int native_poll(int exitFd, int monitorFd, int timeout);

    static {
        try {
            System.loadLibrary("seeeyes-lib");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("WARNING: SysFsMonitor Could not load seeeyes library!");
        }
    }
}
