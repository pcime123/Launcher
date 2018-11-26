package com.sscctv.launcher_tm;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.GridView;

import java.util.ArrayList;
import java.util.List;

public class AppsActivity extends AppCompatActivity {
    private PackageManager mPackageManager;
    private List<AppDetail> mApps;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_apps);

        mPackageManager = getPackageManager();
        mApps = new ArrayList<>();

        Intent i = new Intent(Intent.ACTION_MAIN, null);
        i.addCategory(Intent.CATEGORY_LAUNCHER);

        List<ResolveInfo> availableActivities = mPackageManager.queryIntentActivities(i, 0);
        for(ResolveInfo ri:availableActivities){
            AppDetail app = new AppDetail(ri.loadLabel(mPackageManager), ri.activityInfo.loadIcon(mPackageManager));

            app.name = ri.activityInfo.packageName;

            mApps.add(app);
        }

        GridView gridview = (GridView) findViewById(R.id.apps_grid);
        gridview.setAdapter(new AppIconAdapter(this, R.layout.apps_item, mApps));
        gridview.setOnItemClickListener(new GridView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent i = mPackageManager.getLaunchIntentForPackage(mApps.get(position).name.toString());
                AppsActivity.this.startActivity(i);
            }
        });
    }
}
