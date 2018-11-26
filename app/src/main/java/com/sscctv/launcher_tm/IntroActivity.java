package com.sscctv.launcher_tm;

import android.app.Activity;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;

public class IntroActivity extends Activity {
    private PopupWindow popupWindow;
    private Button btn_ok, btn_no;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_intro);

        View popupView = getLayoutInflater().inflate(R.layout.activity_intro, null);
        popupWindow = new PopupWindow(popupView, RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        popupWindow.setFocusable(true);
        popupWindow.setAnimationStyle(-1);
        popupWindow.showAtLocation(popupView, Gravity.CENTER, 0, 0);

        btn_ok.findViewById(R.id.intro_ok);

        Button btn_no = findViewById(R.id.intro_no);
        btn_no.setOnClickListener(no_button_click);
    }

    @Override
    public void onBackPressed() {
        // TODO Auto-generated method stub
        super.onBackPressed();
    }


   private View.OnClickListener no_button_click = new View.OnClickListener() {
       @Override
       public void onClick(View v) {
           finish();
           popupWindow.dismiss();
           Log.d("Intro", "No Button");
       }
   };
}

