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
    private TextView mPoeLevel, mFocusLevel, mFocusPoeLevel;

    private View mPoeView, mFocusPoeView;
    private View.OnTouchListener mViewTouchListener;
    private WindowManager mManager, sManager;
    private WindowManager.LayoutParams mParams, sParams;
    private float mTouchX, mTouchY;
    private int mViewX, mViewY;
    private int mValue, sValue, mFocus;
    private boolean isMove = false;

    private int mFocusLevelMax;

    private boolean isRunning;
    private boolean screenState;
    private boolean pseLevel;
    private boolean voltInput;
    private boolean updateFlag;
    public static int iFOREGROUND_SERVICE = 101;
    private DataOutputStream opt;
    private static final String TAG = "PoE Service";
    public static Context mContext;

    public PoEIntentService() {
        super("PoEIntentService");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mContext = this;
//        Log.d(TAG, "onCreate() ");
        gpioPortSet();
        LayoutInflater poe_Inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        assert poe_Inflater != null;
        mPoeView = poe_Inflater.inflate(R.layout.poe_on_view, null);
        mPoeLevel = (TextView) mPoeView.findViewById(R.id.mp_poe_level);
        mParams = new WindowManager.LayoutParams(WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT, 0, 0,
                WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);
        mParams.gravity = Gravity.TOP | Gravity.CENTER;
        mManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        mManager.addView(mPoeView, mParams);
        mPoeView.setVisibility(View.INVISIBLE);

        LayoutInflater mInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mFocusPoeView = mInflater.inflate(R.layout.poe_foucs_view, null);
        mFocusPoeLevel = (TextView) mFocusPoeView.findViewById(R.id.focus_poe_level);
        sParams = new WindowManager.LayoutParams(WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT, 50, 20,
                WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE,
                PixelFormat.TRANSLUCENT);
        sParams.gravity = Gravity.BOTTOM | Gravity.LEFT;
        sManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        sManager.addView(mFocusPoeView, sParams);
        mFocusPoeView.setVisibility(View.INVISIBLE);


    }


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

//        Log.d(TAG, "Start service in foreground ");
//        sendNotification();
        //if (mMcuControl == null) {
        startWatchingOEClose();
        startWatchingViewerClose();
        startPoEViewCotrol();
        startWatchingOEBroadcast();

        mMcuControl = new McuControl();
        mMcuControl.start(VideoSource.IPC);
        startPoeCheck();
        try {
            mMcuControl.startPoeCheck();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }

        updateFlag = true;
        pseLevel = false;
        voltInput = false;
        screenState = false;
//                levelUpdate = true;
        isRunning = true;


        pollPoE();
        mPoeView.setVisibility(View.VISIBLE);
        mPoeViewTouchHandler();


        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        isRunning = false;

        stopWatchingOEBroadcast();
        stopWatchingOEClose();
        stopWatchingViewerClose();
        stopPoEViewCotrol();
        stopPoeCheck();
        mMcuControl.stop();
        mMcuControl.removeReceiveBufferListener(mLevelMeterListener);
        mMcuControl = null;
        exitNotification();

//        Toast.makeText(this, "PoE Service Destroy", Toast.LENGTH_SHORT).show();
        if (mPoeView != null) {
            mManager.removeView(mPoeView);
        }
        super.onDestroy();
    }

    @Override
    protected void onHandleIntent(Intent intent) {
    }

    public void startWatchingOEBroadcast() {
        mOEBroadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String state = intent.getStringExtra("state");
//                Log.d(TAG, "BraodCast: " + state);
                String focus = intent.getStringExtra("focus");

                if (state != null && state.equals("resume")) {
                    screenState = true;
                    mPoeView.setVisibility(View.INVISIBLE);
                    mFocusPoeView.setVisibility(View.VISIBLE);

                    updateFlag = true;
//                    Log.d(TAG, "Resume pfLevel = " + screenState);
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
//                    Log.d(TAG, "Pause pfLevel " + screenState);
                }

            }
        };

        IntentFilter filter = new IntentFilter();
        filter.addAction(sBroadcast);
        registerReceiver(mOEBroadcastReceiver, filter);
    }

    public void stopWatchingOEBroadcast() {
        if (mOEBroadcastReceiver != null) {
            this.unregisterReceiver(mOEBroadcastReceiver);
        }
    }

    public void startWatchingViewerClose() {
//        Log.d(TAG, "startWatchingOEClose()");
        if (viewerReceiver == null) {
            viewerReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String state = intent.getStringExtra("state");
//                    Log.v(TAG, "Close Screen Broadcast : " + state);
                    switch (state) {
                        case "resume":
                            stopPoeCheck();
                            mMcuControl.controlStop();
                            if(getPseState().equals("1")) {
                                mPoeLevel.setText("PoE 48V OUT");
                            } else {
                                mPoeLevel.setText("PoE OFF");
                            }
//                        Log.w(TAG, "PoE Volt Check OFF");
                            break;
                        case "pause":
                            startPoeCheck();
                            mMcuControl.controlStart();
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

                    if (screenState) {
                        Message poe_focus_msg = poe_focus_handler.obtainMessage();
                        poe_focus_handler.sendMessage(poe_focus_msg);
                    } else {
                        Message poe_msg = poe_handler.obtainMessage();
                        poe_handler.sendMessage(poe_msg);
                    }
//                    Log.d(TAG, "PoE Volt: " + mValue + " . " + sValue + " V ");

                }
            }
        };
        mMcuControl.addReceiveBufferListener(mLevelMeterListener);

    }


    public void pseCheckState() {
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
        if ((!voltInput) && (mValue != 0)) {
            voltInput = true;
            vpDisable();
        } else if ((voltInput) && (mValue == 0)) {
//            Log.d(TAG, "Volt Input: " + voltInput + " mValue: " + mValue);
            voltInput = false;
            updateFlag = true;
            vpEnable();
        }
    }


    @SuppressLint("HandlerLeak")
    final Handler poe_handler = new Handler() {
        public void handleMessage(Message msg) {
//            Log.d(TAG,  "Not Full Screen updateFlag = " + updateFlag);
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
    };

    @SuppressLint("HandlerLeak")
    final Handler poe_focus_handler = new Handler() {
        public void handleMessage(Message msg) {
//            Log.d(TAG, "Full Screen updateFlag = " + updateFlag);
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
    };


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


    public void startWatchingOEClose() {
//        Log.d(TAG, "startWatchingOEClose()");
        if (closeReceiver == null) {
            closeReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String state = intent.getStringExtra("state");
//                Log.v(TAG, "Close Screen Broadcast : " + state);

                    switch (state) {
                        case "PoE ON":
                            sendNotification();
                            poeStart();
                            break;
                        case "PoE OFF":
                            exitNotification();
                            poeStop();
                            break;

                    }
                }
            };
            IntentFilter filter = new IntentFilter();
            String closeBroadcast = "net.biyee.onviferenterprise.OnviferActivity";
            filter.addAction(closeBroadcast);
            registerReceiver(closeReceiver, filter);
        }
    }

    public void stopWatchingOEClose() {
        if (closeReceiver != null) {
            this.unregisterReceiver(closeReceiver);
        }
    }

    public void sendNotification() {
        Resources res = getResources();
        isRunning = true;
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this);
        builder.setContentTitle("PoE")
                .setContentText("Running PoE service")
                .setTicker("PoE")
                .setSmallIcon(R.drawable.poe_noti)
                .setLargeIcon(BitmapFactory.decodeResource(res, R.drawable.poe_large))
                .setAutoCancel(true)
                .setWhen(System.currentTimeMillis());
//                    .setDefaults(Notification.DEFAULT_ALL);

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
            builder.setCategory(Notification.CATEGORY_MESSAGE)
                    .setPriority(Notification.PRIORITY_HIGH)
                    .setVisibility(Notification.VISIBILITY_PUBLIC);
        }

        nm = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        nm.notify(11000, builder.build());

    }

    public void exitNotification() {
        isRunning = false;
        if (nm != null) nm.cancel(11000);
    }

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
        vpEnable();
        pseEnable();
    }        // PoE Start

    public void poeStop() {
        vpDisable();
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
