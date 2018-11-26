package com.sscctv.launcher_tm;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.sscctv.seeeyes.ptz.LevelMeterListener;
import com.sscctv.seeeyes.ptz.McuControl;
import com.sscctv.seeeyes.VideoSource;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class poeActivity extends Activity {

    private static final String TAG = "MainActivity";
    private McuControl mMcuControl;         //Focus Meta MCU operating circuit
    private TextView mPoeLevel;
    private TextView sPoeLevel;
    private TextView mFocusLevel;
    private int mFocusLevelMax;
    private VideoSource mSource;
    private LevelMeterListener mLevelMeterListener;
    private DataOutputStream opt;
    private String sourceId;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_poe);
        mPoeLevel = (TextView) findViewById(R.id.mpoe_level);
        sPoeLevel = (TextView) findViewById(R.id.spoe_level);
        mMcuControl = new McuControl();

        gpioPortSet();
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
    protected void onResume() {

        super.onResume();


        sourceId = VideoSource.IPC;

        mSource = new VideoSource(sourceId);
        mMcuControl.start(sourceId);
        startLevelMeterHandler();
        Log.d(TAG, "Source = " + sourceId);

    }


    @Override
    protected void onPause() {

        super.onPause();
        mMcuControl.stop();

        try {
            poeVoltCheckOff();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void startLevelMeterHandler() {
        mMcuControl.addReceiveBufferListener(new LevelMeterListener() {
            @Override
            public void onLevelChanged(final int level, final int value) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Log.d(TAG, "Level = " + level + ", Value = " + value);
                        switch (level) {
                            case MASTER_CHECK_POE:
                                try {
                                    mupdatePoeLevel(value);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                break;
                            case SUB_CHECK_POE:
                                supdatePoeLevel(value);
                                break;

                        }
                    }
                });
            }
        });
    }

    public void mupdatePoeLevel(int mValue) throws IOException {
        mPoeLevel.setText(String.format("PoE : %02d", mValue));
        Log.d(TAG, "Master PoE Level = " + mValue);

//        if(mValue == 0) {
//            mPoeLevel.setText(String.format("PoE On"));
//            mSource.vpEnable();
//        } else {
//            mSource.vpDisable();
//        }
    }
    public void supdatePoeLevel(int sValue) {
        sPoeLevel.setText(String.format(". %02d V", sValue));
        Log.d(TAG, "Sub PoE Level = " + sValue);
    }

    public void onClick(View view) throws IOException {
        int i = view.getId();
        if (i == R.id.start_check) {
            startPoeCheck();
        } else if (i == R.id.pse_enable) {
            pseEnable();
            Toast toast = Toast.makeText(getApplicationContext(), "PSE Pin Enable", Toast.LENGTH_SHORT);
            toast.show();
        } else if (i == R.id.pse_disable) {
            pseDisable();
            Toast toast = Toast.makeText(getApplicationContext(), "PSE Pin Disable", Toast.LENGTH_SHORT);
            toast.show();
        } else if (i == R.id.vp_enable) {
            vpEnable();
            Toast toast = Toast.makeText(getApplicationContext(), "VP Pin Enable", Toast.LENGTH_SHORT);
            toast.show();
        } else if (i == R.id.vp_disable) {
            vpDisable();
            Toast toast = Toast.makeText(getApplicationContext(), "VP Pin Disable", Toast.LENGTH_SHORT);
            toast.show();
        } else if (i == R.id.vp_stopcheck) {
            stopPoeCheck();
        }
    }

    private void startPoeCheck() {
        try {
            mMcuControl.start(mSource.getSourceId());
            mMcuControl.startPoeCheck();
            poeVoltCheckOn();
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void stopPoeCheck() {
        try {
            mMcuControl.stopPoeCheck();
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
            opt.writeBytes("echo 0 > /sys/class/gpio_sw/PE11/data\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void poeStart() throws IOException {
        vpDisable();
        pseEnable();
    }        // PoE Start

    public void poeStop() throws IOException {
        vpEnable();
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