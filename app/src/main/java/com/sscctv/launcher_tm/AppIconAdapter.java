package com.sscctv.launcher_tm;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.List;

/**
 * Created by trlim on 2015. 12. 11..
 *
 * 앱 아이콘과 이름을 표시하기 위한 어댑터
 */
public class AppIconAdapter extends ArrayAdapter<AppDetail> {
    private class ViewHolder {
        TextView label;
        ImageView icon;
        Button button;
    }

    private int mLayoutResource;

    public AppIconAdapter(Context context, int itemLayoutResource, List<AppDetail> apps) {
        super(context, itemLayoutResource, apps);

        mLayoutResource = itemLayoutResource;
    }
    public AppIconAdapter(Context context, int itemLayoutResource, int nameArrayResource, int imageArrayResource) {
        super(context, itemLayoutResource);

        mLayoutResource = itemLayoutResource;

        Resources res = context.getResources();

        TypedArray names = res.obtainTypedArray(nameArrayResource);
        TypedArray images = res.obtainTypedArray(imageArrayResource);

        addAll(AppDetail.fromResource(names, images));
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder;

        if (convertView == null) {
            viewHolder = new ViewHolder();

            LayoutInflater inflater = LayoutInflater.from(getContext());
            convertView = inflater.inflate(mLayoutResource, parent, false);

            viewHolder.label = (TextView) convertView.findViewById(R.id.grid_label);
            viewHolder.icon = (ImageView) convertView.findViewById(R.id.grid_image);
            //viewHolder.button = (Button) convertView.findViewById(R.id.button);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
        }

        AppDetail appDetail = getItem(position);

        if (viewHolder.label != null) {
            viewHolder.label.setText(appDetail.label);
        }
        viewHolder.icon.setImageDrawable(appDetail.icon);

        return convertView;
    }
}
