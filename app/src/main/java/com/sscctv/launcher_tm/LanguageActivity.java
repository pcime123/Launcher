package com.sscctv.launcher_tm;

import android.app.Activity;
import android.app.AlarmManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.io.IOException;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class LanguageActivity extends Activity implements View.OnClickListener {

    private final String TAG = "LanguageActivity";
    private View decorView;
    private int uiOption;
    private AlarmManager alarm;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_language);


        Button btn_ko = findViewById(R.id.button_korean);
        Button btn_en = findViewById(R.id.button_english);
        Button btn_ja = findViewById(R.id.button_japanese);
        Button btn_it = findViewById(R.id.button_italian);
        Button btn_ge = findViewById(R.id.button_german);
        Button btn_next = findViewById(R.id.button_next);

        btn_ko.setOnClickListener(this);
        btn_en.setOnClickListener(this);
        btn_ja.setOnClickListener(this);
        btn_it.setOnClickListener(this);
        btn_ge.setOnClickListener(this);
        btn_next.setOnClickListener(this);

        btn_ko.setFocusableInTouchMode(true);
        btn_ko.requestFocus();

        Runtime runtime = Runtime.getRuntime();
        try {
            runtime.exec("su -c ime enable com.standard.inputmethod.koreanime/.KoreanIME");
            runtime.exec("su -c ime enable jp.co.omronsoft.openwnn/.OpenWnnJAJP");
        } catch (IOException e) {
            e.printStackTrace();
        }


//        decorView = getWindow().getDecorView();
//        uiOption = getWindow().getDecorView().getSystemUiVisibility();
//        uiOption |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
//        uiOption |= View.SYSTEM_UI_FLAG_FULLSCREEN;
//        uiOption |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;


        alarm = (AlarmManager) getSystemService(Context.ALARM_SERVICE);

    }

    public void startActivity() {
        //Log.d(TAG, "startActivity");
        startActivity(new Intent(LanguageActivity.this, MainActivity.class));
        finish();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        // TODO Auto-generated method stub
        super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
//            decorView.setSystemUiVisibility(uiOption);
        }
    }

    private void switchLanguage(View.OnClickListener con, Locale language) {

        try {
            Locale locale = language;
            Class amnClass = Class.forName("android.app.ActivityManagerNative");
            Object amn = null;
            Configuration config = null;

            Method methodGetDefault = amnClass.getMethod("getDefault");
            methodGetDefault.setAccessible(true);
            amn = methodGetDefault.invoke(amnClass);

            Method methodGetConfiguration = amnClass.getMethod("getConfiguration");
            methodGetConfiguration.setAccessible(true);
            config = (Configuration) methodGetConfiguration.invoke(amn);

            Class configClass = config.getClass();
            Field f = configClass.getField("userSetLocale");
            f.setBoolean(config, true);

            config.locale = locale;

            Method methodUpdateConfiguration = amnClass.getMethod("updateConfiguration", Configuration.class);
            methodUpdateConfiguration.setAccessible(true);
            methodUpdateConfiguration.invoke(amn, config);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onBackPressed() {
//        if (!getIntent().hasCategory(Intent.CATEGORY_HOME)) {
//            super.onBackPressed();
//        }
    }

    @Override
    public void onClick(View v) {
        final AlarmManager systemService = (AlarmManager) getSystemService(Context.ALARM_SERVICE);

        switch (v.getId()) {
            case R.id.button_korean:
                switchLanguage(this, new Locale("ko"));
//                systemService.setTimeZone("Asia/Seoul");
                Runtime runtime = Runtime.getRuntime();
                try {
                    runtime.exec("su -c ime set com.standard.inputmethod.koreanime/.KoreanIME");
                } catch (IOException e) {
                    e.printStackTrace();
                }

                alarm.setTimeZone("GMT+9:00");
                break;
            case R.id.button_english:
                switchLanguage(this, new Locale("en"));
//                systemService.setTimeZone("America/New_york");
                alarm.setTimeZone("GMT-5:00");
                break;
            case R.id.button_japanese:
                switchLanguage(this, new Locale("ja"));
//                systemService.setTimeZone("Asia/Seoul");
                Runtime runtime1 = Runtime.getRuntime();
                try {
                    runtime1.exec("su -c ime set jp.co.omronsoft.openwnn/.OpenWnnJAJP");
                } catch (IOException e) {
                    e.printStackTrace();
                }
                alarm.setTimeZone("GMT+9:00");

                break;
            case R.id.button_italian:
                switchLanguage(this, new Locale("it"));
//                systemService.setTimeZone("Europe/Rome");
                alarm.setTimeZone("GMT+1:00");
                break;
            case R.id.button_german:
                switchLanguage(this, new Locale("de"));
//                systemService.setTimeZone("Europe/Berlin");
                alarm.setTimeZone("GMT+1:00");
                break;
            case R.id.button_next:
                startActivity();

                SharedPreferences pref = getSharedPreferences("doFirst", MODE_PRIVATE);
                SharedPreferences.Editor editor = pref.edit();
                editor.putBoolean("doFirst", false);
                editor.apply();
                break;

        }
//        startActivity();
    }

}
