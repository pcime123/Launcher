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
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.sscctv.seeeyes.VideoSource;
import com.sscctv.seeeyes.ptz.LevelMeterListener;
import com.sscctv.seeeyes.ptz.McuControl;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;

import static com.sscctv.seeeyes.VideoSource.IPC;
import static java.lang.String.format;

public class PoEIntentService extends IntentService {
    static final String sLogTag = "PoeService";
    public static final String sAction = "net.biyee.poe.action";
    private final String sBroadcast = "net.biyee.onviferenterprise.PlayVideoActivity";
    private McuControl mMcuControl;
    private VideoSource mSource;
    private LevelMeterListener mLevelMeterListener;
    BroadcastReceiver mOEBroadcastReceiver;
    BroadcastReceiver mAppBroadcastReceiver;
    private NotificationManager nm;
    private BroadcastReceiver closeReceiver = null;
    private BroadcastReceiver viewerReceiver = null;
    private BroadcastReceiver controlReceiver = null;
    private static TextView mPoeLevel, mFocusLevel, mFocusPoeLevel;

    private View mPoeView, mFocusPoeView;
    private View.OnTouchListener mViewTouchListener;
    private WindowManager mManager, sManager;
    private WindowManager.LayoutParams mParams, sParams;
    private float mTouchX, mTouchY;
    private int mViewX, mViewY;
    private static int mValue, sValue, mFocus;
    private boolean isMove = false;

    private int sideValue;

    private boolean isRunning, isExit;
    private boolean screenState;
    private static boolean pseLevel;
    private static boolean voltInput;
    private static boolean updateFlag;
    public static int iFOREGROUND_SERVICE = 101;
    private DataOutputStream opt;
    private static final String TAG = "PoE Service";
    public static Context mContext;

    public PoEIntentService() {
        super("PoEIntentService");
    }
//
//    private PoEHandler handler = new PoEHandler(this);
//
//    private static final class PoEHandler extends Handler {
//        private final WeakReference<PoEIntentService> ref;
//
//        public PoEHandler(PoEIntentService act) {
//            ref = new WeakReference<>(act);
//        }
//
//        @Override
//        public void handleMessage(Message msg) {
//            PoEIntentService act = ref.get();
//            if(act != null) {
//                if (updateFlag) {
//                    updateFlag = false;
//                    if (pseLevel) {
//                        mPoeLevel.setText("PoE 48V OUT");
//                    } else {
//                        mPoeLevel.setText("PoE OFF");
//                    }
//                }
//                if (voltInput) {
//                    mPoeLevel.setText(format("PoE : %02d.%02d V", mValue, sValue));
//                }
//            }
//        }
//
//        private static final Runnable runnable = () -> {
//        };
//    }


    @Override
    public void onCreate() {
        super.onCreate();
        mContext = this;
        Log.d(TAG, "onCreate() ");
        poeHandler.sendEmptyMessage(0);
        poeFocusHandlers.sendEmptyMessage(0);

        gpioPortSet();
        LayoutInflater poe_Inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        assert poe_Inflater != null;
        mPoeView = poe_Inflater.inflate(R.layout.poe_on_view, null);
        mPoeLevel = mPoeView.findViewById(R.id.mp_poe_level);

        mParams = new WindowManager.LayoutParams(WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT, 0, 0,
                WindowManager.LayoutParams.TYPE_PHONE,
                (WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE),
                PixelFormat.TRANSLUCENT);
        mParams.gravity = Gravity.TOP | Gravity.CENTER;
        mManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        assert mManager != null;
        mManager.addView(mPoeView, mParams);
        mPoeView.setVisibility(View.INVISIBLE);

        LayoutInflater mInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        assert mInflater != null;
        mFocusPoeView = mInflater.inflate(R.layout.poe_foucs_view, null);
        mFocusPoeLevel = mFocusPoeView.findViewById(R.id.focus_poe_level);
        sParams = new WindowManager.LayoutParams(WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT, 50, 20,
                WindowManager.LayoutParams.TYPE_PHONE,
                (WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE),
                PixelFormat.TRANSLUCENT);
        sParams.gravity = Gravity.BOTTOM | Gravity.START;
        sManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        assert sManager != null;
        sManager.addView(mFocusPoeView, sParams);
        mFocusPoeView.setVisibility(View.INVISIBLE);

//        nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

//        handler.postDelayed(null, 0);
    }

    private final poe_handler poeHandler = new poe_handler(this);

    private static class poe_handler extends Handler {
        private final WeakReference<PoEIntentService> ref;

        private poe_handler(PoEIntentService test) {
            ref = new WeakReference<>(test);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            PoEIntentService poEIntentService = ref.get();
            if (poEIntentService != null) {
//                Log.d(TAG, "poe_handler: " + updateFlag);

                if (updateFlag) {
                    updateFlag = false;
                    if (pseLevel) {
                        mPoeLevel.setText("PoE 48V OUT");
                    } else {
                        mPoeLevel.setText("PoE OFF");
                    }
                }
                if (voltInput) {
                    mPoeLevel.setText(format("PoE : %02d.%02d V", mValue, sValue));
                }
            }
        }

    }

    private final poe_focus_handler poeFocusHandlers = new poe_focus_handler(this);

    private static class poe_focus_handler extends Handler {
        private final WeakReference<PoEIntentService> ref;

        private poe_focus_handler(PoEIntentService test) {
            ref = new WeakReference<>(test);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            PoEIntentService poEIntentService = ref.get();
            if (poEIntentService != null) {
//                Log.d(TAG, "poe_focus_handler: " + updateFlag);
                if (updateFlag) {
                    updateFlag = false;
                    if (pseLevel) {
                        mFocusPoeLevel.setText("PoE : 48V OUT");
                    } else {
                        mFocusPoeLevel.setText("PoE : OFF");
                    }
                }
                if (voltInput) {
                    mFocusPoeLevel.setText(format("PoE : %02d.%02d V", mValue, sValue));
                }
            }
        }

    }
//    @SuppressLint("HandlerLeak")
//    final Handler poe_handler = new Handler() {
//        public void handleMessage(Message msg) {
////            Log.d(TAG,  "Not Full Screen updateFlag = " + updateFlag);
//            if (updateFlag) {
//                updateFlag = false;
//                if (pseLevel) {
//                    mPoeLevel.setText("PoE 48V OUT");
//                } else {
//                    mPoeLevel.setText("PoE OFF");
//                }
//            }
//            if (voltInput) {
//                mPoeLevel.setText(format("PoE : %02d.%02d V", mValue, sValue));
//            }
//        }
//    };

//    @SuppressLint("HandlerLeak")
//    final Handler poe_focus_handler = new Handler() {
//        public void handleMessage(Message msg) {
////            Log.d(TAG, "Full Screen updateFlag = " + updateFlag);
//            if (updateFlag) {
//                updateFlag = false;
//                if (pseLevel) {
//                    mFocusPoeLevel.setText("PoE : 48V OUT");
//                } else {
//                    mFocusPoeLevel.setText("PoE : OFF");
//                }
//            }
//            if (voltInput) {
//                mFocusPoeLevel.setText(format("PoE : %02d.%02d V", mValue, sValue));
//            }
//        }
//    };


    private void gpioPortSet() {
        try {
            Runtime command = Runtime.getRuntime();
            Process proc;

            proc = command.exec("su");
            opt = new DataOutputStream(proc.getOutputStream());
        } catch (Exception e) {
            e.printStackTrace();
            throw new SecurityException();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Start PoE Service");
        if (mMcuControl == null) {
            mMcuControl = new McuControl();
            mMcuControl.start(VideoSource.IPC);
        }
//        sendNotification();
        //if (mMcuControl == null) {
//        startWatchingOEClose();
        startWatchingViewerClose();
        startPoEViewCotrol();
        startWatchingOEBroadcast();

        isExit = false;
        updateFlag = true;
        pseLevel = false;
        voltInput = false;
        screenState = false;
//                levelUpdate = true;
//        isRunning = true;

        pollPoE();

        mPoeView.setVisibility(View.VISIBLE);
        mPoeViewTouchHandler();


        try {
            mMcuControl.startPoeCheck();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }


        return START_STICKY;
    }


    @Override
    public void onDestroy() {
//        isRunning = false;
        isExit = false;
        Log.i(TAG, "PoeService Destroy");
//        handler.removeCallbacks(null);
        poeHandler.removeCallbacks(null);
        poeFocusHandlers.removeCallbacks(null);
        stopWatchingOEBroadcast();
//        stopWatchingOEClose();
        stopWatchingViewerClose();
        stopPoEViewCotrol();
        stopPoeCheck();
        mMcuControl.stop();
        mMcuControl.removeReceiveBufferListener(mLevelMeterListener);
        mMcuControl = null;

//        Log.d(TAG, "Notification isRunning: " + isRunning + " isExit: " + isExit);

//        if (!isExit && !isRunning) {
//            exitNotification();
//        }

//        Toast.makeText(this, "PoE Service Destroy", Toast.LENGTH_SHORT).show();
        if (mPoeView != null) {
            mManager.removeView(mPoeView);
            mPoeView = null;
        }
        if (mFocusPoeView != null) {
            mManager.removeView(mFocusPoeView);
            mFocusPoeView = null;
        }
        super.onDestroy();
    }

    @Override
    protected void onHandleIntent(Intent intent) {
    }

    public void startWatchingOEBroadcast() {
        if (mOEBroadcastReceiver == null) {
            mOEBroadcastReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String state = intent.getStringExtra("state");
                    Log.d(TAG, "startWatchingOEBroadcast BroadCast: " + state);
                    String focus = intent.getStringExtra("focus");

                    if (state != null && state.equals("resume")) {
                        screenState = true;
                        mPoeView.setVisibility(View.INVISIBLE);
                        mFocusPoeView.setVisibility(View.VISIBLE);
//                        if (mMcuControl == null) {
//                            mMcuControl = new McuControl();
//                            mMcuControl.start(VideoSource.IPC);
//
//                            pollPoE();
//                        }
                        updateFlag = true;
                    Log.d(TAG, "Resume pfLevel = " + screenState + " value: " + levelUpdate);
                    }
                    if (state != null && state.equals("pause")) {
                        screenState = false;
                        mFocusPoeView.setVisibility(View.INVISIBLE);
                        mPoeView.setVisibility(View.VISIBLE);
                        try {
                            mMcuControl.startPoeCheck();
                        } catch (IOException | InterruptedException e) {
                            e.printStackTrace();
                        }
                        updateFlag = true;
                    Log.d(TAG, "Pause pfLevel " + screenState);
                    }

                }
            };

            IntentFilter filter = new IntentFilter();
            filter.addAction(sBroadcast);
            registerReceiver(mOEBroadcastReceiver, filter);
        }
    }

    public void stopWatchingOEBroadcast() {
        if (mOEBroadcastReceiver != null) {
            unregisterReceiver(mOEBroadcastReceiver);
        }
    }

    public void startWatchingViewerClose() {
//        Log.d(TAG, "startWatchingOEClose()");
        if (viewerReceiver == null) {
            viewerReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String state = intent.getStringExtra("state");
//                    Log.v(TAG, "startWatchingViewerClose Broadcast : " + state);
                    switch (state) {
                        case "resume":
//                            mMcuControl.controlStop();
                            if (mMcuControl != null) {
                                stopPoeCheck();

                                mMcuControl.removeReceiveBufferListener(mLevelMeterListener);
                                mMcuControl.stop();
                                mMcuControl = null;
                            }
                            if (getPseState().equals("1")) {
                                mPoeLevel.setText("PoE 48V OUT");
                            } else {
                                mPoeLevel.setText("PoE OFF");
                            }
//                        Log.w(TAG, "PoE Volt Check OFF");
                            break;
                        case "pause":
//                            mMcuControl.controlStart();
                            if (mMcuControl == null) {
                                mMcuControl = new McuControl();
                                mMcuControl.start(VideoSource.IPC);
                            }
                            pollPoE();

                            startPoeCheck();
//                        Log.w(TAG, "PoE Volt Check ON");
                            break;
//                        case "nosignal":
//                        case "signal":
//                            mPoeLevel.setText("PoE 48V OUT");
//                            break;
                    }
                }
            };
            IntentFilter filter = new IntentFilter();
            String viewerBroadcast = "com.sscctv.seeeyesmonitor";
            filter.addAction(viewerBroadcast);
            registerReceiver(viewerReceiver, filter);
        }
    }

    public void stopWatchingViewerClose() {
        if (viewerReceiver != null) {
            unregisterReceiver(viewerReceiver);
        }
    }

    public void startPoEViewCotrol() {
        if (controlReceiver == null) {
            controlReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String state = intent.getStringExtra("location");
//                    Log.v(TAG, "startPoEViewCotrol Broadcast : " + state);
                    switch (state) {
                        case "open":
                            mPoeView.setVisibility(View.VISIBLE);
                            break;
                        case "close":
                            mPoeView.setVisibility(View.INVISIBLE);
                            break;
                    }
                }
            };
            IntentFilter filter = new IntentFilter();
            String viewerBroadcast = "com.sscctv.poeView";
            filter.addAction(viewerBroadcast);
            registerReceiver(controlReceiver, filter);
        }
    }

    public void stopPoEViewCotrol() {
        if (controlReceiver != null) {
            unregisterReceiver(controlReceiver);
        }
    }


    boolean levelUpdate = false;

    public void pollPoE() {
        Log.d(TAG, "pollPoE Start");
        mLevelMeterListener = new LevelMeterListener() {
            @Override
            public void onLevelChanged(final int level, final int value) {
                switch (level) {
                    case MASTER_CHECK_POE:
                        mValue = value;
                        break;
                    case SUB_CHECK_POE:
                        sValue = value;
                        levelUpdate = true;
                        break;
                }
//                Log.d(TAG, "levelUpdate: " + levelUpdate);
                if (levelUpdate) {

                    levelUpdate = false;
                    voltCheckState();
                    pseCheckState();
//                    Log.d(TAG, "PoeFocusHandlers1 :" + screenState);
                    sideCheck();

                    if (screenState) {
                        Message poe_focus_msg = poeFocusHandlers.obtainMessage();
                        poeFocusHandlers.sendMessage(poe_focus_msg);
//                        Log.d(TAG, "PoeFocusHandlers");
                    } else {
                        Message poe_msg = poeHandler.obtainMessage();
                        poeHandler.sendMessage(poe_msg);
                    }
//                    Log.d(TAG, "PoE Volt: " + mValue + " . " + sValue + " V , PSE Pin: " + getPseState() + " VP Pin: " + getVpState());

                }
            }
        };
        mMcuControl.addReceiveBufferListener(mLevelMeterListener);

    }


    public void pseCheckState() {
//        Log.d(TAG, "PseCheckState: " + getPseState());
        if (getPseState().equals("0")) {
            pseLevel = false;
            updateFlag = true;
            pseDisable();
        } else if (getPseState().equals("1")) {
            pseLevel = true;
            updateFlag = true;
            pseEnable();

        }
    }

    public void voltCheckState() {
        if ((!voltInput) && (mValue > 10)) {
            voltInput = true;
//            vpDisable();
        } else if ((voltInput) && (mValue < 10)) {

            voltInput = false;
            updateFlag = true;
//            vpEnable();


        }
    }

    private void sideCheck() {
        SharedPreferences sharedPreferences = getSharedPreferences("state", MODE_PRIVATE);
        String port1 = sharedPreferences.getString("stat1", "out");
        String port2 = sharedPreferences.getString("stat2", "out");

        if (port2.equals("in")) {
            sideValue++;
        } else {
            sideValue = 0;
        }

        if (sideValue > 10) {
            if (mValue > 10) {
                vpDisable();
                sideValue = 21;
            }
        } else {
            vpEnable();
        }

//
//        if (mValue > 10) {
//            sideValue++;
//
//            if (sideValue > 20 && port2.equals("in")) {
//                vpDisable();
//                sideValue = 21;
//            }
//        } else {
//            sideValue = 0;
//            vpEnable();
//        }
//        Log.d(TAG, "sideCheck mValue: " + mValue + " value: " + sideValue + " port2: " + port2);


    }


    @SuppressLint("ClickableViewAccessibility")
    public void mPoeViewTouchHandler() {
        mViewTouchListener = (v, event) -> {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    isMove = false;
                    mTouchX = event.getRawX();
                    mTouchY = event.getRawY();
                    mViewX = mParams.x;
                    mViewY = mParams.y;
                    mViewX = sParams.x;
                    mViewY = sParams.y;
                    break;
                case MotionEvent.ACTION_UP:
                    break;
                case MotionEvent.ACTION_MOVE:
                    isMove = true;
                    int x = (int) (event.getRawX() - mTouchX);
                    int y = (int) (event.getRawY() - mTouchY);
                    final int num = 5;
                    if ((x > -num && x < num) && (y > -num && y < num)) {
                        isMove = false;
                        break;
                    }
                    mParams.x = mViewX + x;
                    mParams.y = mViewY + y;
                    sParams.x = mViewX + x;
                    sParams.y = mViewY + y;
                    mManager.updateViewLayout(mPoeView, mParams);
                    break;
            }
            return true;
        };

        mPoeView.setOnTouchListener(mViewTouchListener);
    }

//
//    public void startWatchingOEClose() {
//        Log.d(TAG, "startWatchingOEClose()");
//        if (closeReceiver == null) {
//            closeReceiver = new BroadcastReceiver() {
//                @Override
//                public void onReceive(Context context, Intent intent) {
//                    String state = intent.getStringExtra("state");
//                    Log.v(TAG, "Close Screen Broadcast : " + state);
//
//                    switch (state) {
//                        case "PoE ON":
//                            sendNotification();
//                            poeStart();
//                            break;
//                        case "PoE OFF":
//                            exitNotification();
//                            poeStop();
//                            break;
//                    }
//                }
//            };
//            IntentFilter filter = new IntentFilter();
//            String closeBroadcast = "net.biyee.onviferenterprise.OnviferActivity";
//            filter.addAction(closeBroadcast);
//            registerReceiver(closeReceiver, filter);
//        }
//    }

//    public void stopWatchingOEClose() {
//        if (closeReceiver != null) {
//            this.unregisterReceiver(closeReceiver);
//        }
//    }

//    public void sendNotification() {
//        isRunning = true;
//
////        Log.d(TAG, "sendNotification isRunning: " + isRunning);
//        Resources res = getResources();
//        NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
//        builder.setContentTitle("PoE")
//                .setContentText("Running PoE service")
//                .setTicker("PoE")
//                .setSmallIcon(R.drawable.poe_noti)
//                .setLargeIcon(BitmapFactory.decodeResource(res, R.drawable.poe_large))
//                .setAutoCancel(true)
//                .setWhen(System.currentTimeMillis());
////                    .setDefaults(Notification.DEFAULT_ALL);
//
//        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
//            builder.setCategory(Notification.CATEGORY_MESSAGE)
//                    .setPriority(Notification.PRIORITY_HIGH)
//                    .setVisibility(Notification.VISIBILITY_PUBLIC);
//        }
//
//        nm.notify(11000, builder.build());
//
//    }
//
//    public void exitNotification() {
//        isRunning = false;
////        Log.d(TAG, "exitNotification: " + nm);
//
//        if (nm != null) {
//            nm.cancel(11000);
////            Log.d(TAG, "exitNotification cancel: " + nm);
//        }
//
//    }

    private void startPoeCheck() {
        try {
            mMcuControl.startPoeCheck();
//            poeVoltCheckOn();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void stopPoeCheck() {
        try {
            mMcuControl.stopPoeCheck();
//            poeVoltCheckOff();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }


    public void pseEnable() {
        // Select Lan port: CAM Side
        try {
            opt.writeBytes("echo 1 > /sys/class/gpio_sw/PE17/data\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void pseDisable() {
        // Select Lan port: NVR Side
        try {
            opt.writeBytes("echo 0 > /sys/class/gpio_sw/PE17/data\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void vpEnable() {
        // VP Pin Select: IPM internal 48V output
        try {
            opt.writeBytes("echo 0 > /sys/class/gpio_sw/PE11/data\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void vpDisable() {
        // VP Pin Select: IPM external 48V output
        try {
            opt.writeBytes("echo 1 > /sys/class/gpio_sw/PE11/data\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void poeStart() {
//        vpEnable();
        pseEnable();
    }        // PoE Start

    public void poeStop() {
//        vpDisable();
        pseDisable();
    }         // PoE Stop

    public void poeVoltCheckOn() throws IOException {
        vpDisable();
        pseEnable();
    }  // PoE Voltage Check Start

    public void poeVoltCheckOff() throws IOException {
        vpEnable();
        pseDisable();
    } // PoE Voltage Check Stop

    public String getVpState() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("cat /sys/class/gpio_sw/PE11/data");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            sValue = input.readLine();
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return sValue;
    }

    public String getPseState() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("cat /sys/class/gpio_sw/PE17/data");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            sValue = input.readLine();
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return sValue;
    }
}
