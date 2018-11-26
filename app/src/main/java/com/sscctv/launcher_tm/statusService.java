package com.sscctv.launcher_tm;

import android.annotation.SuppressLint;
import android.app.IntentService;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

import com.sscctv.seeeyes.VideoSource;
import com.sscctv.seeeyes.ptz.LevelMeterListener;
import com.sscctv.seeeyes.ptz.McuControl;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.lang.String.format;

public class statusService extends IntentService {

    private static final String TAG = "statusService";
    public static int iFOREGROUND_SERVICE = 101;
    private String[] array = new String[23];
    private StringBuilder str = new StringBuilder(2);

    private Timer timer;
    private NotificationManager nm;
    private NotificationManager nm1;
    private NotificationManager nm2;
    private boolean state1 = false;
    private boolean state2 = false;
    private boolean state4 = false;

    public statusService() {
        super("PoEIntentService");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate() ");
        getStatus();


    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

//        Log.d(TAG, "Start service in foreground ");
//        sendNotification();
//        Log.d(TAG, "Timer? " + timer);
        if(timer == null){
            timer = new Timer();
            timer.schedule(timerTask, 100, 1000);
        }
        state1 = false;
        state2 = false;

        return START_STICKY;
    }

//    public void startCheck() {
//
//    }

    public void stopCheck() {
        Log.d(TAG, "Stop Service");
        if (timer != null) timer.cancel();
        timer = null;
    }

    TimerTask timerTask = new TimerTask() {
        @Override
        public void run() {
            getStatus();
            port1_check();
            port2_check();
            port4_check();

        }
    };
    // | Not Use |   CAM   |    NVR    |   Not Use   |     SFP     |   Not Use   |
    // | PORT 00 | PORT 01 |  PORT 02  |   PORT 03   |   PORT 04   |   PORT 05   |
    // | 0 1 0 0 | 0 1 0 0 | 0 1 0  0  | 0  1  0  0  | 0  1  0  0  | 0  1  0  0  |
    // | 0 1 2 3 | 4 5 6 7 | 8 9 10 11 | 12 13 14 15 | 16 17 18 19 | 20 21 22 23 |

    private String getPort1Stat() {
        String value;
        value = array[4];
        return value;
    }

    private String getPort2Stat() {
        String value;
        value = array[8];
        return value;
    }

    private String getPort4Stat() {
        String value;
        value = array[16];
        return value;
    }

    private void port1_check() {
        Resources res = getResources();
//        Log.d(TAG, "Port 1: " + getPort1Stat() + " State: " + state1);

        if (getPort1Stat().equals("1") && !state1) {

            state1 = true;

            NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
            builder.setContentTitle("Link In CAM Port")
                    .setContentText("Connected to CAM Port")
                    .setTicker("Connected to CAM Port")
                    .setSmallIcon(R.drawable.eth_noti)
                    .setLargeIcon(BitmapFactory.decodeResource(res, R.drawable.eth_large))
                    .setAutoCancel(true)
                    .setWhen(System.currentTimeMillis());
//                    .setDefaults(Notification.DEFAULT_ALL);

//
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                builder.setCategory(Notification.CATEGORY_MESSAGE)
                        .setPriority(Notification.PRIORITY_HIGH)
                        .setVisibility(Notification.VISIBILITY_PUBLIC);
            }

            nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            nm.notify(8000, builder.build());

//            }
        } else if (getPort1Stat().equals("0") && state1) {
            state1 = false;

            if(nm != null) nm.cancel(8000);

        }

    }

    private void port2_check() {
        Resources res = getResources();
//        Log.d(TAG, "Port 2: " + getPort2Stat() + " State: " + state2);

        if (getPort2Stat().equals("1") && !state2) {
            state2 = true;

            NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
            builder.setContentTitle("Link In NET Port")
                    .setContentText("Connected to NET Port")
                    .setTicker("Connected to NET Port")
                    .setSmallIcon(R.drawable.nvr_noti)
                    .setLargeIcon(BitmapFactory.decodeResource(res, R.drawable.net_large))
                    .setAutoCancel(true)
                    .setWhen(System.currentTimeMillis());
//                    .setDefaults(Notification.DEFAULT_ALL);

//
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                builder.setCategory(Notification.CATEGORY_MESSAGE)
                        .setPriority(Notification.PRIORITY_HIGH)
                        .setVisibility(Notification.VISIBILITY_PUBLIC);
            }

            nm1 = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            nm1.notify(9000, builder.build());

//            }
        } else if (getPort2Stat().equals("0") && state2) {
            state2 = false;

            if(nm1 != null) nm1.cancel(9000);

        }

    }

    private void port4_check() {
        Resources res = getResources();
//        Log.d(TAG, "Port 4: " + getPort4Stat() + " State: " + state4);

        if (getPort4Stat().equals("1") && !state4) {
            state4 = true;

            NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
            builder.setContentTitle("Link In SFP Port")
                    .setContentText("Connected to SFP Port")
                    .setTicker("Connected to SFP Port")
                    .setSmallIcon(R.drawable.sfp_noti)
                    .setLargeIcon(BitmapFactory.decodeResource(res, R.drawable.sfp_large))
                    .setAutoCancel(true)
                    .setWhen(System.currentTimeMillis());
//                    .setDefaults(Notification.DEFAULT_ALL);

//
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                builder.setCategory(Notification.CATEGORY_MESSAGE)
                        .setPriority(Notification.PRIORITY_HIGH)
                        .setVisibility(Notification.VISIBILITY_PUBLIC);
            }

            nm2 = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
            nm2.notify(10000, builder.build());

//            }
        } else if (getPort4Stat().equals("0") && state4) {
            state4 = false;

            if(nm2 != null) nm2.cancel(10000);

        }

    }



    public String getStatus() {
        String sValue = "";
        try {
            Process p = Runtime.getRuntime().exec("cat /sys/class/misc/mv88e6176/port");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            sValue = input.readLine();
            input.close();

            array = sValue.split(" ");

            for (String test : array) {
                int i = 0;
            }


        } catch (IOException e) {
            e.printStackTrace();
        }
        return sValue;
    }

    private static String getEthernetIPAddress() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("ifconfig eth0");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));

            String sLine;
            while ((sLine = input.readLine()) != null) {
                if (sLine.contains("eth0:")) {
                    Pattern pIPAddress = Pattern.compile("ip (.+?) ");
                    Matcher matcher = pIPAddress.matcher(sLine);
                    if (matcher.find()) {
                        sValue = matcher.group(1);
                        break;
                    }
                }
            }
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return sValue;
    }


    @Override
    public void onDestroy() {
        stopCheck();
        super.onDestroy();
    }

    @Override
    protected void onHandleIntent(Intent intent) {
    }

}
