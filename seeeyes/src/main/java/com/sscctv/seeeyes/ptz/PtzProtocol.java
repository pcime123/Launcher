package com.sscctv.seeeyes.ptz;

import java.util.Arrays;

/**
 * Created by trlim on 2015. 12. 28..
 *
 * PTZ 프로토콜 명령 코드 정의
 */
final class PtzProtocol {
    private static final int _BIT0 = 1;
    private static final int _BIT1 = (1 << 1);
    private static final int _BIT2 = (1 << 2);
    private static final int _BIT3 = (1 << 3);
    private static final int _BIT4 = (1 << 4);
    private static final int _BIT5 = (1 << 5);
    private static final int _BIT6 = (1 << 6);
    private static final int _BIT7 = (1 << 7);

    private static final char COMMAND_PAN_RIGHT  = _BIT0;
    private static final char COMMAND_PAN_LEFT   = _BIT1;
    private static final char COMMAND_TILT_UP    = _BIT2;
    private static final char COMMAND_TILT_DOWN  = _BIT3;
    private static final char COMMAND_ZOOM_TELE  = _BIT4;
    private static final char COMMAND_ZOOM_WIDE  = _BIT5;

    private static final int COMMAND_PTZ_UPRT	= _BIT2|_BIT0;
    private static final int COMMAND_PTZ_UPLT	= _BIT2|_BIT1;
    private static final int COMMAND_PTZ_DNRT	= _BIT3|_BIT0;
    private static final int COMMAND_PTZ_DNLT	= _BIT3|_BIT1;

    private static final char COMMAND_FOCUS_FAR  = _BIT5|_BIT4;
    private static final char COMMAND_FOCUS_NEAR = _BIT5|_BIT4|_BIT0;
    private static final int COMMAND_FOCUS_AUTO	= _BIT5|_BIT4|_BIT1;

    private static final int COMMAND_PRST_SET	= _BIT5|_BIT4|_BIT3|_BIT0;
    private static final int COMMAND_PRST_CALL	= _BIT5|_BIT4|_BIT3|_BIT1;
    private static final int COMMAND_NORTH_SET	= _BIT5|_BIT4|_BIT3|_BIT1|_BIT0;
    private static final int COMMAND_NORTH_GO	= _BIT5|_BIT4|_BIT3|_BIT2;

    private static final char COMMAND_MENU_RIGHT = _BIT6|_BIT0;
    private static final char COMMAND_MENU_LEFT  = _BIT6|_BIT1;
    private static final char COMMAND_MENU_UP    = _BIT6|_BIT2;
    private static final char COMMAND_MENU_DOWN  = _BIT6|_BIT3;
    private static final char COMMAND_MENU_ENTER = _BIT6|_BIT4;
    private static final char COMMAND_MENU_ESC   = _BIT6|_BIT5;
    private static final char COMMAND_MENU_ON    = _BIT6|_BIT5|_BIT4;
    private static final char COMMAND_MENU_OFF   = _BIT6|_BIT5|_BIT4|_BIT3;

    private static final int COMMAND_PTZ_STOP    = _BIT7;
    private static final int COMMAND_CS_ERR		= _BIT7|_BIT3;
    private static final char COMMAND_PTZ_END    = 0xff;

    // ptz_protocol.h에 있는 PtzProtocol의 값과 같아야 함
    private static final int PROTOCOL_PANASONIC_C = 8;
    private static final int PROTOCOL_PANASONIC_N = 9;

    public static final int PACKET_SIZE_MAX = 32;

    public static char controlToCommand(char control) {
        char command;

        switch (control) {
            case PtzControl.PAN_LEFT:
                command = COMMAND_PAN_LEFT;
                break;

            case PtzControl.PAN_RIGHT:
                command = COMMAND_PAN_RIGHT;
                break;

            case PtzControl.TILT_UP:
                command = COMMAND_TILT_UP;
                break;

            case PtzControl.TILT_DOWN:
                command = COMMAND_TILT_DOWN;
                break;

            case PtzControl.ZOOM_IN:
                command = COMMAND_ZOOM_TELE;
                break;

            case PtzControl.ZOOM_OUT:
                command = COMMAND_ZOOM_WIDE;
                break;

            case PtzControl.FOCUS_FAR:
                command = COMMAND_FOCUS_FAR;
                break;

            case PtzControl.FOCUS_NEAR:
                command = COMMAND_FOCUS_NEAR;
                break;

            case PtzControl.STOP:
                command = COMMAND_PTZ_STOP;
                break;

            case PtzControl.MENU_UP:
                command = COMMAND_MENU_UP;
                break;

            case PtzControl.MENU_DOWN:
                command = COMMAND_MENU_DOWN;
                break;

            case PtzControl.MENU_LEFT:
                command = COMMAND_MENU_LEFT;
                break;

            case PtzControl.MENU_RIGHT:
                command = COMMAND_MENU_RIGHT;
                break;

            case PtzControl.MENU_ON:
                command = COMMAND_MENU_ON;
                break;

            case PtzControl.MENU_ENTER:
                command = COMMAND_MENU_ENTER;
                break;

            case PtzControl.MENU_ESC:
                command = COMMAND_MENU_ESC;
                break;

            default:
                return COMMAND_PTZ_END;
        }

        return command;
    }

    public static String commandToString(char key) {
        switch (key) {
            case COMMAND_PTZ_STOP:	return "STOP";
            case COMMAND_MENU_OFF:	return "MOFF";
            case COMMAND_MENU_ON:	return "MON ";
            case COMMAND_MENU_ESC:	return "ESC ";
            case COMMAND_MENU_ENTER:return "ENT ";
            case COMMAND_MENU_DOWN:	return "M DN";
            case COMMAND_MENU_UP:	return "M UP";
            case COMMAND_MENU_LEFT:	return "M LT";
            case COMMAND_MENU_RIGHT:return "M RT";
            case COMMAND_FOCUS_NEAR:return "   N";
            case COMMAND_FOCUS_FAR:	return "   F";
            case COMMAND_PTZ_END:	break;
            default:
                if (0 != (key & COMMAND_PAN_LEFT))		return "L   ";
                else if (0 != (key & COMMAND_PAN_RIGHT))	return "R   ";
                if (0 != (key & COMMAND_TILT_UP))		return " U  ";
                else if (0 != (key & COMMAND_TILT_DOWN))	return " D  ";
                if (0 != (key & COMMAND_ZOOM_TELE))		return "  T ";
                else if (0 != (key & COMMAND_ZOOM_WIDE))	return "  W ";
                break;
        }

        return "";
    }

    private static final char[] hexArray = "0123456789ABCDEF".toCharArray();

    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 3];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 3] = hexArray[v >>> 4];
            hexChars[j * 3 + 1] = hexArray[v & 0x0F];
            hexChars[j * 3 + 2] = ' ';
        }
        return new String(hexChars);
    }

    public static String packetToString(int protocol, byte[] packet) {
        if ((PtzProtocol.PROTOCOL_PANASONIC_C == protocol) || (PtzProtocol.PROTOCOL_PANASONIC_N == protocol)) {
            return new String(Arrays.copyOfRange(packet, 1, packet.length));
        } else {
            return bytesToHex(packet);
        }
    }
}
