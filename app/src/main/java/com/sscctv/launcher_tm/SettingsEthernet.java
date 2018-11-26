package com.sscctv.launcher_tm;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.IpConfiguration;
import android.net.LinkAddress;
import android.net.NetworkUtils;
import android.net.StaticIpConfiguration;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v4.content.ContextCompat;
import android.text.InputFilter;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Type;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.List;
import java.util.Timer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import android.net.EthernetManager;

public class SettingsEthernet extends Activity implements RadioGroup.OnCheckedChangeListener {
    public static Context mContext;

    ProgressDialog _pd;
    private Boolean disposed = false;
    private static final String TAG = "SettingEthernet";
    private static final String NETWORK_PREFERENCE = "NETWORK_PREFERENCE";
    private TextView netModeSelect;
    private String ethMode;
    private int step = 0;
    private boolean mode;
    private EthernetManager mEthernetManager;
    private StaticIpConfiguration mStaticIpConfiguration;
    private IpConfiguration mIpConfiguration;

    private InputMethodManager imm;

    private SharedPreferences preferences;
    private SharedPreferences.Editor prefEditor;
    private Timer mTimer;
    private Handler mHandler;
    private InetAddress gatewayAddr;
    private Inet4Address inetAddr;
    private Inet4Address dnsAddr;
    private List<String> addList;
    private int prefixLength;
    private String netKey;
    private StringBuilder sb;

//    @BindView(R.id.btn_eth_set)
//    Button ethSet;
//
//    @BindView(R.id.btn_wifi_set)
//    Button wifiSet;

    @BindView(R.id.btn_input_network)
    Button inputBtn;

    @BindView(R.id.btn_clear_network)
    Button clearBtn;

    @BindView(R.id.btnAdd)
    Button addBtn;

    @BindView(R.id.edit)
    EditText editText;
//
//    @BindView(R.id.expandableListView)
//    ExpandableListView expandableListView;
//
//    List<String> listDataHeader;
//    HashMap<String, List<String>> listDataChild;

    @BindView(R.id.input_address)
    EditText IpAddress;

    @BindView(R.id.input_subnet)
    EditText MaskAddress;

    @BindView(R.id.input_gate)
    EditText GateWay;

    @BindView(R.id.input_dns)
    EditText DNSAddress;

    @BindView(R.id.group_mode)
    RadioGroup groupMode;

    @BindView(R.id.mode_dhcp)
    RadioButton dhcpMode;

    @BindView(R.id.mode_static)
    RadioButton staticMode;

    private String key;
    private int count;
    //    private ArrayList<String> items = new ArrayList<String>();
    private ListViewAdapter adapter;
    private ListView listView;
    private ListViewItem item;
    private final String fileName = "Ethernet.txt";
    private final String fileTemp = ".tmp";
    private String header;
    private String ipAddr;
    private String netMask;
    private String gateWay;
    private String dnsServer;


    @SuppressLint({"CommitPrefEdits", "PrivateApi", "WrongConstant"})
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings1);
        ButterKnife.bind(this);

        File dir = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/");
        if (!dir.exists()) {
            dir.mkdir();
        }

//        Mode = findViewById(R.id.input_mode);

        RadioGroup netMode = findViewById(R.id.group_mode);
        netMode.setOnCheckedChangeListener(this);

        _pd = new ProgressDialog(this);
        imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);

        mContext = this;

        mEthernetManager = (EthernetManager) getSystemService("ethernet");

        ipInput();
//        Log.d(TAG, "Get IP Address: " + getIpAddress());
        mHandler = new Handler();
//        groupMode.setFocusable(false);
//        groupMode.setFocusableInTouchMode(false);

        IpAddress.setOnFocusChangeListener((v, hasFocus) -> IpAddress.setSelection(IpAddress.getText().length()));
        MaskAddress.setOnFocusChangeListener((v, hasFocus) -> MaskAddress.setSelection(MaskAddress.getText().length()));
        GateWay.setOnFocusChangeListener((v, hasFocus) -> GateWay.setSelection(GateWay.getText().length()));
        DNSAddress.setOnFocusChangeListener((v, hasFocus) -> DNSAddress.setSelection(DNSAddress.getText().length()));


        listView = findViewById(R.id.listView);
        adapter = new ListViewAdapter();
        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @SuppressLint("SetTextI18n")
            @Override
            public void onItemClick(AdapterView parent, View view, int position, long id) {

                AlertDialog.Builder builder = new AlertDialog.Builder(mContext, R.style.CustomDialogStyle);

                item = (ListViewItem) parent.getItemAtPosition(position);
                String str = item.getTitle();

                String sValue = Arrays.toString(loadArray(str));
                sValue = sValue.replaceAll("\\[", "");
                sValue = sValue.replaceAll("\\]", "");
                String delStr = sValue;
                String[] info = sValue.split("_@#@_");
//                Log.d(TAG, "Info: " + Arrays.toString(info));

                header = info[0];
                ipAddr = info[1];
                netMask = info[2];
                gateWay = info[3];
                dnsServer = info[4];

                final TextView nullView  = new TextView(mContext);
                final TextView txtHeader = new TextView(mContext);
                final TextView txtIpAddr = new TextView(mContext);
                final TextView txtNetMask = new TextView(mContext);
                final TextView txtGateWay = new TextView(mContext);
                final TextView txtDnsAddr = new TextView(mContext);

                nullView.setText("");
                txtHeader.setText(getResources().getString(R.string.header) + " : " + header);
                txtIpAddr.setText(getResources().getString(R.string.ip_address) + " : " + ipAddr);
                txtNetMask.setText(getResources().getString(R.string.netmask) + " : " + netMask);
                txtGateWay.setText(getResources().getString(R.string.gateway) + " : " + gateWay);
                txtDnsAddr.setText(getResources().getString(R.string.dns_server) + " : " + dnsServer);

                nullView.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_null));
                txtHeader.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_title));
                txtIpAddr.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_title));
                txtNetMask.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_title));
                txtGateWay.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_title));
                txtDnsAddr.setTextSize(getResources().getDimensionPixelSize(R.dimen.dialog_text_title));

//                txtHeader.setTypeface(null, Typeface.BOLD);
//                txtIpAddr.setTypeface(null, Typeface.BOLD);
//                txtNetMask.setTypeface(null, Typeface.BOLD);
//                txtGateWay.setTypeface(null, Typeface.BOLD);
//                txtDnsAddr.setTypeface(null, Typeface.BOLD);

                txtHeader.setTextColor(getResources().getColor(R.color.WhiteSmoke));
                txtIpAddr.setTextColor(getResources().getColor(R.color.WhiteSmoke));
                txtNetMask.setTextColor(getResources().getColor(R.color.WhiteSmoke));
                txtGateWay.setTextColor(getResources().getColor(R.color.WhiteSmoke));
                txtDnsAddr.setTextColor(getResources().getColor(R.color.WhiteSmoke));

//                Log.d(TAG, str + " / " + ipAddr + " / " + netMask + " / " + gateWay + " / " + dnsServer);


                LinearLayout container = new LinearLayout(mContext);
                LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                params.topMargin = getResources().getDimensionPixelSize(R.dimen.dialog_top_margin);
                params.bottomMargin = getResources().getDimensionPixelSize(R.dimen.dialog_bottom_margin);
                params.leftMargin = getResources().getDimensionPixelSize(R.dimen.dialog_margin);
                params.rightMargin = getResources().getDimensionPixelSize(R.dimen.dialog_margin);

                nullView.setLayoutParams(params);
                txtHeader.setLayoutParams(params);
                txtIpAddr.setLayoutParams(params);
                txtNetMask.setLayoutParams(params);
                txtGateWay.setLayoutParams(params);
                txtDnsAddr.setLayoutParams(params);

                container.setOrientation(LinearLayout.VERTICAL);
                container.addView(nullView);
                container.addView(txtHeader);
                container.addView(txtIpAddr);
                container.addView(txtNetMask);
                container.addView(txtGateWay);
                container.addView(txtDnsAddr);

                builder.setTitle(getResources().getString(R.string.eth_set));
                builder.setMessage(getResources().getString(R.string.eth_contens));

                builder.setView(container);

                builder.setPositiveButton(getResources().getString(R.string.close),
                        (dialog, which) -> {
//                                Toast.makeText(getApplicationContext(), "Nothing", Toast.LENGTH_SHORT).show();
                            //TODO
                        });

                builder.setNeutralButton(getResources().getString(R.string.delete),
                        (dialog, which) -> {
                            delNetworkInfo(delStr);
                            adapter.clear();
                            adapter.notifyDataSetChanged();
                            readNetworkInfo();

                        });

                builder.setNegativeButton(getResources().getString(R.string.setup),
                        (dialog, which) -> {
                            setupNetwork task = new setupNetwork();
                            task.execute();
                        });
                builder.show();

            }
        });

        readNetworkInfo();
        adapter.notifyDataSetChanged();

    }


    //TODO: netcfg eth0 dhcp, netcfg eth0 up


    private void hideKeyboard() {
        imm.hideSoftInputFromWindow(editText.getWindowToken(), 0);
    }


    @SuppressLint("StaticFieldLeak")
    private class CheckTypesTask extends AsyncTask<Void, Void, Void> {
        ProgressDialog progressDialog = new ProgressDialog(SettingsEthernet.this);

        boolean value = false;

        @Override
        protected void onPreExecute() {

            progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            progressDialog.setMessage(getResources().getString(R.string.please_wait));
            progressDialog.show();

            killDhcp();

            if (ethMode.equals("STATIC")) {
                try {
                    netcfgEthDown();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                mStaticIpConfiguration = new StaticIpConfiguration();
                if (isIpAddress(IpAddress.getText().toString())) {
                    inetAddr = getIPv4Address(IpAddress.getText().toString());
                    value = true;
                } else {
                    Toast.makeText(getApplicationContext(), getResources().getString(R.string.not_ip), Toast.LENGTH_SHORT).show();
                    progressDialog.dismiss();
                    value = false;
                    return;
                }

                if (isIpAddress(MaskAddress.getText().toString())) {
                    prefixLength = maskStr2InetMask(MaskAddress.getText().toString());
                    value = true;
                } else {
                    Toast.makeText(getApplicationContext(), getResources().getString(R.string.not_mask), Toast.LENGTH_SHORT).show();
                    progressDialog.dismiss();
                    value = false;
                    return;

                }

                if (isIpAddress(GateWay.getText().toString())) {
                    gatewayAddr = getIPv4Address(GateWay.getText().toString());
                    value = true;
                } else {
                    Toast.makeText(getApplicationContext(), getResources().getString(R.string.not_gate), Toast.LENGTH_SHORT).show();

                    gatewayAddr = getIPv4Address("0.0.0.0");
                    value = true;
                }

                if (isIpAddress(DNSAddress.getText().toString())) {
                    dnsAddr = getIPv4Address(DNSAddress.getText().toString());
                    value = true;
                } else {
                    Toast.makeText(getApplicationContext(), getResources().getString(R.string.not_dns), Toast.LENGTH_SHORT).show();
                    value = false;
                    progressDialog.dismiss();
                    return;
                }

            } else if (ethMode.equals("DHCP")) {
                try {
                    dhcpcdDhcp();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

//            Log.d(TAG, "Go SET");

            super.onPreExecute();

        }

        @Override
        protected Void doInBackground(Void... voids) {
            if (ethMode.equals("STATIC")) {
                try {
                    netcfgEthUp();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                if (value) setStaticIP();
            } else if (ethMode.equals("DHCP")) {
//                Log.d(TAG, "DHCP SET");
                try {
                    netcfgEthDown();
                    netcfgEthUp();

                } catch (IOException e) {
                    e.printStackTrace();
                }
                mIpConfiguration = new IpConfiguration(IpConfiguration.IpAssignment.DHCP, IpConfiguration.ProxySettings.NONE, null, null);
                mEthernetManager.setConfiguration(mIpConfiguration);


            }
            return null;
        }

        @Override
        @SuppressLint("DefaultLocale")
        protected void onPostExecute(Void aVoid) {
            step = 0;
            Log.d(TAG, "Network Setup");

            new Thread(() -> {
                while (!getEnableIP()) {
                    step++;

                    Log.d(TAG, "Step: " + step);
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        Log.e(TAG, e.getMessage());
                    }

                    if (getEnableIP()) {

                        try {
                            Thread.sleep(2000);
                            retrieveInfo();
                            progressDialog.dismiss();
                            Log.d(TAG, "Enable IP Network Setup");

//                            Log.d(TAG, "DHCP Set complete");
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                            Log.e(TAG, e.getMessage());

                        }

                        break;
                    }

                    if (step == 30) {
                        retrieveInfo();
                        progressDialog.dismiss();
                        Log.d(TAG, "Stop 30 Enable IP Network Setup");

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(getApplicationContext(), getResources().getString(R.string.time_out), Toast.LENGTH_SHORT).show();

                            }
                        });
                        break;
                    }

                }
                Log.d(TAG, "Get Enable IP Network Setup");

                retrieveInfo();
                progressDialog.dismiss();
            }).start();

            super.onPostExecute(aVoid);
        }
    }

    @SuppressLint("StaticFieldLeak")
    private class setupNetwork extends AsyncTask<Void, Void, Void> {
        ProgressDialog progressDialog = new ProgressDialog(SettingsEthernet.this);

        boolean value = false;

        @Override
        protected void onPreExecute() {

            progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
//            progressDialog.setCancelable(false);
            progressDialog.setMessage(getResources().getString(R.string.please_wait));
            progressDialog.show();

            ethMode = "STATIC";

            killDhcp();

            try {
                netcfgEthDown();
            } catch (IOException e) {
                e.printStackTrace();
            }

            mStaticIpConfiguration = new StaticIpConfiguration();
            inetAddr = getIPv4Address(ipAddr);
            prefixLength = maskStr2InetMask(netMask);
            gatewayAddr = getIPv4Address(gateWay);
            dnsAddr = getIPv4Address(dnsServer);

//            Log.d(TAG, "Go SET");

            super.onPreExecute();

        }

        @Override
        protected Void doInBackground(Void... voids) {
            if (ethMode.equals("STATIC")) {
                try {
                    netcfgEthUp();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                setStaticIP();
            } else if (ethMode.equals("DHCP")) {
//                Log.d(TAG, "DHCP SET");
                try {
                    netcfgEthDown();
                    netcfgEthUp();

                } catch (IOException e) {
                    e.printStackTrace();
                }
                mIpConfiguration = new IpConfiguration(IpConfiguration.IpAssignment.DHCP, IpConfiguration.ProxySettings.NONE, null, null);
                mEthernetManager.setConfiguration(mIpConfiguration);


            }
            return null;
        }

        @Override
        @SuppressLint("DefaultLocale")
        protected void onPostExecute(Void aVoid) {
            step = 0;
            Log.d(TAG, "Network Setup");

            new Thread(() -> {
                while (!getEnableIP()) {
                    step++;

                    Log.d(TAG, "Step: " + step);
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                        Log.e(TAG, e.getMessage());
                    }

                    if (getEnableIP()) {

                        try {
                            Thread.sleep(2000);
                            retrieveInfo();
                            progressDialog.dismiss();
                            Log.d(TAG, " Enable IP Network Setup");

//                            Log.d(TAG, "DHCP Set complete");
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                            Log.e(TAG, e.getMessage());

                        }

                        break;
                    }

                    if (step == 30) {
                        retrieveInfo();
                        progressDialog.dismiss();
                        Log.d(TAG, "Stop 30 Enable IP Network Setup");

                        Toast.makeText(getApplicationContext(), getResources().getString(R.string.time_out), Toast.LENGTH_SHORT).show();

                        break;
                    }

                }
                Log.d(TAG, "Get Enable IP Network Setup");

            }).start();

            super.onPostExecute(aVoid);
        }
    }

    @Override
    protected void onResume() {

        if (mEthernetManager != null) {
            retrieveInfo();
        }

        super.onResume();

    }

    @Override
    protected void onPause() {
        disposed = true;
        super.onPause();
    }

    @OnClick(R.id.btn_clear_network)
    void onClickClear() {


        IpAddress.setText("");
        MaskAddress.setText("");
        GateWay.setText("");
        DNSAddress.setText("");

    }

    @OnClick(R.id.btn_input_network)
    void onClickInput() {
        hideKeyboard();

        CheckTypesTask task = new CheckTypesTask();
        task.execute();
    }


    private void killDhcp() {
        List<String> listPID = new ArrayList<>();
        try {
            Process p = Runtime.getRuntime().exec("ps");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String sLine;

            while ((sLine = input.readLine()) != null) {
                Pattern pattenDHCP = Pattern.compile("dhcp\\s+([0-9]+)\\s.+dhcpcd");
                Matcher m = pattenDHCP.matcher(sLine);
                if (m.find()) {
                    listPID.add(m.group(1));

                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        for (String sPID : listPID) {
            Log.d(TAG, "kill " + sPID);
            try {
                Process pKillDHCP = Runtime.getRuntime().exec("su -c kill " + sPID);
                BufferedReader inputKillDHCP = new BufferedReader(new InputStreamReader(pKillDHCP.getInputStream()));
                String sLineKillDHCP;
                Log.d(TAG, "su kill " + sPID);
                while ((sLineKillDHCP = inputKillDHCP.readLine()) != null) {
                    Log.d(TAG, sLineKillDHCP);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }


    private void netcfgEthDown() throws IOException {
        Runtime.getRuntime().exec("su -c netcfg eth0 down");
    }

    private void netcfgEthUp() throws IOException {
        Runtime.getRuntime().exec("su -c netcfg eth0 up");

    }

    private void netcfgDhcp() throws IOException {
        Runtime.getRuntime().exec("su -c netcfg eth0 dhcp");
    }

    private void dhcpcdDhcp() throws IOException {
        Runtime.getRuntime().exec("dhcpcd -p eth0");
    }

    public void setDefaultIP(StaticIpConfiguration mStaticIpConfiguration, EthernetManager mEthernetManager) {
        Inet4Address inetAddr = getIPv4Address("192.168.1.150");
        int prefixLength = maskStr2InetMask("255.255.255.0");
        Inet4Address gatewayAddr = getIPv4Address("192.168.1.1");
        Inet4Address dnsAddr = getIPv4Address("168.126.63.1");

        Class<?> clazz = null;

        try {
            clazz = Class.forName("android.net.LinkAddress");
        } catch (Exception e) {
            e.getMessage();
        }

        Class[] cl = new Class[]{InetAddress.class, int.class};
        Constructor<LinkAddress> cons = null;

        try {
            cons = (Constructor<LinkAddress>) clazz.getConstructor(cl);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }

        Object[] x = {inetAddr, prefixLength};

        String dnsStr2 = "8.8.8.4";

        try {
            mStaticIpConfiguration.ipAddress = cons.newInstance(x);
        } catch (IllegalAccessException | InstantiationException | InvocationTargetException e) {
            e.printStackTrace();
        }

        mStaticIpConfiguration.gateway = gatewayAddr;
        mStaticIpConfiguration.dnsServers.add(dnsAddr);

        if (!dnsStr2.isEmpty()) {
            mStaticIpConfiguration.dnsServers.add(getIPv4Address(dnsStr2));
        }
//        Log.d(TAG, "mStatic: " + mStaticIpConfiguration);

        mIpConfiguration = new IpConfiguration(IpConfiguration.IpAssignment.STATIC, IpConfiguration.ProxySettings.NONE, mStaticIpConfiguration, null);
        mEthernetManager.setConfiguration(mIpConfiguration);

    }

    private void setStaticIP() {
        if (Arrays.toString(inetAddr.getAddress()).isEmpty() || prefixLength == 0 || gatewayAddr.toString().isEmpty() || dnsAddr.toString().isEmpty()) {
            return;
        }

        Class<?> clazz = null;

        try {
            clazz = Class.forName("android.net.LinkAddress");
        } catch (Exception e) {
            e.getMessage();
        }

        Class[] cl = new Class[]{InetAddress.class, int.class};
        Constructor<LinkAddress> cons = null;

        try {
            cons = (Constructor<LinkAddress>) clazz.getConstructor(cl);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }

        Object[] x = {inetAddr, prefixLength};

        String dnsStr2 = "8.8.8.4";

        try {
            mStaticIpConfiguration.ipAddress = cons.newInstance(x);
        } catch (IllegalAccessException | InstantiationException | InvocationTargetException e) {
            e.printStackTrace();
        }

        mStaticIpConfiguration.gateway = gatewayAddr;
        mStaticIpConfiguration.dnsServers.add(dnsAddr);

        if (!dnsStr2.isEmpty()) {
            mStaticIpConfiguration.dnsServers.add(getIPv4Address(dnsStr2));
        }
//        Log.d(TAG, "mStatic: " + mStaticIpConfiguration);

        mIpConfiguration = new IpConfiguration(IpConfiguration.IpAssignment.STATIC, IpConfiguration.ProxySettings.NONE, mStaticIpConfiguration, null);
        mEthernetManager.setConfiguration(mIpConfiguration);

    }

    public static int maskStr2InetMask(String maskStr) {
        StringBuffer sb;
        String str;
        int inetmask = 0;
        int count = 0;
        /*
         * check the subMask format
         */
        Pattern pattern = Pattern.compile("(^((\\d|[01]?\\d\\d|2[0-4]\\d|25[0-5])\\.){3}(\\d|[01]?\\d\\d|2[0-4]\\d|25[0-5])$)|^(\\d|[1-2]\\d|3[0-2])$");
        if (!pattern.matcher(maskStr).matches()) {
            Log.e(TAG, "subMask is error");
            return 0;
        }

        String[] ipSegment = maskStr.split("\\.");
        for (int n = 0; n < ipSegment.length; n++) {
            sb = new StringBuffer(Integer.toBinaryString(Integer.parseInt(ipSegment[n])));
            str = sb.reverse().toString();
            count = 0;
            for (int i = 0; i < str.length(); i++) {
                i = str.indexOf("1", i);
                if (i == -1)
                    break;
                count++;
            }
            inetmask += count;
        }
        return inetmask;
    }

    public static Inet4Address getIPv4Address(String text) {
        try {
            return (Inet4Address) NetworkUtils.numericToInetAddress(text);
        } catch (IllegalArgumentException | ClassCastException e) {
            return null;
        }
    }

    @SuppressLint("SetTextI18n")
    private void retrieveInfo() {
//        Log.d(TAG, "Ethernet Retrieve Information..");
        runOnUiThread(() -> {
            String sValue;
            String dns;
            String mode;

            mode = getEthMode();

            switch (mode) {
                case "DHCP":
//                    Log.d(TAG, "DHCP MODE");
                    dhcpMode.setChecked(true);
//                    addBtn.setEnabled(false);
//
                    try {
                        Process p = Runtime.getRuntime().exec("getprop");
                        BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
                        while ((sValue = input.readLine()) != null) {

                            if (sValue.contains("[dhcp.eth0.gateway]:")) {
                                Pattern pDHCPGateway = Pattern.compile("\\[dhcp.eth0.gateway\\]: \\[(.+?)\\]");
                                Matcher m = pDHCPGateway.matcher(sValue);
                                if (m.find()) {
                                    GateWay.setText(m.group(1));
//                                    Log.d(TAG, "GateWay---------" + m.group(1));
                                }

                            } else if (sValue.contains("[dhcp.eth0.ipaddress]:")) {
                                Pattern pDHCPIPAddress = Pattern.compile("\\[dhcp.eth0.ipaddress\\]: \\[(.+?)\\]");
                                Matcher m = pDHCPIPAddress.matcher(sValue);
                                if (m.find()) {
                                    IpAddress.setText(m.group(1));
//                                    Log.d(TAG, "IP---------" + m.group(1));
                                }
                            } else if (sValue.contains("[dhcp.eth0.mask]:")) {
                                Pattern pDHCPIPAddress = Pattern.compile("\\[dhcp.eth0.mask\\]: \\[(.+?)\\]");
                                Matcher m = pDHCPIPAddress.matcher(sValue);
                                if (m.find()) {
                                    MaskAddress.setText(m.group(1));
//                                    Log.d(TAG, "Mask---------" + m.group(1));

                                }
                            } else if (sValue.contains("[net.dns1]:")) {
                                Pattern pattern = Pattern.compile("\\[net.dns1\\]: \\[(.+?)\\]");
                                Matcher m = pattern.matcher(sValue);
                                if (m.find()) {
                                    dns = m.group(1);

                                    DNSAddress.setText(dns);
//                                    Log.d(TAG, "DNS---------" + m.group(1));
                                }
                            }
                        }
                        input.close();
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }

                    break;
                case "STATIC":
//                    addBtn.setEnabled(true);
                    staticMode.setChecked(true);
//                    Log.d(TAG, "Static MODE");
//                    Log.d(TAG, "IP Address: " + getEthernetIPAddress());
//                    Log.d(TAG, "Mask Address: " + getEthernetMaskAddress());
//                    Log.d(TAG, "GateWay: " + getGateWay());
                    IpAddress.setText(getEthernetIPAddress());
                    MaskAddress.setText(getEthernetMaskAddress());
                    GateWay.setText(getGateWay());
                    DNSAddress.setText(getDNSAddress());
                    break;

            }

//            } else {
//                addBtn.setEnabled(false);
////                Mode.setText("Not Ethernet");
//                IpAddress.setText("");
//                MaskAddress.setText("");
//                GateWay.setText("");
//                DNSAddress.setText("");
//            }
        });

    }

    private static boolean getEnableIP() {
        String sValue = "";
        boolean bValue;

        try {
            Process p = Runtime.getRuntime().exec("su -c ifconfig eth0");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));

            String sLine;
            while ((sLine = input.readLine()) != null) {
                sValue = sLine;
            }
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (!sValue.contains("eth0")) {
            bValue = false;
        } else {
            bValue = true;
        }
//        Log.d(TAG, "ifconfig: " + bValue);

        return bValue;
    }

    private static String getEthernetIPAddress() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("su -c ifconfig eth0");
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
//        Log.d(TAG, "getEthernet: " + sValue);
        return sValue;
    }

    private static String getEthernetMaskAddress() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("ifconfig eth0");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));

            String sLine;
            while ((sLine = input.readLine()) != null) {
                if (sLine.contains("eth0:")) {
                    Pattern pIPAddress = Pattern.compile("mask (.+?) ");
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
//        Log.d(TAG, "getEthernetMaskAddress: " + sValue);

        return sValue;
    }

    private static String getEthernetGateWay() {
        String sValue = "";
        try {
            Process p = Runtime.getRuntime().exec("su -c ip route get 8.8.8.8");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));

            String sLine;
            while ((sLine = input.readLine()) != null) {
                if (sLine.contains("8.8.8.8")) {
                    Pattern pIPAddress = Pattern.compile("via (.+?) ");
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
//        Log.d(TAG, "getEthernetGateWay: " + sValue);

        return sValue;
    }

    private static String getDNSAddress() {
        String sValue = "";

        try {
            Process p = Runtime.getRuntime().exec("getprop net.dns1");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            sValue = input.readLine();
            input.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
//        Log.d(TAG, "getDNSAddress: " + sValue);

        return sValue;
    }


    public static boolean isIpAddress(String ipAddress) {

        boolean returnValue = false;

        String regex = "^([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})$";
        Pattern p = Pattern.compile(regex);
        Matcher m = p.matcher(ipAddress);

        if (m.matches()) {
            returnValue = true;
        }
        return returnValue;
    }


    public String isEthernetDHCPEnabled() {
        String bReturn = "";
        try {
            String sLine;
            Process p = Runtime.getRuntime().exec("getprop dhcp.eth0.result");
            BufferedReader input = new BufferedReader(new InputStreamReader(p.getInputStream()));
            sLine = input.readLine();
//            Log.d(TAG, sLine);

            if (sLine.contains("ok")) {
                bReturn = "DHCP";
            } else if (sLine.contains("failed")) {
                bReturn = "STATIC";
            }

        } catch (Exception ex) {
//            Log.d(TAG, "DHCPEnabled():" + ex.getMessage());
        }

        return bReturn;
    }

    private String getEthMode() {
        String mode = mEthernetManager.getConfiguration().toString();
        int iDHCP = mode.indexOf("DHCP");

        if (iDHCP == -1) {
            ethMode = "STATIC";
        } else {
            ethMode = "DHCP";
        }
        return ethMode;
    }

    private String getIpAddress() {
        String str = mEthernetManager.getConfiguration().toString();
        int value = str.indexOf("address ") + 8;
        int value2 = str.indexOf("/");
//        Log.d(TAG, "value: " + value + " value2: " + value2);
        return str.substring(value, value2);
    }

    private String getGateWay() {
        String str = mEthernetManager.getConfiguration().toString();
        int value = str.indexOf("Gateway ") + 8;
        int value2 = str.indexOf("DNS");
//        Log.d(TAG, "value: " + value + " value2: " + value2);
        String str2 = str.substring(value, value2);
        return str2.replaceAll(" ", "");
    }

    private String GetLocalIpAddress() {
        try {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements(); ) {
                NetworkInterface intf = en.nextElement();
                for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                    InetAddress inetAddress = enumIpAddr.nextElement();
                    if (inetAddress.isLoopbackAddress()) {
                        Log.i("IP Address", intf.getDisplayName() + "(loopback) | " + inetAddress.getHostAddress());
                    } else {
                        Log.i("IP Address", intf.getDisplayName() + " | " + inetAddress.getHostAddress());
                    }
                    if (!inetAddress.isLoopbackAddress() && inetAddress instanceof Inet4Address) {
                        return inetAddress.getHostAddress();

                    }
                }
            }
        } catch (SocketException ex) {
            return "ERROR Obtaining IP";
        }
        return "No IP Available";
    }


    void showMessage(final String sMessage) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                SettingsEthernet.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            if (_pd.isShowing()) {
//                                Log.d(TAG, "Nothing");
                            } else {
                                _pd.show();
                            }
                            _pd.setMessage(sMessage);
                        } catch (Exception ex) {
//                            Log.d(TAG, "Exception [Show Message]: ", ex);
                        }
                    }
                });
            }
        }).start();
    }

    void dismissMessage() {
        SettingsEthernet.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    if ((_pd != null) && (_pd.isShowing()) && (!SettingsEthernet.this.isFinishing()) && (!disposed)) {
                        _pd.dismiss();
                    }
                } catch (Exception ex) {
//                    Log.d(TAG, "Exception [Dismiss Message]: ", ex);
                }
            }
        });
    }


    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
        if (checkedId == R.id.mode_dhcp) {
//            Toast.makeText(getApplicationContext(), "DHCP MODE", Toast.LENGTH_SHORT).show();
            IpAddress.setEnabled(false);
            MaskAddress.setEnabled(false);
            GateWay.setEnabled(false);
            DNSAddress.setEnabled(false);
            dhcpMode.setNextFocusDownId(R.id.btn_input_network);
            inputBtn.setNextFocusUpId(R.id.mode_dhcp);
            clearBtn.setEnabled(false);
            ethMode = "DHCP";
            dhcpMode.requestFocus();
            dhcpMode.setFocusable(true);

        } else if (checkedId == R.id.mode_static) {
//            Toast.makeText(getApplicationContext(), "Static MODE", Toast.LENGTH_SHORT).show();
            IpAddress.setEnabled(true);
            MaskAddress.setEnabled(true);
            GateWay.setEnabled(true);
            DNSAddress.setEnabled(true);
            ethMode = "STATIC";
            clearBtn.setEnabled(true);
            if(IpAddress.getText().toString().isEmpty()){
                retrieveInfo();
            }
            staticMode.requestFocus();
            staticMode.setFocusable(true);

        }
    }

//    private void prepareListData() {
//        listDataHeader = new ArrayList<String>();
//        listDataChild = new HashMap<String, List<String>>();
//
//        // Adding child data
//        listDataHeader.add("Hanwha Techwin");
//        listDataHeader.add("Hikvision");
//        listDataHeader.add("Dahua");
//        listDataHeader.add("Bosch");
//        listDataHeader.add("Uniview");
//        listDataHeader.add("Axias");
//        listDataHeader.add("HoneyWell");
//        listDataHeader.add("Panasonic");
//        listDataHeader.add("Link Local Address");
//
//        // Adding child data
//        List<String> techwin = new ArrayList<String>();
//        techwin.add("192.168.1.180");
//        techwin.add("255.255.255.0");
//        techwin.add("192.168.1.1");
//
//
//        List<String> Hikvision = new ArrayList<String>();
//        Hikvision.add("192.168.1.190");
//        Hikvision.add("255.255.255.0");
//        Hikvision.add("192.168.1.1");
//
//        List<String> Dahua = new ArrayList<String>();
//        Dahua.add("192.168.0.180");
//        Dahua.add("255.255.255.0");
//        Dahua.add("192.168.0.1");
//
//        List<String> Bosch = new ArrayList<String>();
//        Bosch.add("192.168.0.180");
//        Bosch.add("255.255.255.0");
//        Bosch.add("192.168.0.1");
//
//        List<String> Uniview = new ArrayList<String>();
//        Uniview.add("192.168.0.190");
//        Uniview.add("255.255.255.0");
//        Uniview.add("192.168.0.1");
//
//        List<String> Axias = new ArrayList<String>();
//        Axias.add("192.168.0.180");
//        Axias.add("255.255.255.0");
//        Axias.add("192.168.0.1");
//
//        List<String> HoneyWell = new ArrayList<String>();
//        HoneyWell.add("192.168.1.180");
//        HoneyWell.add("255.255.255.0");
//        HoneyWell.add("192.168.1.1");
//
//        List<String> Panasonic = new ArrayList<String>();
//        Panasonic.add("192.168.0.180");
//        Panasonic.add("255.255.255.0");
//        Panasonic.add("192.168.0.1");
//
//        List<String> Local = new ArrayList<String>();
//        Local.add("169.254.1.2");
//        Local.add("255.255.0.0");
//
//
//        listDataChild.put(listDataHeader.get(0), techwin); // Header, Child data
//        listDataChild.put(listDataHeader.get(1), Hikvision);
//        listDataChild.put(listDataHeader.get(2), Dahua);
//        listDataChild.put(listDataHeader.get(3), Bosch);
//        listDataChild.put(listDataHeader.get(4), Uniview);
//        listDataChild.put(listDataHeader.get(5), Axias);
//        listDataChild.put(listDataHeader.get(6), HoneyWell);
//        listDataChild.put(listDataHeader.get(7), Panasonic);
//        listDataChild.put(listDataHeader.get(8), Local);
//
//    }

    private void ipInput() {
        InputFilter[] filters = new InputFilter[1];
        filters[0] = (source, start, end, dest, dstart, dend) -> {
            if (end > start) {
                String destTxt = dest.toString();
                String resultingTxt = destTxt.substring(0, dstart) +
                        source.subSequence(start, end) + destTxt.substring(dend);
                if (!resultingTxt.matches("^\\d{1,3}(\\." + "(\\d{1,3}(\\.(\\d{1,3}(\\.(\\d{1,3})?)?)?)?)?)?")) {
                    return "";
                } else {
                    String[] splits = resultingTxt.split("\\.");
                    for (int i = 0; i < splits.length; i++) {
                        if (Integer.valueOf(splits[i]) > 255) {
                            return "";
                        }
                    }
                }
            }
            return null;
        };
        IpAddress.setFilters(filters);
        MaskAddress.setFilters(filters);
        GateWay.setFilters(filters);
        DNSAddress.setFilters(filters);

    }

    @OnClick(R.id.btnAdd)
    void addClick() {
        hideKeyboard();
        String header = editText.getText().toString();
        boolean check = false;
        if(header.equals("")) {
            Toast.makeText(getApplicationContext(), getResources().getString(R.string.not_title), Toast.LENGTH_SHORT).show();
        } else {
            String[] strData = adapter.getAllTitle().toArray(new String[adapter.getAllTitle().size()]);
            String value;

            for(int i = 0; i<adapter.getCount(); i++) {
//                Log.d(TAG, "Value: " + strData[i] + " Count: " + count);

                value = strData[i];
//                Log.d(TAG, "Result Value: " + value);
                if(value.equals(header)){
                    check = true;
                }
            }

            if(!check) {
                ipAddr = IpAddress.getText().toString();
                netMask = MaskAddress.getText().toString();
                gateWay = GateWay.getText().toString();
                dnsServer = DNSAddress.getText().toString();
                addNetworkInfo(header, ipAddr, netMask, gateWay, dnsServer);

            } else {
                Toast.makeText(getApplicationContext(), getResources().getString(R.string.same_title), Toast.LENGTH_SHORT).show();

            }
        }





    }


    public void ethDelete(int position) {
        int count;

        count = adapter.getCount();
        if (count > 0) {
            adapter.removeItem(position);
            adapter.notifyDataSetChanged();

        }

    }




    public void addNetworkInfo(String header, String ipAddr, String netMask, String gateWay, String dnsAddr) {
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/", fileName);
        FileOutputStream fos = null;
        BufferedWriter bufwr = null;

        String value[] = {header+ "_@#@_" +ipAddr + "_@#@_" + netMask + "_@#@_" + gateWay + "_@#@_" + dnsAddr};
        saveArray(header, value);

        editText.setText("");

        try {
            fos = new FileOutputStream(file, true);
            bufwr = new BufferedWriter(new OutputStreamWriter(fos));
            bufwr.write(header + "_@#@_" + ipAddr + "_@#@_" + netMask + "_@#@_" + gateWay + "_@#@_" + dnsAddr + "\n");
            bufwr.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            if (bufwr != null) {
                bufwr.close();
            }
            if (fos != null) {
                fos.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        adapter.addItem(ContextCompat.getDrawable(this, R.drawable.ic_add_circle_black_24dp), header, ipAddr);
        adapter.notifyDataSetChanged();
    }

    private void delNetworkInfo(String msg) {
//        Log.d(TAG, "delete: " + msg);

        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/", fileName);
        File tempFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/", fileTemp);

        if (!file.isFile()) {
            return;
        }

        try {

            BufferedReader bufrd = new BufferedReader(new FileReader(file));
            PrintWriter pw = new PrintWriter(new FileWriter(tempFile));
            String line = null;

            while ((line = bufrd.readLine()) != null) {
//                Log.d(TAG, "delete line: " + line);
                if (!line.trim().equals(msg)) {
                    pw.println(line);
                    pw.flush();
                }
            }
            pw.close();
            bufrd.close();

            if (!file.delete()) {
                Log.d(TAG, "Could not delete file: " + !file.delete());
            }

            if(!tempFile.renameTo(file)){
                Log.d(TAG, "Could not rename file: " + !tempFile.renameTo(file));
            }

        } catch (IOException e) {
            e.printStackTrace();
        }


    }

    private void saveNetworkInfo(String[] items) {

        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/", fileName);
        FileWriter fw = null;
        BufferedWriter bufwr = null;


        try {
            fw = new FileWriter(file);
            bufwr = new BufferedWriter(fw);

            for (String str : items) {
//                Log.d(TAG, "Save: " + Arrays.toString(items));
                bufwr.write(str);
                bufwr.newLine();
            }
            bufwr.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            if (bufwr != null) {
                bufwr.close();
            }
            if (fw != null) {
                fw.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void readNetworkInfo() {
//        StringBuilder str = new StringBuilder();
//        try{
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ethernet/", fileName);
        FileReader fr = null;
        BufferedReader bufrd = null;
        String str;
        String header;
        String ipAddr;
        String netMask;
        String gateWay;
        String dnsAddr;

        if (file.exists()) {
            try {
                fr = new FileReader(file);
                bufrd = new BufferedReader(fr);
                while ((str = bufrd.readLine()) != null) {
                    if (str.equals("")) {
                        //TODO
                    } else {
                        String[] info = str.split("_@#@_");
                        header = info[0];
                        ipAddr = info[1];
                        netMask = info[2];
                        gateWay = info[3];
                        dnsAddr = info[4];

                        String value[] = {header+ "_@#@_" +ipAddr + "_@#@_" + netMask + "_@#@_" + gateWay + "_@#@_" + dnsAddr};
                        saveArray(header, value);
//                        Log.d(TAG, header + " / " + ipAddr + " / " + netMask + " / " + gateWay + " / " + dnsAddr);
                        adapter.addItem(ContextCompat.getDrawable(this, R.drawable.ic_add_circle_black_24dp), header, ipAddr);
                    }
                }
                bufrd.close();
                fr.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public void saveArray(String arrayName, String[] array) {
//        Log.d(TAG, "Save: " + arrayName + " , " + Arrays.toString(array));
        SharedPreferences prefs = getSharedPreferences(NETWORK_PREFERENCE, MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putInt(arrayName + "_size", array.length);
        for (int i = 0; i < array.length; i++)
            editor.putString(arrayName + "_" + i, array[i]);
        editor.apply();
    }

    public String[] loadArray(String arrayName) {
        SharedPreferences prefs = getSharedPreferences(NETWORK_PREFERENCE, MODE_PRIVATE);
        int size = prefs.getInt(arrayName + "_size", 0);
        String array[] = new String[size];
        for (int i = 0; i < size; i++)
            array[i] = prefs.getString(arrayName + "_" + i, null);
        return array;
    }
}
