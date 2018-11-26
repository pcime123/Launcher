package com.sscctv.launcher_tm;

import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;

import java.util.ArrayList;

/**
 * Created by trlim on 2015. 12. 11..
 *
 * 앱 아이콘 항목 관련 정보
 */
public class AppDetail {
    public CharSequence label;
    public CharSequence name;
    public Drawable icon;


    public AppDetail(CharSequence label, Drawable icon) {
        this.label = label;
        this.icon = icon;
    }

    public static ArrayList<AppDetail> fromResource(TypedArray labels, TypedArray icons) {
        ArrayList<AppDetail> appDetails = new ArrayList<>();

        for (int i = 0; i < labels.length(); i++) {
            appDetails.add(new AppDetail(labels.getString(i), icons.getDrawable(i)));
        }

        return appDetails;
    }
}
