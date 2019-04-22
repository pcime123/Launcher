package com.sscctv.launcher_tm;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;

import static java.lang.String.format;

public class FactoryActivity extends Activity {

    private ImageView imgAndroid;
    private Animation anim;

    private AnimationDrawable animationDrawable;

    private View decorView;
    private int uiOption;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_factory);

//        decorView = getWindow().getDecorView();
//        uiOption = getWindow().getDecorView().getSystemUiVisibility();
//        uiOption |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
//        uiOption |= View.SYSTEM_UI_FLAG_FULLSCREEN;
//        uiOption |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;


        imgAndroid = findViewById(R.id.img_android);
        imgAndroid.setBackgroundResource(R.drawable.splash_loading);
        animationDrawable = (AnimationDrawable) imgAndroid.getBackground();
//        initView();

    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        // TODO Auto-generated method stub
        // super.onWindowFocusChanged(hasFocus);

        if (hasFocus) {
            animationDrawable.start();
//            factoryHandler.sendEmptyMessageDelayed(0, 3000);
//            startActivity(new Intent(FactoryActivity.this, LanguageActivity.class));
//            finish();
//            decorView.setSystemUiVisibility(uiOption);
            timer();
        }
    }

//    private final factoryHandler factoryHandler = new factoryHandler(this);
//
//    private static class factoryHandler extends Handler {
//        private final WeakReference<FactoryActivity> ref;
//
//        private factoryHandler(FactoryActivity test) {
//            ref = new WeakReference<>(test);
//        }
//
//        @Override
//        public void handleMessage(Message msg) {
//            super.handleMessage(msg);
//
//            FactoryActivity factoryActivity = ref.get();
//            if(factoryActivity != null) {
//
//            }
//        }
//
//    }

    private void timer() {
        @SuppressLint("HandlerLeak") Handler handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                startActivity(new Intent(FactoryActivity.this, LanguageActivity.class));
                finish();
            }
        };
        handler.sendEmptyMessageDelayed(0, 3000);
    }

    @Override
    public void onBackPressed() {
//        if (!getIntent().hasCategory(Intent.CATEGORY_HOME)) {
//            super.onBackPressed();
//        }
    }


}
