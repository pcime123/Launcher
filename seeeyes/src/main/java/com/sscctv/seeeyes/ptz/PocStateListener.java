package com.sscctv.seeeyes.ptz;

/**
 * Created by trlim on 2016. 3. 2..
 *
 * PoC 상태를 분석해 주는 클래스
 */
public abstract class PocStateListener extends McuCommandListener {
    @Override
    public void onMcuCommand(char cmd, char data) {
        if (cmd == 'V') {
            switch (data) {
                case 'O':
                    onPocStarted();
                    break;

                default:
                    onPocStateChange(data);
                    break;

                case 'X':
                    onPocStopped();
                    break;

                case 'N':
                    onPocNotSupported();
                    break;
            }
        }
    }

    public abstract void onPocStarted();
    public abstract void onPocStateChange(int state);
    public abstract void onPocStopped();
    public abstract void onPocNotSupported();
}
