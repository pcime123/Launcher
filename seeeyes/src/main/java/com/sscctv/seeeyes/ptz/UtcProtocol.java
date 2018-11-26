package com.sscctv.seeeyes.ptz;

/**
 * Created by trlim on 2016. 3. 17..
 *
 * UTC 프로토콜 관련 공통 정의
 */
public class UtcProtocol {
    public static final int TYPE_NONE = 0;
    public static final int TYPE_CVBS = 1;
    public static final int TYPE_TVI = 2;
    public static final int TYPE_AHD = 3;
    public static final int TYPE_CVI = 4;
    public static final int TYPE_AUTO = 5;

    public static final int CVBS_PELCO_C = 0;
    public static final int CVBS_A1_CCVC = 1;

    public static final int TVI_HIK_VISION0 = 0;
    public static final int TVI_HIK_VISION1 = 1;
    public static final int TVI_PELCO_C = 2;

    public static final int AHD_A_CP = 10;
    public static final int AHD_A_CP_720P = 11;
    public static final int AHD_A_CP_CVBS = 12;

    public static final int CVI_DAHUA0 = 20;
    public static final int CVI_DAHUA1 = 21;
    public static final int CVI_DAHUA2 = 22;
}
