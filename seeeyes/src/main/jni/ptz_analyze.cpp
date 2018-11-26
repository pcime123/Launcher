////////////////////////////////////////////////////////////////////////////////////////////////////
// PTZ Protocol Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
// Common
#include "common.h"
#include "define.h"

#include "ptz_protocol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

// 2바이트를 수신하여 1바이트의 hex 코드 값으로 변환
// ASCII 데이터 바이트로 통신하는 프로토콜 대응
static BYTE ascii_to_hex(BYTE x, BYTE y)
{
    BYTE tmp;

    if(x<0x3a)		tmp = (x-0x30)<<4;
    else if(x<0x47) tmp = (x-0x37)<<4;
    else			tmp = (x-0x57)<<4;

    if(y<0x3a)		tmp |= (y-0x30);
    else if(y<0x47)	tmp |= (y-0x37);
    else			tmp |= (y-0x57);

    return tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Compony Protocol Data Analyze Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
static void dec_pkt_D_MAX(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,chksum_data;
	WORD checksum = 0;

	for(i=0;i<10;i++){ checksum += pkt[i];	chksum_data=(0x2020 - checksum);}
	if(pkt[10] == chksum_data)
	{
		*ch_id = pkt[2];

		if((pkt[3] == 0x00) && (pkt[5] == 0x00) && (pkt[6] == 0x00) && (pkt[7] == 0x00)){
			if(pkt[4] == 0xb1)		*key_data = MENU_ON;
			else if(pkt[4] == 0x00)	*key_data = PTZ_STOP;
		}
		else if(pkt[3] == 0x02)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0x01)		*key_data = FOCUS_FAR;
		else{
			if(pkt[4] & 0x02)		*key_data |= PAN_RIGHT;
			else if(pkt[4] & 0x04)	*key_data |= PAN_LEFT;
			if(pkt[4] & 0x08)		*key_data |= TILT_UP;
			else if(pkt[4] & 0x10)	*key_data |= TILT_DOWN;

			if(pkt[4] & 0x20)		*key_data |= ZOOM_TELE;
			else if(pkt[4] & 0x40)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_DY_255RXC(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if((pkt[0] == 'a') && (pkt[1] == 't'))
	{
		*ch_id = ((pkt[2]-0x30)*100) + ((pkt[3]-0x30)*10) + (pkt[4]-0x30) ;

		if((pkt[5] == 'j') && (pkt[6] == 0x0a) && (pkt[7] == 0x0d) && (pkt[8] == 'a') && (pkt[9] == 't'))
			*key_data = PTZ_STOP;

		else if(((pkt[6]&0xf0) == 0x30) && (pkt[7] == '0') && (pkt[8] == 0x0a) && (pkt[9] == 0x0d)){
			if(pkt[5] == 'r'){
				if(pkt[6] & 0x01)		*key_data = ZOOM_TELE;
				else if(pkt[6] & 0x02)	*key_data = ZOOM_WIDE;
				else if(pkt[6] & 0x08)	*key_data = FOCUS_NEAR;
				else if(pkt[6] & 0x04)	*key_data = FOCUS_FAR;
			}
			else if(pkt[5] == 'm'){
				if(pkt[6] & 0x08)		*key_data = PAN_RIGHT;
				else if(pkt[6] & 0x04)	*key_data = PAN_LEFT;
				if(pkt[6] & 0x01)		*key_data = TILT_UP;
				else if(pkt[6] & 0x02)	*key_data = TILT_DOWN;
			}
		}
	}
}

static void dec_pkt_FineSystem(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if((pkt[0] == 0xaa) && (pkt[1] == 'Z'))
	{
		*ch_id = pkt[3]-0x30;

		if(pkt[2] == 0x00)			*key_data = PTZ_STOP;
		else if(pkt[2] == 0x04)		*key_data = FOCUS_NEAR;
		else if(pkt[2] == 0x02)		*key_data = FOCUS_FAR;
		else{
			if(pkt[2] & 0x40)		*key_data |= PAN_RIGHT;
			else if(pkt[2] & 0x20)	*key_data |= PAN_LEFT;
			if(pkt[2] & 0x10)		*key_data |= TILT_UP;
			else if(pkt[2] & 0x80)	*key_data |= TILT_DOWN;
			if(pkt[2] & 0x01)		*key_data |= ZOOM_TELE;
			else if(pkt[2] & 0x08)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_Honeywell(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=0;i<6;i++)	checksum += pkt[i];
	if((pkt[5] == 0x55) && (pkt[6] == checksum))
	{
		*ch_id = pkt[1];

		if((pkt[2] == 0x19) && (pkt[3] == 0x80) && (pkt[4] == 0x80))	*key_data = MENU_ON;
		else if(pkt[2] == 0x10){
			if((pkt[3] == 0x0d) && (pkt[4] == 0x88))	*key_data = PTZ_STOP;
			else if(pkt[3] == 0x03)			*key_data = FOCUS_NEAR;
			else if(pkt[3] == 0x04)			*key_data = FOCUS_FAR;
			else{
				if((pkt[4]&0xf0)<0x80)		*key_data |= PAN_LEFT;
				else if((pkt[4]&0xf0)>0x80)	*key_data |= PAN_RIGHT;
				if((pkt[4]&0x0f)<0x08)		*key_data |= TILT_UP;
				else if((pkt[4]&0x0f)>0x08)	*key_data |= TILT_DOWN;
				if(pkt[3] == 0x0c)			*key_data |= ZOOM_WIDE;
				else if(pkt[3] == 0x0e)		*key_data |= ZOOM_TELE;
			}
		}
	}
}

static void dec_pkt_VRx_2201(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if(pkt[1] == 0x02)
	{
		*ch_id = pkt[0];

		if((pkt[2] == 0x80) && (pkt[3] == 0x80) && (pkt[4] == 0x80) && (pkt[5] == 0x80))
			*key_data = PTZ_STOP;
		else{
			if(pkt[4] == 0x3f)			*key_data = FOCUS_NEAR;
			else if(pkt[4] == 0xc1)		*key_data = FOCUS_FAR;
			else{
				if(pkt[2] == 0x3f)		*key_data |= PAN_LEFT;
				else if(pkt[2] == 0xc1)	*key_data |= PAN_RIGHT;
				if(pkt[3] == 0xc1)		*key_data |= TILT_UP;
				else if(pkt[3] == 0x3f)	*key_data |= TILT_DOWN;
				if(pkt[5] == 0x3f)		*key_data |= ZOOM_TELE;
				else if(pkt[5] == 0xc1)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_Panasonic_C(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)			// Panasonic conventional Protocol
{
	*ch_id = ((pkt[3]&0x0f)*10) + (pkt[4]&0x0f);

	if((pkt[10] == '2') && (pkt[11] == '0') && (pkt[12] == '2'))
	{
		if((pkt[13] == '1') && (pkt[14] == '3')
		&&((pkt[15] == '2') || (pkt[15] == '3') || (pkt[15] == '4') || (pkt[15] == '5') || (pkt[15] == '6'))){
			switch(pkt[16]){
				case '4':	*key_data = PTZ_STOP;				break;
				case '8':	*key_data = PAN_LEFT;				break;
				case '9':	*key_data = PAN_LEFT|TILT_UP;		break;
				case 'A':	*key_data = TILT_UP;				break;
				case 'B':	*key_data = PAN_RIGHT|TILT_UP;		break;
				case 'C':	*key_data = PAN_RIGHT;				break;
				case 'D':	*key_data = PAN_RIGHT|TILT_DOWN;	break;
				case 'E':	*key_data = TILT_DOWN;				break;
				case 'F':	*key_data = PAN_LEFT|TILT_DOWN;		break;
			}
		}
		if((pkt[13] == '1') && (pkt[14] == '2') && ((pkt[15] == '2') || (pkt[15] == '6'))){
			switch(pkt[16]){
				case '4':	*key_data = PTZ_STOP;				break;
				case '8':	*key_data = ZOOM_TELE;				break;
				//case '9':	*key_data = ZOOM_TELE|FOCUS_FAR;	break;
				case 'A':	*key_data = FOCUS_FAR;				break;
				//case 'B':	*key_data = ZOOM_WIDE|FOCUS_FAR;	break;
				case 'C':	*key_data = ZOOM_WIDE;				break;
				//case 'D':	*key_data = ZOOM_WIDE|FOCUS_NEAR;	break;
				case 'E':	*key_data = FOCUS_NEAR;				break;
				//case 'F':	*key_data = ZOOM_TELE|FOCUS_NEAR;	break;
			}
		}
	}
	else if((pkt[10] == '0') && (pkt[11] == '0') && (pkt[12] == '2'))
	{
		if((pkt[13] == '1') && (pkt[14] == '9') && (pkt[15] == '4')){
			switch(pkt[16]){
				case '0':	*key_data = MENU_ON;	break;
				case '1':	*key_data = MENU_OFF;	break;
				case 'A':	*key_data = MENU_ENTER;	break;
				case '2':	*key_data = MENU_UP;	break;
				case '3':	*key_data = MENU_RIGHT;	break;
				case '4':	*key_data = MENU_DOWN;	break;
				case '5':	*key_data = MENU_LEFT;	break;
				case 'F':	*key_data = PTZ_STOP;	break;
			}
		}
	}

	if(*key_data == 0)	*key_data = PTZ_END;
}

static void dec_pkt_Panasonic_N(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)			// Panasonic New Protocol
{
	*ch_id = ((pkt[3]&0x0f)*10) + (pkt[4]&0x0f);

	if((pkt[10] == '9') && (pkt[11] == '0') && (pkt[12] == '2'))
	{
		if((pkt[13] == '8') && (pkt[14] == '1') && (pkt[15] == '0') && (pkt[16] == '0'))	*key_data = PTZ_STOP;
		else if((pkt[13] == '4') && (pkt[14] == '0') && (pkt[15] == '0')){
			if(pkt[16] == '3')		*key_data = FOCUS_NEAR;
			else if(pkt[16] == '7')	*key_data = FOCUS_FAR;
			else if(pkt[16] == '8')	*key_data = PTZ_STOP;
		}
		else{
			switch(pkt[14]){
				case '4':	*key_data = PTZ_STOP;				break;
				case '8':	*key_data = PAN_LEFT;				break;
				case '9':	*key_data = PAN_LEFT|TILT_UP;		break;
				case 'A':	*key_data = TILT_UP;				break;
				case 'B':	*key_data = PAN_RIGHT|TILT_UP;		break;
				case 'C':	*key_data = PAN_RIGHT;				break;
				case 'D':	*key_data = PAN_RIGHT|TILT_DOWN;	break;
				case 'E':	*key_data = TILT_DOWN;				break;
				case 'F':	*key_data = PAN_LEFT|TILT_DOWN;		break;
			}

			if(pkt[13] == '3')		*key_data |= ZOOM_TELE;
			else if(pkt[13] == '7')	*key_data |= ZOOM_WIDE;
		}
	}
	else if((pkt[10] == '0') && (pkt[11] == '0') && (pkt[12] == '2'))
	{
		if((pkt[13] == '1') && (pkt[14] == '9') && (pkt[15] == '4')){
			switch(pkt[16]){
				//case '0':	*key_data = MENU_ON;	break;
				//case '1':	*key_data = MENU_ESC;	break;
				case 'A':	*key_data = MENU_ENTER;	break;
				case '2':	*key_data = MENU_UP;	break;
				case '3':	*key_data = MENU_RIGHT;	break;
				case '4':	*key_data = MENU_DOWN;	break;
				case '5':	*key_data = MENU_LEFT;	break;
				case 'F':	*key_data = PTZ_STOP;	break;
			}
		}
		else if((pkt[13] == '1') && (pkt[14] == '9') && (pkt[15] == 'C')){
			switch(pkt[16]){
				case '0':	*key_data = MENU_ON;	break;
				case '1':	*key_data = MENU_OFF;	break;
			}
		}
	}

	if(*key_data == 0)	*key_data = PTZ_END;
}

static void dec_pkt_Pelco_D(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=1;i<6;i++) checksum += pkt[i];
	if(pkt[6] == checksum)
	{
		*ch_id = pkt[1];

		if((pkt[3] != 0x25) && (pkt[3] != 0x27)){
			if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
				*key_data = PTZ_STOP;
			else if((pkt[3] == 0x03) && (pkt[5] == 0x5f))	*key_data = MENU_ON;
			else if((pkt[2] == 0x02) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = MENU_ENTER;
		 	else if((pkt[2] == 0x04) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = MENU_ESC;
		 	else{
				if(pkt[2] == 0x01)			*key_data = FOCUS_NEAR;
				else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;
				else{
					if(pkt[3] & 0x02)		*key_data |= PAN_RIGHT;
					else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
					if(pkt[3] & 0x08)		*key_data |= TILT_UP;
					else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
					if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
					else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
				}
			}
		}
	}
}

static void dec_pkt_Pelco_D_CNB1(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)				//Pelco_D CNB1 = Box,Zoom Camera
{
	BYTE i, checksum = 0;

	for(i=1;i<6;i++) checksum += pkt[i];
	if(pkt[6] == checksum)
	{
		*ch_id = pkt[1];

		if((pkt[3] != 0x25) && (pkt[3] != 0x27)){
			if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
				*key_data = PTZ_STOP;
			else if((pkt[3] == 0x23) && (pkt[5] == 0x5f))	*key_data = MENU_ESC;
			else if((pkt[3] == 0x07) && (pkt[5] == 0x5f))	*key_data = MENU_ENTER;
		 	else{
				if(pkt[2] == 0x01)			*key_data = FOCUS_NEAR;
				else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;
				else{
					if(pkt[3] & 0x02)		*key_data |= PAN_RIGHT;
					else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
					if(pkt[3] & 0x08)		*key_data |= TILT_UP;
					else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
					if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
					else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
				}
			}
		}
	}
}

static void dec_pkt_Pelco_D_CNB2(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)				//Pelco_D CNB2 = Box,Zoom Camera
{
	BYTE i, checksum = 0;

	for(i=1;i<6;i++) checksum += pkt[i];
	if(pkt[6] == checksum)
	{
		*ch_id = pkt[1];

		if((pkt[3] != 0x25) && (pkt[3] != 0x27)){
			if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
				*key_data = PTZ_STOP;
			else if((pkt[3] == 0x07) && (pkt[5] == 0x5f))	*key_data = MENU_ON;
		 	else{
				if(pkt[2] == 0x01)			*key_data = FOCUS_NEAR;		// MENU_ENTER
				else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;		// MENU_ESC
				else{
					if(pkt[3] & 0x02)		*key_data |= PAN_RIGHT;
					else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
					if(pkt[3] & 0x08)		*key_data |= TILT_UP;
					else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
					if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
					else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
				}
			}
		}
	}
}

static void dec_pkt_Pelco_P(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	checksum = pkt[0];
	for(i=1;i<7;i++) checksum ^= pkt[i];
	if(pkt[7] == checksum)
	{
		*ch_id = pkt[1] + 1;

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
			*key_data = PTZ_STOP;
		else if((pkt[3] == 0x03) && (pkt[5] == 0x5f))	*key_data = MENU_ON;
		else if((pkt[2] == 0x04) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = MENU_ENTER;
	 	else if((pkt[2] == 0x08) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = MENU_ESC;
	 	else{
			if(pkt[2] == 0x01)			*key_data = FOCUS_FAR;
			else if(pkt[2] == 0x02)		*key_data = FOCUS_NEAR;
			else{
				if(pkt[3] & 0x02)		*key_data |= PAN_RIGHT;
				else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
				if(pkt[3] & 0x08)		*key_data |= TILT_UP;
				else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
				if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
				else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_Samsung(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i;
	WORD checksum = 0;

	for(i=1;i<8;i++)	checksum += pkt[i];
	if(pkt[8] == ((~checksum)&0xff))
	{
		*ch_id = pkt[2];

		if((pkt[4] == 0x00) && (pkt[5] == 0x00)&& (pkt[6] == 0x00)&& (pkt[7] == 0x00)){
			*key_data = PTZ_STOP;
		}
		else{
			if((pkt[6] != 0xff) && (pkt[7] != 0xff)){
				if(pkt[4] == 0x02)			*key_data = FOCUS_NEAR;
				else if(pkt[4] == 0x01)		*key_data = FOCUS_FAR;
				else{
					if(pkt[5] & 0x08)		*key_data |= TILT_DOWN;
					else if(pkt[5] & 0x04)	*key_data |= TILT_UP;
					if(pkt[5] & 0x02)		*key_data |= PAN_RIGHT;
					else if(pkt[5] & 0x01)	*key_data |= PAN_LEFT;
					if(pkt[4] & 0x40)		*key_data |= ZOOM_WIDE;
					else if(pkt[4] & 0x20)	*key_data |= ZOOM_TELE;
				}
			}
			else if((pkt[3] == 0x03) && (pkt[6] == 0xff) && (pkt[7] == 0xff)){
				if((pkt[4] == 0x18) && (pkt[5] == 0xFF))	*key_data = MENU_ENTER;
				if((pkt[4] == 0x17) && (pkt[5] == 0x00))	*key_data = MENU_ESC;
				if((pkt[4] == 0x17) && (pkt[5] == 0x01))	*key_data = MENU_ON;
			}
		}
	}
}

static void dec_pkt_Techwin(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i;
	WORD checksum = 0;

	for(i=1;i<9;i++)	checksum += pkt[i];
	if(pkt[10] == ((~checksum)&0xff))
	{
		*ch_id = pkt[1];

		if((pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))		*key_data = PTZ_STOP;
		else if((pkt[3] == 0x00) && (pkt[4] == 0xb1) && (pkt[5] == 0x00))	*key_data = MENU_ON;
		else if((pkt[3] == 0x00) && (pkt[4] == 0xb1) && (pkt[5] == 0x01))	*key_data = MENU_OFF;
		else if((pkt[3] == 0x01) && ((pkt[4] == 0x00) || (pkt[4] == 0xb1)) && (pkt[5] == 0x06))	*key_data = MENU_ENTER;
		else if((pkt[3] == 0x02) && ((pkt[4] == 0x00) || (pkt[4] == 0xb1)) && (pkt[5] == 0x07))	*key_data = MENU_ESC;
		else if((pkt[4]&0x01) == 0x00){
			if(pkt[3] == 0x02)			*key_data = FOCUS_NEAR;
			else if(pkt[3] == 0x01)		*key_data = FOCUS_FAR;
			else{
				if(pkt[4] & 0x02)		*key_data |= PAN_RIGHT;
				else if(pkt[4] & 0x04)	*key_data |= PAN_LEFT;
				if(pkt[4] & 0x08)		*key_data |= TILT_UP;
				else if(pkt[4] & 0x10)	*key_data |= TILT_DOWN;
				if(pkt[4] & 0x20)		*key_data |= ZOOM_TELE;
				else if(pkt[4] & 0x40)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_Sungjin_SJ_100(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE checksum;

	checksum = pkt[0] + pkt[1] + pkt[2] + pkt[3];

	if((pkt[0] == '@') && (pkt[1] == 'C') && (pkt[4] == checksum))
	{
		*ch_id = pkt[2];

		if(pkt[3] == 0x00)			*key_data = PTZ_STOP;
		else if(pkt[3] == 0x40)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0x30)		*key_data = FOCUS_FAR;
		else{
			if(pkt[3] & 0x08)		*key_data |= PAN_RIGHT;
			else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
			if(pkt[3] & 0x01)		*key_data |= TILT_UP;
			else if(pkt[3] & 0x02)	*key_data |= TILT_DOWN;
			if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
			else if(pkt[3] & 0x10)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_Sungjin_SJ_1000(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	checksum = pkt[1];
	for(i=2;i<7;i++) checksum ^= pkt[i];
	if(pkt[7] == checksum)
	{
		*ch_id = (((WORD)pkt[1]&0x03)<<8)|pkt[2];

		if((pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00) && (pkt[6] == 0x00))
			*key_data = PTZ_STOP;
		else if((pkt[1]&0xf3) == 0x04){
			if(pkt[5]&0xf0){
				if(pkt[5]&0x80)		*key_data = FOCUS_NEAR;
				else				*key_data = FOCUS_FAR;
			}
			else{
				if(pkt[3]){
					if(pkt[3]&0x80)	*key_data |= PAN_RIGHT;
					else			*key_data |= PAN_LEFT;
				}
				if(pkt[4]){
					if(pkt[4]&0x80)	*key_data |= TILT_UP;
					else			*key_data |= TILT_DOWN;
				}
				if(pkt[5]&0x0f){
					if(pkt[5]&0x08)	*key_data |= ZOOM_TELE;
					else			*key_data |= ZOOM_WIDE;
				}
			}
		}
	}
}

static void dec_pkt_Sysmania(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=0;i<5;i++) checksum += pkt[i];

	if(pkt[5] == checksum)
	{
		*ch_id = ((pkt[0]-0x10)*100) + (pkt[1]-0x20) + ((pkt[2]-0x30)*10);

		if((pkt[3] == 0x00) && (pkt[4] == 0x40))	*key_data = PTZ_STOP;
		else if(pkt[3] == 0xac)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0xaf)		*key_data = FOCUS_FAR;
		else{
			if(pkt[4] & 0x08)		*key_data |= PAN_RIGHT;
			else if(pkt[4] & 0x04)	*key_data |= PAN_LEFT;
			if(pkt[4] & 0x01)		*key_data |= TILT_UP;
			else if(pkt[4] & 0x02)	*key_data |= TILT_DOWN;
			if(pkt[3] == 0xa3)		*key_data |= ZOOM_TELE;
			else if(pkt[3] == 0xa0)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_Vicon_Stn(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if(((pkt[0]&0xf0) == 0x80) && ((pkt[1]&0xf0) == 0x10))
	{
		*ch_id = ((pkt[0]&0x0f)<<4) + (pkt[1]&0x0f);

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))	*key_data = PTZ_STOP;
		else if(pkt[3] == 0x10)		*key_data = FOCUS_FAR;
		else if(pkt[3] == 0x08)		*key_data = FOCUS_NEAR;
		else{
			if(pkt[2] & 0x40)		*key_data |= PAN_LEFT;
			else if(pkt[2] & 0x20)	*key_data |= PAN_RIGHT;
			if(pkt[2] & 0x10)		*key_data |= TILT_UP;
			else if(pkt[2] & 0x08)	*key_data |= TILT_DOWN;

			if(pkt[3] & 0x40)		*key_data |= ZOOM_WIDE;
			else if(pkt[3] & 0x20)	*key_data |= ZOOM_TELE;
		}
	}
}

static void dec_pkt_Vicon_Ext(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if(((pkt[0]&0xf0) == 0x80) && ((pkt[1]&0xf0) == 0x50))
	{
		*ch_id = ((pkt[0]&0x0f)<<4) + (pkt[1]&0x0f);

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00) && (pkt[6] == 0x00) && (pkt[7] == 0x00))
			*key_data = PTZ_STOP;
		else if(pkt[3] == 0x10)		*key_data = FOCUS_FAR;
		else if(pkt[3] == 0x08)		*key_data = FOCUS_NEAR;
		else{
			if(pkt[2] & 0x40)		*key_data |= PAN_LEFT;
			else if(pkt[2] & 0x20)	*key_data |= PAN_RIGHT;
			if(pkt[2] & 0x10)		*key_data |= TILT_UP;
			else if(pkt[2] & 0x08)	*key_data |= TILT_DOWN;

			if(pkt[3] & 0x40)		*key_data |= ZOOM_WIDE;
			else if(pkt[3] & 0x20)	*key_data |= ZOOM_TELE;
		}
	}
}

static void dec_pkt_PCS_35(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	checksum = pkt[1];
	for(i=2;i<11;i++) checksum ^= pkt[i];	// Checksum	: ID ~ ETX 까지 XOR

	if((pkt[11] == checksum) && (pkt[3] == 0x02) && (pkt[10] == 0x03))
	{
		*ch_id = pkt[2] - 0x30;			// CH(Camera ID):31h ~ 93h (CH 1 ~ 99)

		if((pkt[5] == 0x80) && (pkt[6] == 0x80) && (pkt[7] == 0x80) && (pkt[8] == 0x80) && (pkt[9] == 0x80))
			*key_data = PTZ_STOP;
		else if(pkt[6] == 0x84)		*key_data = FOCUS_FAR;
		else if(pkt[6] == 0x88)		*key_data = FOCUS_NEAR;
		else{
			if(pkt[7] & 0x08)		*key_data |= PAN_LEFT;
			else if(pkt[7] & 0x04)	*key_data |= PAN_RIGHT;
			if(pkt[7] & 0x01)		*key_data |= TILT_UP;
			else if(pkt[7] & 0x02)	*key_data |= TILT_DOWN;

			if(pkt[6] & 0x20)		*key_data |= ZOOM_WIDE;
			else if(pkt[6] & 0x10)	*key_data |= ZOOM_TELE;
		}
	}
}

static void dec_pkt_PCS_358(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	checksum = pkt[1];
	for(i=2;i<14;i++) checksum ^= pkt[i];	// Checksum	: ID ~ ETX 까지 XOR

	if((pkt[14] == checksum) && (pkt[6] == 0x02) && (pkt[13] == 0x03))
	{
		*ch_id = pkt[2] - 0x30;			// CH(Camera ID):31h ~ 93h (CH 1 ~ 99)

		if((pkt[8] == 0x80) && (pkt[9] == 0x80) && (pkt[10] == 0x80) && (pkt[11] == 0x80) && (pkt[12] == 0x80))
			*key_data = PTZ_STOP;
		else if(pkt[9] == 0x84)		*key_data = FOCUS_FAR;
		else if(pkt[9] == 0x88)		*key_data = FOCUS_NEAR;
		else{
			if(pkt[10] & 0x08)		*key_data |= PAN_LEFT;
			else if(pkt[10] & 0x04)	*key_data |= PAN_RIGHT;
			if(pkt[10] & 0x01)		*key_data |= TILT_UP;
			else if(pkt[10] & 0x02)	*key_data |= TILT_DOWN;

			if(pkt[9] & 0x20)		*key_data |= ZOOM_WIDE;
			else if(pkt[9] & 0x10)	*key_data |= ZOOM_TELE;
		}
	}
}

static void dec_pkt_NIKO(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=0;i<7;i++) checksum += pkt[i];

	if(pkt[7] == checksum)						// Byte1~Byte7을 더한 값(overflow 무시)
	{
		*ch_id = pkt[6];

		switch(pkt[1])
		{
			case 0x5f:
				switch(pkt[2]){
					case 0x25:	*key_data = MENU_ON;	break;
					case 0x27:	*key_data = MENU_ENTER;	break;
					case 0x28:	*key_data = MENU_ESC;	break;
					case 0x4f:	*key_data = MENU_UP;	break;
					case 0x50:	*key_data = MENU_DOWN;	break;
					case 0x39:	*key_data = FOCUS_NEAR;	break;
					case 0x38:	*key_data = FOCUS_FAR;	break;
					case 0x01:	*key_data = ZOOM_TELE;	break;
					case 0x03:	*key_data = ZOOM_WIDE;	break;
				}
			break;
			case 0x97:	*key_data = PAN_LEFT;	break;
			case 0x99:	*key_data = PAN_RIGHT;	break;
			case 0x9b:	*key_data = TILT_UP;	break;
			case 0x9d:	*key_data = TILT_DOWN;	break;
		}
	}
}

static void dec_pkt_TOKINA_DMP(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	if(pkt[1] == 0x02)
	{
		*ch_id = pkt[0];

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))	*key_data = PTZ_STOP;
		else if(pkt[4] == 0x80)		*key_data = FOCUS_NEAR;
		else if(pkt[4] == 0x40)		*key_data = FOCUS_FAR;
		else{
			if(pkt[3] & 0x80)		*key_data |= PAN_RIGHT;
			else if(pkt[3] & 0x40)	*key_data |= PAN_LEFT;
			if(pkt[2] & 0x80)		*key_data |= TILT_UP;
			else if(pkt[2] & 0x40)	*key_data |= TILT_DOWN;
			if(pkt[5] & 0x80)		*key_data |= ZOOM_TELE;
			else if(pkt[5] & 0x40)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_Multi_X(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=0;i<7;i++) checksum += pkt[i];
	if(pkt[7] == checksum)
	{
		*ch_id = pkt[2];

		if((pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))	*key_data = PTZ_STOP;
		else if(pkt[3] == 0xdc){
			if(pkt[5] == 0x00)			*key_data = MENU_ESC;
			else if(pkt[5] == 0x01)		*key_data = MENU_ON;
			else if(pkt[5] == 0x02)		*key_data = MENU_ENTER;
		}
		else{
			if(pkt[4] == 0x02)			*key_data = FOCUS_NEAR;
			else if(pkt[4] == 0x01)		*key_data = FOCUS_FAR;
			else{
				if(pkt[4] & 0x80)		*key_data |= PAN_LEFT;
				else if(pkt[4] & 0x40)	*key_data |= PAN_RIGHT;
				if(pkt[4] & 0x20)		*key_data |= TILT_UP;
				else if(pkt[4] & 0x10)	*key_data |= TILT_DOWN;
				if(pkt[4] & 0x08)		*key_data |= ZOOM_TELE;
				else if(pkt[4] & 0x04)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_LG_LPT_A100L(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=0;i<5;i++) checksum += pkt[i];
	checksum &= 0x7f;

	if((pkt[4] == 0x03) && (pkt[5] == checksum))			// Check Sum : (sum of Byte1~Byte5) & 0x7f
	{
		*ch_id = pkt[1];

		switch(pkt[2]){
			case 0x36:	*key_data = TILT_UP;	break;
			case 0x37:	*key_data = TILT_DOWN;	break;
			case 0x38:	*key_data = PAN_LEFT;	break;
			case 0x39:	*key_data = PAN_RIGHT;	break;
			case 0x3a:	*key_data = TILT_UP|PAN_LEFT;	break;
			case 0x3b:	*key_data = TILT_DOWN|PAN_LEFT;	break;
			case 0x3c:	*key_data = TILT_UP|PAN_RIGHT;	break;
			case 0x3d:	*key_data = TILT_DOWN|PAN_RIGHT;break;
			case 0x3e:	*key_data = ZOOM_TELE;	break;
			case 0x3f:	*key_data = ZOOM_WIDE;	break;
			case 0x40:	*key_data = FOCUS_NEAR;	break;
			case 0x41:	*key_data = FOCUS_FAR;	break;
			case 0x42:	*key_data = MENU_ON;	break;
		}
	}
}

static void dec_pkt_ERNA(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=0;i<5;i++) checksum += pkt[i];

	if((pkt[0] == 0x02) && (pkt[2] == 0x01) && (pkt[5] == checksum))
	{
		*ch_id = pkt[1];

		if(pkt[3] == 0x00)			*key_data = PTZ_STOP;
		else if(pkt[3] == 0x40)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;
		else{
			if(pkt[3] & 0x01)		*key_data |= PAN_RIGHT;
			else if(pkt[3] & 0x02)	*key_data |= PAN_LEFT;
			if(pkt[3] & 0x04)		*key_data |= TILT_UP;
			else if(pkt[3] & 0x08)	*key_data |= TILT_DOWN;
			if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
			else if(pkt[3] & 0x10)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_Bosch_OSRD(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,chksum_byte,checksum = 0;

	if(pkt[3]<0x05){
		for(i=0;i<6;i++) checksum += pkt[i];
		chksum_byte = pkt[6];
	}
	else{
		for(i=0;i<7;i++) checksum += pkt[i];
		chksum_byte = pkt[7];
	}

	checksum &= 0x7f;					// Check Sum : (sum of Byte all) & 0x7f

	if(chksum_byte == checksum)
	{
		*ch_id = ((pkt[1]&0x7f)<<7) + (pkt[2]&0x7f) + 1;

		if((pkt[4] == 0x00) && (pkt[5] == 0x00))	*key_data = PTZ_STOP;
		else
		{
			switch(pkt[3]){
				case 0x12:
					if((pkt[4] == 0x3c) && (pkt[5] == 0x01))		*key_data = MENU_ON;
					else if((pkt[4] == 0x3c) && (pkt[5] == 0x00))	*key_data = MENU_ESC;
				break;
				case 0x02:
					if(pkt[4] == 0x02)			*key_data = FOCUS_NEAR;
					else if(pkt[5] == 0x02)		*key_data = FOCUS_FAR;
					else{
						if(pkt[4] & 0x10)		*key_data |= PAN_LEFT;
						else if(pkt[5] & 0x10)	*key_data |= PAN_RIGHT;
						if(pkt[4] & 0x08)		*key_data |= TILT_UP;
						else if(pkt[5] & 0x08)	*key_data |= TILT_DOWN;
						if(pkt[5] & 0x04)		*key_data |= ZOOM_TELE;
						else if(pkt[4] & 0x04)	*key_data |= ZOOM_WIDE;
					}
				break;
				case 0x03:
					if(pkt[4] == 0x01)			*key_data = FOCUS_NEAR;
					else if(pkt[5] == 0x40)		*key_data = FOCUS_FAR;
					else{
						if(pkt[5] & 0x01)		*key_data |= PAN_RIGHT;
						else if(pkt[5] & 0x02)	*key_data |= PAN_LEFT;
						if(pkt[5] & 0x04)		*key_data |= TILT_DOWN;
						else if(pkt[5] & 0x08)	*key_data |= TILT_UP;
						if(pkt[5] & 0x20)		*key_data |= ZOOM_TELE;
						else if(pkt[5] & 0x10)	*key_data |= ZOOM_WIDE;
					}
				break;
				case 0x04:
					if((pkt[4]&0x0e) == 0x0e){
						if(pkt[4] == 0x01)			*key_data = FOCUS_NEAR;
						else if(pkt[5] == 0x40)		*key_data = FOCUS_FAR;
						else{
							if(pkt[5] & 0x01)		*key_data |= PAN_RIGHT;
							else if(pkt[5] & 0x02)	*key_data |= PAN_LEFT;
							if(pkt[5] & 0x04)		*key_data |= TILT_DOWN;
							else if(pkt[5] & 0x08)	*key_data |= TILT_UP;
							if(pkt[5] & 0x20)		*key_data |= ZOOM_TELE;
							else if(pkt[5] & 0x10)	*key_data |= ZOOM_WIDE;
						}
					}
				break;
				case 0x05:
				case 0x08:
					if(pkt[5] == 0x01)			*key_data = FOCUS_NEAR;
					else if(pkt[6] == 0x40)		*key_data = FOCUS_FAR;
					else{
						if(pkt[6] & 0x01)		*key_data |= PAN_RIGHT;
						else if(pkt[6] & 0x02)	*key_data |= PAN_LEFT;
						if(pkt[6] & 0x04)		*key_data |= TILT_DOWN;
						else if(pkt[6] & 0x08)	*key_data |= TILT_UP;
						if(pkt[6] & 0x20)		*key_data |= ZOOM_TELE;
						else if(pkt[6] & 0x10)	*key_data |= ZOOM_WIDE;
					}
				break;
			}
		}
	}
}

static void dec_pkt_Bosch_BiCom(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=1;i<11;i++) checksum += pkt[i];

	checksum ^= 0xff;
	checksum += 1;

//	if((pkt[11] == checksum) && (pkt[12] == 0xc0))
	if(pkt[12] == 0xc0)
	{
		*ch_id = ((pkt[2]&0x7f)<<7) + (pkt[3]&0x7f) + 1;

		if((pkt[9] == 0x00) && (pkt[10] == 0x00))	*key_data = PTZ_STOP;
		else{
			switch(pkt[7]){
				case 0x12:
					if(pkt[9]&0x80)	*key_data = PAN_RIGHT;
					else			*key_data = PAN_LEFT;
				break;
				case 0x13:
					if(pkt[9]&0x80)	*key_data = TILT_DOWN;
					else			*key_data = TILT_UP;
				break;
				case 0x14:
					if(pkt[9]&0x80)	*key_data = ZOOM_WIDE;
					else			*key_data = ZOOM_TELE;
				break;
				case 0x15:
					if(pkt[9]&0x80)	*key_data = FOCUS_FAR;
					else			*key_data = FOCUS_NEAR;
				break;
			}
		}
	}
}

static void dec_pkt_Cyber_Scan1(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=0;i<6;i++) checksum += pkt[i];

	if(pkt[6] == checksum)		// Byte1~Byte6을 더한 값(overflow 무시)
	{
		*ch_id = pkt[1];

		if((pkt[3] == 0x0d) && (pkt[4] == 0x88))		*key_data = PTZ_STOP;
		else if((pkt[2] == 0x19) && (pkt[3] == 0x01))	*key_data = MENU_ON;
		else if((pkt[2] == 0x40) && (pkt[3] == 0xdc))	*key_data = MENU_ENTER;
		else if((pkt[2] == 0x1f) && (pkt[3] == 0xdc))	*key_data = MENU_ESC;
		else if(pkt[2] == 0x10){
			if(pkt[3] == 0x03)				*key_data = FOCUS_NEAR;
			else if(pkt[3] == 0x04)			*key_data = FOCUS_FAR;
			else{
				if((pkt[4]&0xf0)>0x80)		*key_data |= PAN_RIGHT;
				else if((pkt[4]&0xf0)<0x80)	*key_data |= PAN_LEFT;
				if((pkt[4]&0x0f)>0x08)		*key_data |= TILT_UP;
				else if((pkt[4]&0x0f)<0x08)	*key_data |= TILT_DOWN;

				if(pkt[3] > 0x0d)			*key_data |= ZOOM_TELE;
				else if(pkt[3] < 0x0d)		*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_Yujin_System(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=0;i<12;i++) checksum += pkt[i];	// Byte1~Byte12을 더한 값(overflow 무시)
	if(pkt[12] == checksum)
	{
		*ch_id = pkt[1];

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
			*key_data = PTZ_STOP;
	 	else{
			if(pkt[2] == 0x01)			*key_data = FOCUS_NEAR;
			else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;
			else{
				if(pkt[3] & 0x02)		*key_data |= PAN_LEFT;
				else if(pkt[3] & 0x04)	*key_data |= PAN_RIGHT;
				if(pkt[3] & 0x08)		*key_data |= TILT_UP;
				else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
				if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
				else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_Dynacolor_DSCP(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE checksum = 0;

	checksum = pkt[0]^pkt[1]^pkt[2]^pkt[3]^pkt[4];	// checksum = bite 1 XOR bite2 XOR bite3 XOR bite 4 XOR bite5

	if(pkt[5] == checksum)
	{
		*ch_id = pkt[0];

		switch(pkt[2])
		{
			case 0x13:
			case 0x14:	*key_data = PTZ_STOP;	break;
			case 0x18:
				if(pkt[3] == 0x00)		*key_data = PAN_RIGHT;
				else if(pkt[3] == 0x01)	*key_data = PAN_LEFT;
				else if(pkt[3] == 0x02)	*key_data = TILT_UP;
				else if(pkt[3] == 0x03)	*key_data = TILT_DOWN;
			break;
			case 0x24:
				if(pkt[3] == 0x00)		*key_data = ZOOM_WIDE;
				else if(pkt[3] == 0x01)	*key_data = ZOOM_TELE;
				else if(pkt[3] == 0x04)	*key_data = PTZ_STOP;
			break;
			case 0x25:
				if(pkt[3] == 0x00)		*key_data = FOCUS_NEAR;
				else if(pkt[3] == 0x01)	*key_data = FOCUS_FAR;
				else if(pkt[3] == 0x04)	*key_data = PTZ_STOP;
			break;
			case 0x28:
				if(pkt[3] == 0x04)		*key_data = MENU_ENTER;
				else if(pkt[3] == 0x00)	*key_data = MENU_UP;
				else if(pkt[3] == 0x01)	*key_data = MENU_DOWN;
				else if(pkt[3] == 0x02)	*key_data = MENU_LEFT;
				else if(pkt[3] == 0x03)	*key_data = MENU_RIGHT;
			break;
		}
	}
}

static void dec_pkt_MCU_1200N(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i;
	WORD crc_data,crc16 = 0;

	for(i=0;i<6;i++){
		crc16 ^= ((WORD)pkt[2+i]);
		crc16 = crc_cal(crc16);
	}

	crc_data = pkt[10];				// checksum 2byte : CRC-16
	crc_data <<= 8;
	crc_data |= pkt[11];

	if((pkt[3] == 0x11) && (pkt[4] == 0x01) && (crc_data == crc16))
	{
		*ch_id = pkt[2];

		if((pkt[6] == 0x00) && (pkt[7] == 0x00))	*key_data = PTZ_STOP;
	 	else{
			if(pkt[5] == 0x40)			*key_data = FOCUS_NEAR;
			else if(pkt[5] == 0x80)		*key_data = FOCUS_FAR;
			else{
				if(pkt[5] & 0x01)		*key_data |= PAN_RIGHT;
				else if(pkt[5] & 0x02)	*key_data |= PAN_LEFT;
				if(pkt[5] & 0x04)		*key_data |= TILT_UP;
				else if(pkt[5] & 0x08)	*key_data |= TILT_DOWN;
				if(pkt[5] & 0x10)		*key_data |= ZOOM_TELE;
				else if(pkt[5] & 0x20)	*key_data |= ZOOM_WIDE;
			}
		}
	}
}

static void dec_pkt_AD_SpeedDome(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE checksum_byte,checksum = 0;

	if(pkt[1] == 0xc0){
		checksum = 0x00 - (pkt[0]+pkt[1]+pkt[2]+pkt[3]);
		checksum_byte = pkt[4];
	}
	else if(pkt[1] == 0xcc){
		checksum = 0x00 - (pkt[0]+pkt[1]+pkt[2]);
		checksum_byte = pkt[3];
	}
	else{
		checksum = 0x00 - (pkt[0]+pkt[1]);
		checksum_byte = pkt[2];
	}

	if(checksum_byte == checksum)
	{
		*ch_id = pkt[0];

		switch(pkt[1])
		{
			case 0x83:
			case 0x86:
			case 0x89:
			case 0x8c:	*key_data = PTZ_STOP;	break;
			case 0x87:	*key_data = FOCUS_NEAR;	break;
			case 0x88:	*key_data = FOCUS_FAR;	break;
			case 0x8a:	*key_data = ZOOM_TELE;	break;
			case 0x8b:	*key_data = ZOOM_WIDE;	break;
			case 0xc0:
				if(pkt[2] == 0x81)		*key_data = PAN_LEFT;
				else if(pkt[2] == 0x82)	*key_data = PAN_RIGHT;
				else if(pkt[2] == 0x84)	*key_data = TILT_UP;
				else if(pkt[2] == 0x85)	*key_data = TILT_DOWN;
			break;
			case 0xcc:
				if(pkt[2] == 0x01)		*key_data = MENU_ENTER;
				else if(pkt[2] == 0x02)	*key_data = MENU_OFF;
			break;
		}
	}
}

static void dec_pkt_VCLTP(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	*ch_id = pkt[0] - 0x80 + 1;

	switch(pkt[1])
	{
		case 0x4c:	*key_data = PAN_LEFT;	break;	// 'L'
		case 0x52:	*key_data = PAN_RIGHT;	break;	// 'R'
		case 0x55:	*key_data = TILT_UP;	break;	// 'U'
		case 0x4e:	*key_data = TILT_DOWN;	break;	// 'D'
		case 0x3a:	*key_data = ZOOM_TELE;	break;	// ':'
		case 0x3b:	*key_data = ZOOM_WIDE;	break;	// ';'
		case 0x3c:	*key_data = FOCUS_NEAR;	break;	// '<'
		case 0x3d:	*key_data = FOCUS_FAR;	break;	// '='

		case 0x2a:									// '\':ZOOM_TELE stop
		case 0x2b:									// '+':ZOOM_WIDE stop
		case 0x2c:									// ',':FOCUS_NEAR stop
		case 0x2d:									// '-':FOCUS_FAR stop
		case 0x6c:									// 'l':PAN_LEFT stop
		case 0x72:									// 'r':PAN_RIGHT stop
		case 0x75:									// 'u':TILT_UP stop
		case 0x6e:	*key_data = PTZ_STOP;	break;	// 'd':TILT_DOWN stop
	}
}

static void dec_pkt_LILIN_MLP2(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=1;i<6;i++) checksum += pkt[i];

	if(pkt[6] == checksum)						// Byte2~Byte6을 더한 값(overflow 무시)
	{
		*ch_id = pkt[1];

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00))		*key_data = PTZ_STOP;
		else if((pkt[2] == 0x1a) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = MENU_ON;
		else if((pkt[2] == 0x00) && (pkt[3] == 0x36) && (pkt[4] == 0x00))	*key_data = MENU_ENTER;
		else if((pkt[2] == 0x00) && (pkt[3] == 0x37) && (pkt[4] == 0x00))	*key_data = MENU_ESC;
		else if((pkt[2] == 0x01) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = FOCUS_NEAR;
		else if((pkt[2] == 0x02) && (pkt[3] == 0x00) && (pkt[4] == 0x00))	*key_data = FOCUS_FAR;
		else if((pkt[2] == 0x00) && (pkt[3] & 0xc0)){
			if(pkt[3] & 0x01)		*key_data |= PAN_RIGHT;
			else if(pkt[3] & 0x02)	*key_data |= PAN_LEFT;
			if(pkt[3] & 0x04)		*key_data |= TILT_UP;
			else if(pkt[3] & 0x08)	*key_data |= TILT_DOWN;
			if(pkt[3] & 0x10)		*key_data |= ZOOM_TELE;
			else if(pkt[3] & 0x20)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_LILIN_FastDome(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	*ch_id = pkt[0];

	if((pkt[1] == 0x00) && (pkt[2] == 0x00))		*key_data = PTZ_STOP;
	else if((pkt[1] == 0x06) && (pkt[2] == 0x00))	*key_data = MENU_ENTER;
	else if((pkt[1] == 0x03) && (pkt[2] == 0x00))	*key_data = MENU_ESC;
	else if((pkt[1] == 0x40) && (pkt[2] == 0x00))	*key_data = FOCUS_NEAR;
	else if((pkt[1] == 0x80) && (pkt[2] == 0x00))	*key_data = FOCUS_FAR;
	else{
		if(pkt[1] & 0x01)		*key_data |= PAN_RIGHT;
		else if(pkt[1] & 0x02)	*key_data |= PAN_LEFT;
		if(pkt[1] & 0x04)		*key_data |= TILT_UP;
		else if(pkt[1] & 0x08)	*key_data |= TILT_DOWN;
		if(pkt[1] & 0x10)		*key_data |= ZOOM_TELE;
		else if(pkt[1] & 0x20)	*key_data |= ZOOM_WIDE;
	}
}

static void dec_pkt_NUVICO_DOME(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i,checksum = 0;

	for(i=0;i<6;i++) checksum += pkt[i];

	if(pkt[6] == checksum^0xa5)		// Byte1~Byte6을 더한 값(overflow 무시)
	{
		*ch_id = pkt[2];

		if((pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))	*key_data = PTZ_STOP;
		else if(pkt[3] == 0x08)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0x04)		*key_data = FOCUS_FAR;
		else{
			if(pkt[5] == 0x0f)		*key_data |= TILT_UP;
			else if(pkt[5] == 0x8f)	*key_data |= TILT_DOWN;
			if(pkt[4] == 0x8f)		*key_data |= PAN_LEFT;
			else if(pkt[4] == 0x0f)	*key_data |= PAN_RIGHT;
			if(pkt[3] == 0x13)		*key_data |= ZOOM_TELE;
			else if(pkt[3] == 0x1b)	*key_data |= ZOOM_WIDE;
		}
	}
}

static void dec_pkt_SONY_VISCA(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
//	The monitor may also need to toggle the auto focus on and off with this command.
//	81/01/04/38/10/FF

	*ch_id = pkt[0]&0x0f;

	if((pkt[2] == 0x06) && (pkt[3] == 0x01)){
		if((pkt[6] == 0x03) && (pkt[7] == 0x01))		*key_data = TILT_UP;
		else if((pkt[6] == 0x03) && (pkt[7] == 0x02))	*key_data = TILT_DOWN;
		else if((pkt[6] == 0x01) && (pkt[7] == 0x03))	*key_data = PAN_LEFT;
		else if((pkt[6] == 0x02) && (pkt[7] == 0x03))	*key_data = PAN_RIGHT;
		else if((pkt[6] == 0x03) && (pkt[7] == 0x03))	*key_data = PTZ_STOP;
	}
	else if((pkt[2] == 0x04) && (pkt[3] == 0x07)){
		if((pkt[4] == 0x02) || ((pkt[4]&0x0f) == 0x20))			*key_data = ZOOM_TELE;
		else if((pkt[4] == 0x03) || ((pkt[4]&0x0f) == 0x30))	*key_data = ZOOM_WIDE;
		else if(pkt[4] == 0x00)	*key_data = PTZ_STOP;
	}
	else if((pkt[2] == 0x04) && (pkt[3] == 0x08)){
		if((pkt[4] == 0x02) || ((pkt[4]&0x0f) == 0x20))			*key_data = FOCUS_FAR;
		else if((pkt[4] == 0x03) || ((pkt[4]&0x0f) == 0x30))	*key_data = FOCUS_NEAR;
		else if(pkt[4] == 0x00)	*key_data = PTZ_STOP;
	}
}

static void dec_pkt_LG_KPC_Z180(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)	//use Key Control command
{
	BYTE i,checksum = 0;

	for(i=0;i<5;i++) checksum += pkt[i];		// Check Sum : (sum of Byte1~Byte5) & 0x7f

	if(pkt[5] == checksum)
	{
		*ch_id = pkt[4];

		switch(pkt[2]){
			case 0x25:	*key_data = MENU_ON;	break;
			case 0x24:	*key_data = MENU_ESC;	break;
			case 0x4f:	*key_data = MENU_UP;	break;
			case 0x26:	*key_data = MENU_DOWN;	break;
			case 0x28:	*key_data = MENU_LEFT;	break;
			case 0x27:	*key_data = MENU_RIGHT;	break;
			case 0x0c:	*key_data = PTZ_STOP;	break;
			//case 0x36:	*key_data = TILT_UP		break;
			//case 0x37:	*key_data = TILT_DOWN	break;
			//case 0x38:	*key_data = PAN_LEFT	break;
			//case 0x39:	*key_data = PAN_RIGHT	break;
			case 0x08:
			case 0x38:	*key_data = FOCUS_FAR;	break;
			case 0x09:
			case 0x39:	*key_data = FOCUS_NEAR;	break;
			case 0x01:	case 0x02:	case 0x34:	case 0x35:
			case 0x7c:	*key_data = ZOOM_TELE;	break;
			case 0x03:	case 0x04:	case 0x36:	case 0x37:
			case 0x80:	*key_data = ZOOM_WIDE;	break;
		}
	}
}

static void dec_pkt_CNB_ZxN_20(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)		//use Key Control command
{
	BYTE i,chkdata,checksum = 0;

	for(i=0;i<9;i++) checksum += pkt[i];
	chkdata = ascii_to_hex(pkt[9],pkt[10]);

	if(chkdata == checksum)
	{
		*ch_id = (pkt[1]&0x0f)*10 + (pkt[2]&0x0f);

		chkdata = ascii_to_hex(pkt[5],pkt[6]);

		switch(chkdata){
			case 0x10:	*key_data = MENU_ON;	break;
			case 0x0f:	*key_data = MENU_ESC;	break;
			//case 0x01:	*key_data = MENU_UP;	break;
			//case 0x03:	*key_data = MENU_DOWN;	break;
			//case 0x28:	*key_data = MENU_LEFT;	break;
			//case 0x27:	*key_data = MENU_RIGHT;	break;
			case 0x00:	*key_data = PTZ_STOP;	break;
			case 0x3c:	*key_data = TILT_UP;	break;
			case 0x3d:	*key_data = TILT_DOWN;	break;
			case 0x3e:	*key_data = PAN_LEFT;	break;
			case 0x3f:	*key_data = PAN_RIGHT;	break;
			case 0x05:	*key_data = FOCUS_FAR;	break;
			case 0x06:	*key_data = FOCUS_NEAR;	break;
			case 0x01:
			case 0x02:	*key_data = ZOOM_TELE;	break;
			case 0x03:
			case 0x04:	*key_data = ZOOM_WIDE;	break;
		}
	}
}
/*
static void dec_pkt_Heijmans(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)		//use Key Control command
{
	BYTE i,chkdata,checksum = 0;

	for(i=0;i<9;i++) checksum += pkt[i];
	chkdata = ascii_to_hex(pkt[9],pkt[10]);

	if(chkdata == checksum)
	{
		*ch_id = (pkt[1]&0x0f)*10 + (pkt[2]&0x0f);

		chkdata = ascii_to_hex(pkt[5],pkt[6]);

		switch(chkdata){
			case 0x10:	*key_data = MENU_ON;	break;
			case 0x0f:	*key_data = MENU_ESC;	break;
			//case 0x01:	*key_data = MENU_UP;	break;
			//case 0x03:	*key_data = MENU_DOWN;	break;
			//case 0x28:	*key_data = MENU_LEFT;	break;
			//case 0x27:	*key_data = MENU_RIGHT;	break;
			case 0x00:	*key_data = PTZ_STOP;	break;
			case 0x3c:	*key_data = TILT_UP;	break;
			case 0x3d:	*key_data = TILT_DOWN;	break;
			case 0x3e:	*key_data = PAN_LEFT;	break;
			case 0x3f:	*key_data = PAN_RIGHT;	break;
			case 0x05:	*key_data = FOCUS_FAR;	break;
			case 0x06:	*key_data = FOCUS_NEAR;	break;
			case 0x01:
			case 0x02:	*key_data = ZOOM_TELE;	break;
			case 0x03:
			case 0x04:	*key_data = ZOOM_WIDE;	break;
		}
	}
}

static void dec_pkt_DoDAMM(const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	BYTE i, checksum = 0;

	for(i=1;i<6;i++) checksum += pkt[i];

	if(pkt[6] == checksum)
	{
		*ch_id = pkt[1];

		if((pkt[2] == 0x00) && (pkt[3] == 0x00) && (pkt[4] == 0x00) && (pkt[5] == 0x00))
			*key_data = PTZ_STOP;
		else if(pkt[2] == 0x01)		*key_data = FOCUS_NEAR;
		else if(pkt[3] == 0x80)		*key_data = FOCUS_FAR;
		else if((pkt[2] == 0x00) && (pkt[3] != 0x31) && (pkt[3] != 0x35)){
			if(pkt[3] & 0x02)		*key_data |= PAN_RIGHT;
			else if(pkt[3] & 0x04)	*key_data |= PAN_LEFT;
			if(pkt[3] & 0x08)		*key_data |= TILT_UP;
			else if(pkt[3] & 0x10)	*key_data |= TILT_DOWN;
			if(pkt[3] & 0x20)		*key_data |= ZOOM_TELE;
			else if(pkt[3] & 0x40)	*key_data |= ZOOM_WIDE;
		}
	}
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
// PTZ Protocol Receive check Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
static int UartRxDataChk(Rx_Analyze_Conf *ra)
{
    int have_packet = 0;

	switch(ra->step)
	{
		case 0:
			switch(ra->Protocol)
			{
				case Dongyang_Unitech:		if(ra->buff[0] == 0x55)	ra->buff_cnt++;	break;
				case Dongyang_DY_255RXC:	if(ra->buff[0] == 'a')	ra->buff_cnt++;	break;
				case Fine_System:			if(ra->buff[0] == 0xaa)	ra->buff_cnt++;	break;
				case Hitron_HID_2404:
				case Honeywell_ScanDome2:	if(ra->buff[0] == 0xa5)	ra->buff_cnt++;	break;
				case InterM_VRx_2201:		ra->buff_cnt++;	break;
				case LG_MultiX:				if(ra->buff[0] == 0xe5)	ra->buff_cnt++;	break;
				case LG_LPT_A100L:			if(ra->buff[0] == 0x02)	ra->buff_cnt++;	break;
				case Panasonic_C:
				case Panasonic_N:			if(ra->buff[0] == 0x02)	ra->buff_cnt++;	break;
//				case DoDAMM:
				case Pelco_D:
				case Pelco_D_CNB1:
				case Pelco_D_CNB2:			if(ra->buff[0] == 0xff)	ra->buff_cnt++;	break;
				case Pelco_P:
				case Samsung:
				case Samsung_Techwin:		if(ra->buff[0] == 0xa0)	ra->buff_cnt++;	break;
				case Sungjin_SJ_100:		if(ra->buff[0] == '@')	ra->buff_cnt++;	break;
				case Sungjin_SJ_1000:		if(ra->buff[0] == 0xa0)	ra->buff_cnt++;	break;
				case Sysmania:				if((ra->buff[0]&0xf0)==0x10)	ra->buff_cnt++;	break;
				case Vicon_Stn:				//if(ra->buff[0]&0xf0) == 0x80)	ra->buff_cnt++;	break;
				case Vicon_Ext:				if((ra->buff[0]&0xf0) == 0x80)	ra->buff_cnt++;	break;
				case Ikegami_PCS_35:		//if(ra->buff[0] == 0x01)	ra->buff_cnt++;	break;
				case Ikegami_PCS_358:		if(ra->buff[0] == 0x01)	ra->buff_cnt++;	break;
				case New_Born_Hightech:		if(ra->buff[0] == 0xaa)	ra->buff_cnt++;	break;
				case TOKINA_DMP:			ra->buff_cnt++;	break;
				case Ernitec:				if(ra->buff[0] == 0x02)	ra->buff_cnt++;	break;
				case Bosch_OSRD:			if(ra->buff[0] == 0x86)	ra->buff_cnt++;	break;
				case Bosch_BiCom:			if(ra->buff[0] == 0xc0)	ra->buff_cnt++;	break;
				case Cyber_Scan1:			if(ra->buff[0] == 0xa5)	ra->buff_cnt++;	break;
				case Yujin_System:			if(ra->buff[0] == 0xff)	ra->buff_cnt++;	break;
				case Ladon:
				case Dynacolor_DSCP:		ra->buff_cnt++;	break;
				case MCU_1200N:				if(ra->buff[0] == 0x10)	ra->buff_cnt++;	break;
				case AD_SpeedDome:			ra->buff_cnt++;	break;
				case VCLTP:					if(ra->buff[0]&0x80)	ra->buff_cnt++;	break;
				case LILIN_MLP2:			if(ra->buff[0] == 0xe0)	ra->buff_cnt++;	break;
				case LILIN_FastDome:		ra->buff_cnt++;	break;
				case NUVICO:				if(ra->buff[0] == 0xe5)	ra->buff_cnt++;	break;
				case SONY_VISCA:			if((ra->buff[0]&0xf0) == 0x80)	ra->buff_cnt++;	break;
				case LG_KPC_Z180:			if(ra->buff[0] == 0xc5)	ra->buff_cnt++;	break;
				case CNB_ZxN_20:			if(ra->buff[0] == 0x2a)	ra->buff_cnt++;	break;
//				case Heijmans:				if(ra->buff[0] == 0x02)	ra->buff_cnt++;	break;
				default:	break;
			}

			if(ra->buff_cnt == 1){
				ra->length = protocol_conf[ra->Protocol].pkt_len;
				ra->step = 1;
			}
		break;
		case 1:
			switch(ra->Protocol)
			{
				//case Dongyang_Unitech:	break;
				case Dongyang_DY_255RXC:
					if(ra->buff[1]=='t'){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case Fine_System:
					if(ra->buff[1]=='Z'){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case Hitron_HID_2404:
				case Honeywell_ScanDome2:
					ra->buff_cnt++;
					if(ra->buff_cnt == 3){
						if((ra->buff[2]==0x10) || (ra->buff[2]==0x19))	ra->step = 2;
						else{ ra->buff_cnt = 0;	ra->step = 0; }
					}
				break;
				case InterM_VRx_2201:
					if(ra->buff[1]==0x02){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case Panasonic_C:
					ra->buff_cnt++;
					switch(ra->buff_cnt){
						case 7:	if(ra->buff[6]!='G'){ ra->buff_cnt = 0; ra->step = 0; }		break;
						case 8:	if(ra->buff[7]!='C'){ ra->buff_cnt = 0; ra->step = 0; }		break;
						case 9:
							//ra->length = ra->buff_cnt + (ra->buff[8]-0x30) + 2;
							if(ra->buff[8] == '7')	ra->length = 18;
							else if(ra->buff[8] == 'F')	ra->length = 24;
							else{ ra->buff_cnt = 0;	ra->step = 0; }
							ra->step = 2;
						break;
					}
				break;
				case Panasonic_N:
					ra->buff_cnt++;
					switch(ra->buff_cnt){
						case 7:	if(ra->buff[6]!='G'){ ra->buff_cnt = 0; ra->step = 0; }		break;
						case 8:	if(ra->buff[7]!='C'){ ra->buff_cnt = 0; ra->step = 0; }		break;
						case 9:
							//ra->length = ra->buff_cnt + (ra->buff[8]-0x30) + 2;
							if(ra->buff[8] == '7')	ra->length = 18;
							else if(ra->buff[8] == 'F')	ra->length = 24;
							else{ ra->buff_cnt = 0;	ra->step = 0; }
							ra->step = 2;
						break;
					}
				break;
				//case Pelco_D:				break;
				//case Pelco_D_CNB1:		break;
				//case Pelco_D_CNB2:		break;
				//case Pelco_P:				break;
				case Samsung:
					ra->buff_cnt++;
					if(ra->buff_cnt == 4){
						if((ra->buff[3]==0x01) || (ra->buff[3]==0x03))	ra->step = 2;
						else{ ra->buff_cnt = 0;	ra->step = 0; }
					}
				break;
				//case Samsung_Techwin:		break;
				case Sungjin_SJ_100:
					if(ra->buff[1]=='C'){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case Sysmania:
					ra->buff_cnt++;
					if(ra->buff_cnt==2){
						if((ra->buff[1]&0xf0)!=0x20){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==3){
						if((ra->buff[2]&0xf0)==0x30)	ra->step = 2;
						else{ ra->buff_cnt = 0; ra->step = 0; }
					}
				break;
				//case Vicon_Stn:			break;
				//case Vicon_Ext:			break;
				case Ikegami_PCS_35:
					ra->buff_cnt++;
					if(ra->buff_cnt==2){
						if(ra->buff[1]!=0x70){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==4){
						if(ra->buff[3]!=0x02){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==5){
						if(ra->buff[4]==0x80)	ra->step = 2;
						else{ ra->buff_cnt = 0; ra->step = 0; }
					}
				break;
				case Ikegami_PCS_358:
					if((ra->buff[1]==0x70) || (ra->buff[1]==0x49)){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0; ra->step = 0; }
				break;
				//case New_Born_Hightech:	break;
				case TOKINA_DMP:
					if(ra->buff[1]==0x02){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case LG_MultiX:
					if(ra->buff[1]==0x01){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				//case LG_LPT_A100L:		break;
				case Ernitec:
					ra->buff_cnt++;
					if(ra->buff_cnt == 3){
						if(ra->buff[2]==0x01)	ra->step = 2;
						else{ ra->buff_cnt = 0;	ra->step = 0; }
					}
				break;
				case Bosch_OSRD:
					ra->buff_cnt++;
					if(ra->buff_cnt == 4){
						switch(ra->buff[3]){
							case 0x02:	case 0x03:	case 0x04:	ra->length = 7;	ra->step = 2;	break;
							case 0x05:	case 0x08:	case 0x12:	ra->length = 8;	ra->step = 2;	break;
							default:	ra->buff_cnt = 0;	ra->step = 0;	break;
						}
					}
				break;
				case Bosch_BiCom:
					ra->buff_cnt++;
					if(ra->buff_cnt==2){
						if(ra->buff[1]!=0x8b){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==5){
						if(ra->buff[4]!=0x00){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==6){
						if(ra->buff[5]!=0x60){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==7){
						if(ra->buff[6]!=0x01){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==9){
						if(ra->buff[8]==0x02)	ra->step = 2;
						else{ ra->buff_cnt = 0; ra->step = 0; }
					}
				break;
				//case Cyber_Scan1:			break;
				//case Yujin_System:		break;
				//case Ladon:
				//case Dynacolor_DSCP:		break;
				case MCU_1200N:
					if(ra->buff[1]==0x02){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case AD_SpeedDome:
					if(ra->buff[1]==0xc0)	ra->length = 5;
					else					ra->length = 3;
					ra->buff_cnt++;
					ra->step = 2;
				break;
				case VCLTP:
					if((ra->buff[1]==0x4c) || (ra->buff[1]==0x52) || (ra->buff[1]==0x55) || (ra->buff[1]==0x4e)){	// 'L','R','U','D'
							ra->length = 3;
							ra->buff_cnt++;
							ra->step = 2;
					}
					else{	//ra->length = 2;
                        have_packet = 1;
					}
				break;
				//case LILIN_MLP2:			break;
				//case LILIN_FastDome:		break;
				case NUVICO:
					if(ra->buff[1]==0x10){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case SONY_VISCA:
					ra->buff_cnt++;
					if(ra->buff[1]==0x06){ ra->length = 9; ra->step = 2; }
					else if(ra->buff[1]==0x04){ ra->length = 6; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case LG_KPC_Z180:
					if(ra->buff[1]==0x5f){ ra->buff_cnt++; ra->step = 2; }
					else{ ra->buff_cnt = 0;	ra->step = 0; }
				break;
				case CNB_ZxN_20:
					ra->buff_cnt++;
					if(ra->buff_cnt==4){
						if(ra->buff[3]!='7'){ ra->buff_cnt = 0; ra->step = 0; }
					}
					else if(ra->buff_cnt==5){
						if(ra->buff[4]=='5')	ra->step = 2;
						else{ ra->buff_cnt = 0; ra->step = 0; }
					}
				break;
//				case Heijmans:
//				break;
				default:	ra->buff_cnt++;	ra->step = 2;	break;
			}
		break;
		case 2:
			ra->buff_cnt++;
			if(ra->buff_cnt == ra->length){
				//uart_gets(ra->rdata, ra->buff, ra->buff_cnt);

                have_packet = 1;
			}
		break;
	}
	return have_packet;

}

static void dec_packet(Rx_Analyze_Conf *ctx, const BYTE *pkt, BYTE *ch_id, BYTE *key_data)
{
	switch(ctx->Protocol)
	{
		case Dongyang_Unitech:		dec_pkt_D_MAX(pkt, ch_id, key_data);			break;
		case Dongyang_DY_255RXC:	dec_pkt_DY_255RXC(pkt, ch_id, key_data);		break;
		case Fine_System:			dec_pkt_FineSystem(pkt, ch_id, key_data);		break;
		case Hitron_HID_2404:
		case Honeywell_ScanDome2:	dec_pkt_Honeywell(pkt, ch_id, key_data);		break;
		case InterM_VRx_2201:		dec_pkt_VRx_2201(pkt, ch_id, key_data);			break;
		case Panasonic_C:			dec_pkt_Panasonic_C(pkt, ch_id, key_data);		break;
		case Panasonic_N:			dec_pkt_Panasonic_N(pkt, ch_id, key_data);		break;
		case Pelco_D:				dec_pkt_Pelco_D(pkt, ch_id, key_data);			break;
		case Pelco_D_CNB1:			dec_pkt_Pelco_D_CNB1(pkt, ch_id, key_data);		break;
		case Pelco_D_CNB2:			dec_pkt_Pelco_D_CNB2(pkt, ch_id, key_data);		break;
		case Pelco_P:				dec_pkt_Pelco_P(pkt, ch_id, key_data);			break;
		case Samsung:				dec_pkt_Samsung(pkt, ch_id, key_data);			break;
		case Samsung_Techwin:		dec_pkt_Techwin(pkt, ch_id, key_data);			break;
		case Sungjin_SJ_100:		dec_pkt_Sungjin_SJ_100(pkt, ch_id, key_data);	break;
		case Sungjin_SJ_1000:		dec_pkt_Sungjin_SJ_1000(pkt, ch_id, key_data);	break;
		case Sysmania:				dec_pkt_Sysmania(pkt, ch_id, key_data);			break;
		case Vicon_Stn:				dec_pkt_Vicon_Stn(pkt, ch_id, key_data);		break;
		case Vicon_Ext:				dec_pkt_Vicon_Ext(pkt, ch_id, key_data);		break;
		case Ikegami_PCS_35:		dec_pkt_PCS_35(pkt, ch_id, key_data);			break;
		case Ikegami_PCS_358:		dec_pkt_PCS_358(pkt, ch_id, key_data);			break;
		case New_Born_Hightech:		dec_pkt_NIKO(pkt, ch_id, key_data);				break;
		case TOKINA_DMP:			dec_pkt_TOKINA_DMP(pkt, ch_id, key_data);		break;
		case LG_MultiX:				dec_pkt_Multi_X(pkt, ch_id, key_data);			break;
		case LG_LPT_A100L:			dec_pkt_LG_LPT_A100L(pkt, ch_id, key_data);		break;
		case Ernitec:				dec_pkt_ERNA(pkt, ch_id, key_data);				break;
		case Bosch_OSRD:			dec_pkt_Bosch_OSRD(pkt, ch_id, key_data);		break;
		case Bosch_BiCom:			dec_pkt_Bosch_BiCom(pkt, ch_id, key_data);		break;
		case Cyber_Scan1:			dec_pkt_Cyber_Scan1(pkt, ch_id, key_data);		break;
		case Yujin_System:			dec_pkt_Yujin_System(pkt, ch_id, key_data);		break;
		case Ladon:
		case Dynacolor_DSCP:		dec_pkt_Dynacolor_DSCP(pkt, ch_id, key_data);	break;
		case MCU_1200N:				dec_pkt_MCU_1200N(pkt, ch_id, key_data);		break;
		case AD_SpeedDome:			dec_pkt_AD_SpeedDome(pkt, ch_id, key_data);		break;
		case VCLTP:					dec_pkt_VCLTP(pkt, ch_id, key_data);			break;
		case LILIN_MLP2:			dec_pkt_LILIN_MLP2(pkt, ch_id, key_data);		break;
		case LILIN_FastDome:		dec_pkt_LILIN_FastDome(pkt, ch_id, key_data);	break;
		case NUVICO:				dec_pkt_NUVICO_DOME(pkt, ch_id, key_data);		break;
		case SONY_VISCA:			dec_pkt_SONY_VISCA(pkt, ch_id, key_data);		break;
		case LG_KPC_Z180:			dec_pkt_LG_KPC_Z180(pkt, ch_id, key_data);		break;
		case CNB_ZxN_20:			dec_pkt_CNB_ZxN_20(pkt, ch_id, key_data);		break;
//		case Heijmans:				dec_pkt_Heijmans(pkt, ch_id, key_data);			break;
//		case DoDAMM:				dec_pkt_DoDAMM(pkt, ch_id, key_data);			break;
	}
}

static jfieldID     field_context;
static jmethodID    method_on_receive_packet;

static inline void ptz_analyzer_reset(Rx_Analyze_Conf *ra)
{
    ra->buff_cnt = 0;
    ra->step = 0;
}

static void ptz_analyzer_decode_byte(JNIEnv *env, jobject thiz, Rx_Analyze_Conf *ra, BYTE byte)
{
    ra->buff[ra->buff_cnt] = byte;

    // 수신된 데이터를 선택한 프로토콜 포맷과 매칭되는 패킷을 찾음
    if (UartRxDataChk(ra)) {
        const BYTE *pkt_data = ra->buff;
        int length = ra->length;
        BYTE ch = 0, key_data = 0;

        dec_packet(ra, pkt_data, &ch, &key_data);

        if (key_data > 0) {
            jbyteArray bytes = env->NewByteArray(ra->length);
            if (bytes) {
                env->SetByteArrayRegion(bytes, 0, ra->length, (jbyte *)ra->buff);

                // Java class의 onReceivePacket()을 호출한다
                env->CallVoidMethod(thiz, method_on_receive_packet, ch, key_data, bytes);
            }
        }

        ptz_analyzer_reset(ra);
    }
}

static void
com_sscctv_seeeyes_ptz_PtzAnalyzer_init(JNIEnv *env, jobject thiz, jint protocol)
{
	Rx_Analyze_Conf *context;

    if (protocol >= PTZ_PROTOCOL_MAX) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid protocol type");
        return;
    }

    context = new Rx_Analyze_Conf;
	if (!context) {
		jniThrowException(env, "java/lang/RuntimeException", "Could not alloc PTZ reader context");
		return;
	}

    context->Protocol = (BYTE)protocol;

    ptz_analyzer_reset(context);

	env->SetLongField(thiz, field_context, (long)context);
}

static jint
com_sscctv_seeeyes_ptz_PtzAnalyzer_decode_data(JNIEnv *env, jobject thiz, jobject buffer, jint length)
{
    Rx_Analyze_Conf *context = (Rx_Analyze_Conf *)env->GetLongField(thiz, field_context);

    jbyte* buf = (jbyte *)env->GetDirectBufferAddress(buffer);
    if (!buf) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer is not direct");
        return -1;
    }
    jlong capacity = env->GetDirectBufferCapacity(buffer);
    if (length > capacity) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid length or buffer capacity");
        return -1;
    }

    // 실제 디코딩을 실행
    while (length > 0) {
//        LOGE("INPUT Data: %x",*buf);
        ptz_analyzer_decode_byte(env, thiz, context, *buf);

        buf++;
        length--;
    }

    return 0;
}

static void
com_sscctv_seeeyes_ptz_PtzAnalyzer_exit(JNIEnv *env, jobject thiz)
{
	Rx_Analyze_Conf *context = (Rx_Analyze_Conf *)env->GetLongField(thiz, field_context);
	delete context;
	env->SetLongField(thiz, field_context, 0);
}

static JNINativeMethod method_table[] = {
	{ "native_init",        "(I)V",	(void *)com_sscctv_seeeyes_ptz_PtzAnalyzer_init },
	{ "native_decode_data", "(Ljava/nio/ByteBuffer;I)I", (void *)com_sscctv_seeeyes_ptz_PtzAnalyzer_decode_data },
	{ "native_exit",        "()V",  (void *)com_sscctv_seeeyes_ptz_PtzAnalyzer_exit },
};

int register_com_sscctv_seeeyes_ptz_PtzAnalyzer(JNIEnv *env)
{
	jclass clazz = env->FindClass("com/sscctv/seeeyes/ptz/PtzAnalyzer");
	if (clazz == NULL) {
		LOGE("Can't find com/sscctv/seeeyes/ptz/PtzAnalyzer");
		return -1;
	}

	// 나중에 참조할 필드 ID를 기억해 둔다.
	field_context = env->GetFieldID(clazz, "mContext", "J");
	if (field_context == NULL) {
		LOGE("Can't find PtzAnalyzer.mContext");
		return -1;
	}

    method_on_receive_packet = env->GetMethodID(clazz, "onReceivePacket", "(CC[B)V");
    if (method_on_receive_packet == NULL) {
        LOGE("Can't find PtzAnalyzer.onReceivePacket");
        return -1;
    }

	return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/ptz/PtzAnalyzer",
									method_table, sizeof(method_table)/sizeof(method_table[0]));
}
