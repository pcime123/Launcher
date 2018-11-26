////////////////////////////////////////////////////////////////////////////////////////////////////
// PTZ Protocol Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "define.h"

#include "ptz_protocol.h"

#include <stdlib.h>
#include <unistd.h>

static jfieldID     field_context;
static jmethodID    method_send_buffer;

struct JniContext {
    JNIEnv *env;
    jobject thiz;
    jobject buffer;
    Ptz_Config *ptz;
};

static inline void delay_us(int us)
{
    usleep(us);
}

static inline void Delay1ms(int ms)
{
    delay_us(ms * 1000);
}

static void sendBuffer(JniContext *ctx, int length, int rxTimeout)
{
    JNIEnv *env = ctx->env;
    jobject thiz = ctx->thiz;
    jobject buffer = ctx->buffer;

    // Java class의 sendBuffer()를 호출한다
    // 전체 패킷의 길이는 앞부분의 header를 포함
    env->CallVoidMethod(thiz, method_send_buffer, buffer, HEADER_LENGTH + length, rxTimeout);
}
/*
const BYTE code Dongyang_Unitech_Str[]			= "D-MAX         ";		// D_MAX
const BYTE code Dongyang_DY_255RXC_Str[]		= "DY-255RXC     ";
const BYTE code Fine_System_Str[]				= "FineSystem    ";
const BYTE code Hitron_HID_2404_Str[]			= "Fastrax II    ";
const BYTE code Honeywell_ScanDome2_Str[]		= "Honeywell     ";
const BYTE code InterM_VRx_2201_Str[]			= "VRX-2201      ";
const BYTE code LG_MultiX_Str[]					= "LG MultiX     ";
const BYTE code LG_LPT_A100L_Str[]				= "LG LPTA100    ";
const BYTE code Panasonic_C_Str[]				= "Panasonic Conv";
const BYTE code Panasonic_N_Str[]				= "Panasonic New ";
const BYTE code Pelco_D_Protocol_Str[]			= "Pelco-D       ";
const BYTE code Pelco_P_Protocol_Str[]			= "Pelco-P       ";
const BYTE code Samsung_Protocol_Str[]			= "Samsung Elec. ";
const BYTE code Samsung_Techwin_Str[]			= "SamsungTechwin";
const BYTE code Sungjin_SJ_100_Str[]			= "Sungjin SJ100 ";
const BYTE code Sungjin_SJ_1000_Str[]			= "Sungjin SJ1000";
const BYTE code Sungjin_Receiver_Str[]			= "Sungjin       ";
const BYTE code Sysmania_Protocol_Str[]			= "Sysmania      ";
const BYTE code Vicon_Stn_Protocol_Str[]		= "Vicon Standard";
const BYTE code Vicon_Ext_Protocol_Str[]		= "Vicon Extend  ";
const BYTE code Ikegami_PCS_35_Str[]			= "Ikegami PCS35 ";
const BYTE code Ikegami_PCS_358_Str[]			= "Ikegami PCS358";
const BYTE code New_Born_Hightech_Str[]			= "NEWBORN       ";		// NIKO
const BYTE code TOKINA_DMP_Str[]				= "TOKINA DMP    ";
const BYTE code Erna_Protocol_Str[]				= "ERNA          ";
const BYTE code Bosch_OSRD_Protocol_Str[]		= "Bosch OSRD    ";
const BYTE code Bosch_BiCom_Protocol_Str[]		= "Bosch BiCom   ";
const BYTE code Cyber_Scan1_Protocol_Str[]		= "CYBER SCAN1   ";
const BYTE code Yujin_System_Protocol_Str[]		= "Yujin System  ";
const BYTE code Dynacolor_DSCP_Protocol_Str[]	= "Dynacolor DSCP";
const BYTE code Ladon_Protocol_Str[]			= "Ladon         ";
const BYTE code MCU_1200N_Protocol_Str[]		= "MCU-1200N     ";
const BYTE code AD_SpeedDome_Str[]				= "AD SpeedDome  ";
const BYTE code VCLTP_Protocol_Str[]			= "VCLTP         ";
const BYTE code LILIN_MLP2_Protocol_Str[]		= "LILIN MLP2    ";
const BYTE code LILIN_FastDome_Protocol_Str[]	= "LILIN Fast    ";
const BYTE code Pelco_D_CNB1_Protocol_Str_Str[]	= "Pelco-CNB1    ";
const BYTE code Pelco_D_CNB2_Protocol_Str_Str[]	= "Pelco-CNB2    ";
const BYTE code NUVICO_Protocol_Str[]			= "NUVICO        ";
const BYTE code SONY_VISCA_Protocol_Str[]		= "SONY VISCA    ";
const BYTE code LG_KPC_Z180_Protocol_Str[]		= "LG KPC-Z18    ";
const BYTE code CNB_ZxN_20_Protocol_Str[]		= "CNB ZxN-20    ";
const BYTE code Heijmans_Protocol_Str[]			= "Heijmans      ";
const BYTE code DoDAMM_Protocol_Str[]			= "DODAAM        ";

const BYTE code Pelco_C_Protocol_Str[]			= "PELCO-C       ";
const BYTE code A1_CCVC_Protocol_Str[]			= "A1-CCVC       ";

const BYTE code *Protocol_Name_Str[PROTOCOL_MAX_NUM] = {
		Dongyang_Unitech_Str,
		Dongyang_DY_255RXC_Str,
		Fine_System_Str,
		Hitron_HID_2404_Str,
		Honeywell_ScanDome2_Str,
		InterM_VRx_2201_Str,
		LG_MultiX_Str,
		LG_LPT_A100L_Str,
		Panasonic_C_Str,
		Panasonic_N_Str,
		Pelco_D_Protocol_Str,
		Pelco_P_Protocol_Str,
		Samsung_Protocol_Str,
		Samsung_Techwin_Str,
		Sungjin_SJ_100_Str,
		Sungjin_SJ_1000_Str,
		Sysmania_Protocol_Str,
		Vicon_Stn_Protocol_Str,
		Vicon_Ext_Protocol_Str,
		Ikegami_PCS_35_Str,
		Ikegami_PCS_358_Str,
		New_Born_Hightech_Str,
		TOKINA_DMP_Str,
		Erna_Protocol_Str,
		Bosch_OSRD_Protocol_Str,
		Bosch_BiCom_Protocol_Str,
		Cyber_Scan1_Protocol_Str,
		Yujin_System_Protocol_Str,
		Dynacolor_DSCP_Protocol_Str,
		Ladon_Protocol_Str,
		MCU_1200N_Protocol_Str,
		AD_SpeedDome_Str,
		VCLTP_Protocol_Str,
		LILIN_MLP2_Protocol_Str,
		LILIN_FastDome_Protocol_Str,
		Pelco_D_CNB1_Protocol_Str_Str,
		Pelco_D_CNB2_Protocol_Str_Str,
		NUVICO_Protocol_Str,
		SONY_VISCA_Protocol_Str,
		LG_KPC_Z180_Protocol_Str,
		CNB_ZxN_20_Protocol_Str,
//	Heijmans_Protocol_Str,
//		DoDAMM_Protocol_Str,
};

const BYTE code *Protocol_COAX_Name_Str[] = {
		Pelco_C_Protocol_Str,
		A1_CCVC_Protocol_Str
};
*/
const code Ptz_Protocol_Conf protocol_conf[PROTOCOL_MAX_NUM] = {
		{_ON,	11},	// 	Dongyang_Unitech
		{_OFF,	10},	// 	Dongyang_DY_255RXC
		{_OFF,	 4},	// 	Fine_System
		{_ON,	 7},	// 	Hitron_HID_2404
		{_ON,	 7},	// 	Honeywell_ScanDome2
		{_OFF,	 6},	// 	InterM_VRx_2201
		{_ON,	 8},	// 	LG_MultiX
		{_OFF,	 6},	// 	LG_LPT_A100L
		{_ON,	18},	// 	Panasonic_C
		{_ON,	18},	// 	Panasonic_N
		{_ON,	 7},	// 	Pelco_D_Protocol
		{_ON,	 8},	// 	Pelco_P_Protocol
		{_ON,	 9},	// 	Samsung_Protocol
		{_ON,	11},	// 	Samsung_Techwin
		{_OFF,	 5},	// 	Sungjin_SJ_100
		{_OFF,	 8},	// 	Sungjin_SJ_1000
		{_OFF,	 6},	// 	Sysmania_Protocol
		{_OFF,	 6},	// 	Vicon_Stn_Protocol
		{_OFF,	10},	// 	Vicon_Ext_Protocol
		{_OFF,	12},	// 	Ikegami_PCS_35
		{_OFF,	15},	// 	Ikegami_PCS_358
		{_ON,	 8},	// 	New_Born_Hightech
		{_OFF,	 6},	// 	TOKINA_DMP
		{_OFF,	 6},	// 	Erna_Protocol
		{_ON,	 8},	// 	Bosch_OSRD_Protocol
		{_OFF,	13},	// 	Bosch_BiCom_Protocol
		{_ON,	 7},	// 	Cyber_Scan1_Protocol
		{_OFF,	13},	// 	Yujin_System_Protocol
		{_ON,	 6},	// 	Dynacolor_DSCP_Protocol
		{_ON,	 6},	// 	Ladon_Protocol
		{_OFF,	12},	// 	MCU_1200N_Protocol
		{_ON,	 3},	// 	AD_SpeedDome
		{_OFF,	 2},	// 	VCLTP_Protocol
		{_ON,	 7},	// 	LILIN_MLP2_Protocol
		{_ON,	 3},	// 	LILIN_FastDome_Protocol
		{_ON,	 7},	// 	Pelco_D_CNB1_Protocol_Str
		{_ON,	 7},	// 	Pelco_D_CNB2_Protocol_Str
		{_OFF,	 7},	// 	NUVICO_Protocol
		{_OFF,	 6},	// 	SONY_VISCA_Protocol
		{_ON,	 6},	// 	LG_KPC_Z180_Protocol
		{_ON,	11},	// 	CNB_ZxN_20_Protocol
//	{_OFF,	16},	// 	Heijmans_Protocol
//	{2,	 7},		// 	DoDAMM_Protocol
};

////////////////////////////////////////////////////////////////////////////////////////////////////
WORD crc_cal(WORD uiCrc)
{
	BYTE i;

	for(i=0; i<8; i++){
		if((uiCrc&0x0001) == 0){
			uiCrc >>= 1;
			uiCrc &= 0x7fff;
		}
		else{
			uiCrc >>= 1;
			uiCrc &= 0x7fff;
			uiCrc ^= 0xa001;
		}
	}

	return uiCrc;
}
/*
static BYTE calculate_crc(BYTE *ptr, BYTE length)
{
	BYTE code crc_table[256] =
	{
		0x00, 0x25, 0x4A, 0x6F, 0x94, 0xB1, 0xDE, 0xFB,
		0x0D, 0x28, 0x47, 0x62, 0x99, 0xBC, 0xD3, 0xF6,
		0x1A, 0x3F, 0x50, 0x75, 0x8E, 0xAB, 0xC4, 0xE1,
		0x17, 0x32, 0x5D, 0x78, 0x83, 0xA6, 0xC9, 0xEC,
		0x34, 0x11, 0x7E, 0x5B, 0xA0, 0x85, 0xEA, 0xCF,
		0x39, 0x1C, 0x73, 0x56, 0xAD, 0x88, 0xE7, 0xC2,
		0x2E, 0x0B, 0x64, 0x41, 0xBA, 0x9F, 0xF0, 0xD5,
		0x23, 0x06, 0x69, 0x4C, 0xB7, 0x92, 0xFD, 0xD8,
		0x68, 0x4D, 0x22, 0x07, 0xFC, 0xD9, 0xB6, 0x93,
		0x65, 0x40, 0x2F, 0x0A, 0xF1, 0xD4, 0xBB, 0x9E,
		0x72, 0x57, 0x38, 0x1D, 0xE6, 0xC3, 0xAC, 0x89,
		0x7F, 0x5A, 0x35, 0x10, 0xEB, 0xCE, 0xA1, 0x84,
		0x5C, 0x79, 0x16, 0x33, 0xC8, 0xED, 0x82, 0xA7,
		0x51, 0x74, 0x1B, 0x3E, 0xC5, 0xE0, 0x8F, 0xAA,
		0x46, 0x63, 0x0C, 0x29, 0xD2, 0xF7, 0x98, 0xBD,
		0x4B, 0x6E, 0x01, 0x24, 0xDF, 0xFA, 0x95, 0xB0,
		0xD0, 0xF5, 0x9A, 0xBF, 0x44, 0x61, 0x0E, 0x2B,
		0xDD, 0xF8, 0x97, 0xB2, 0x49, 0x6C, 0x03, 0x26,
		0xCA, 0xEF, 0x80, 0xA5, 0x5E, 0x7B, 0x14, 0x31,
		0xC7, 0xE2, 0x8D, 0xA8, 0x53, 0x76, 0x19, 0x3C,
		0xE4, 0xC1, 0xAE, 0x8B, 0x70, 0x55, 0x3A, 0x1F,
		0xE9, 0xCC, 0xA3, 0x86, 0x7D, 0x58, 0x37, 0x12,
		0xFE, 0xDB, 0xB4, 0x91, 0x6A, 0x4F, 0x20, 0x05,
		0xF3, 0xD6, 0xB9, 0x9C, 0x67, 0x42, 0x2D, 0x08,
		0xB8, 0x9D, 0xF2, 0xD7, 0x2C, 0x09, 0x66, 0x43,
		0xB5, 0x90, 0xFF, 0xDA, 0x21, 0x04, 0x6B, 0x4E,
		0xA2, 0x87, 0xE8, 0xCD, 0x36, 0x13, 0x7C, 0x59,
		0xAF, 0x8A, 0xE5, 0xC0, 0x3B, 0x1E, 0x71, 0x54,
		0x8C, 0xA9, 0xC6, 0xE3, 0x18, 0x3D, 0x52, 0x77,
		0x81, 0xA4, 0xCB, 0xEE, 0x15, 0x30, 0x5F, 0x7A,
		0x96, 0xB3, 0xDC, 0xF9, 0x02, 0x27, 0x48, 0x6D,
		0x9B, 0xBE, 0xD1, 0xF4, 0x0F, 0x2A, 0x45, 0x60
	};
	BYTE crc = 0;

	while (length--)
	crc = crc_table[crc ^ *ptr++];
	return crc;
}
*/
static BYTE HextoAsciiHi(BYTE x)
{
	BYTE tmp;

	tmp = ((x>>4)<10)?((x>>4)+0x30):((x>>4)+0x37);

	return tmp;
}

static BYTE HextoAsciiLo(BYTE x)
{
	BYTE tmp;

	tmp = ((x&0x0f)<10)?((x&0x0f)+0x30):((x&0x0f)+0x37);

	return tmp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PTZ Protocol Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//	Compony Protocol Data Making Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

//	동양 유니텍 == D-Max
//	D_MAX Applicable Models:
//		DSC-300S Series (High Speed PTZ Dome Camera)
//		DSC-270S Series (High Speed PTZ Dome Camera)
//		DSC-230S Series (High Speed PTZ Dome Camera)
//		DOH-240S Series (Speed PTZ Dome Camera)
//		DPC-200 (Mini PTZ Dome camera)
//		DRX-500, DRX-502A (CCTV PTZ receiver)
//	

static int enc_pkt_D_MAX(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
	BYTE i;
	WORD checksum = 0;
    int pkt_length;

	txd[0] = 0x55;					// Data Packet의 시작
	txd[1] = 0x00;
	txd[2] = ptz->Addr;				// Address

	switch(cmd){
		case PTZ_STOP:	break;
		case MENU_ON:	txd[4] = 0xb1;	break;
		case MENU_ESC:	txd[4] = 0xb1;	break;
		case MENU_ENTER:txd[4] = 0x00;	break;
		case FOCUS_NEAR:txd[3] = 0x02; txd[7] = 0x90; break;
		case FOCUS_FAR:	txd[3] = 0x01; txd[7] = 0x90; break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[4] |= 0x02; txd[5] = 0xa1;}
			else if(cmd&PAN_LEFT)	{txd[4] |= 0x04; txd[5] = 0xa1;}
			if(cmd&TILT_UP)			{txd[4] |= 0x08; txd[6] = 0xa1;}
			else if(cmd&TILT_DOWN)	{txd[4] |= 0x10; txd[6] = 0xa1;}

			if(cmd&ZOOM_TELE)		{txd[4] |= 0x20; txd[7] = 0x09;}
			else if(cmd&ZOOM_WIDE)	{txd[4] |= 0x40; txd[7] = 0x09;}
			break;
	}

	txd[9] = 0xaa;

	for(i=0;i<10;i++) checksum += txd[i];
	txd[10] = 0x2020 - checksum;			// (0x2020 - (Byte1~Byte10을 더한값))의 하위 byte		

	pkt_length = 11;

    return pkt_length;
}

static int enc_pkt_DY_255RXC(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length = -1;

	txd[0] = 'a';							// Data Packet의 시작
	txd[1] = 't';
	txd[2] = 0x30 + (ptz->Addr/ 100);
	txd[3] = 0x30 + ((ptz->Addr % 100) / 10);
	txd[4] = 0x30 + (ptz->Addr % 10);

	if(cmd != PTZ_STOP){
		txd[6] = 0x30;
		txd[7] = '0';
		txd[8] = 0x0a;
		txd[9] = 0x0d;

		pkt_length = 10;
	}

	switch(cmd){
		case PTZ_STOP:
			txd[5] = 'j';
			txd[6] = 0x0a;
			txd[7] = 0x0d;

			txd[8]  = txd[0];
			txd[9]  = txd[1];
			txd[10] = txd[2];
			txd[11] = txd[3];
			txd[12] = txd[4];
			txd[13] = 'k';
			txd[14] = txd[6];
			txd[15] = txd[7];

			pkt_length = 16;
			break;
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[5] = 'r'; txd[6] |= 0x08;	break;
		case FOCUS_FAR:	txd[5] = 'r'; txd[6] |= 0x04;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			{txd[5] = 'm'; txd[6] |= 0x01;}
			else if(cmd&TILT_DOWN)	{txd[5] = 'm'; txd[6] |= 0x02;}
			if(cmd&PAN_LEFT)		{txd[5] = 'm'; txd[6] |= 0x04;}
			else if(cmd&PAN_RIGHT)	{txd[5] = 'm'; txd[6] |= 0x08;}
			if(cmd&ZOOM_TELE)		{txd[5] = 'r'; txd[6] |= 0x01;}
			else if(cmd&ZOOM_WIDE)	{txd[5] = 'r'; txd[6] |= 0x02;}
			break;
	}

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_FineSystem(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = 0xaa;
	txd[1] = 'Z';

//	txd[2] = 0x00;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[2] |= 0x04;	break;
		case FOCUS_FAR:	txd[2] |= 0x02;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[2] |= 0x10;
			else if(cmd&TILT_DOWN)	txd[2] |= 0x80;
			if(cmd&PAN_LEFT)		txd[2] |= 0x20;
			else if(cmd&PAN_RIGHT)	txd[2] |= 0x40;
			if(cmd&ZOOM_TELE)		txd[2] |= 0x01;
			else if(cmd&ZOOM_WIDE)	txd[2] |= 0x08;
			break;
	}

	txd[3] = ptz->Addr + 0x30;

	pkt_length = 4;

    return pkt_length;
}

static int enc_pkt_Honeywell(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xA5;							// Data Packet의 시작
	txd[1] = ptz->Addr;     				// receive device Address

	txd[2] = 0x10;							// Command : PTZ Control
	txd[3] = 0x0D;							// Data 1 : Lens control when CMD is 0x10 - 0x0d is zoom stop
	txd[4] = 0x88;							// Data 2 : pan/tilt speed - 0x88 is stop

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ON:	txd[2] = 0x19; txd[3] = 0x80;	txd[4] = 0x80;	break;
		case MENU_ENTER:
		case FOCUS_NEAR:txd[3] = 0x03;	break;
		case MENU_ESC:
		case FOCUS_FAR:	txd[3] = 0x04;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			{txd[4] &= 0xf0; txd[4] |= 0x0d;}
			else if(cmd&TILT_DOWN)	{txd[4] &= 0xf0; txd[4] |= 0x03;}
			else if(cmd&PAN_LEFT)	{txd[4] &= 0x0f; txd[4] |= 0x30;}
			else if(cmd&PAN_RIGHT)	{txd[4] &= 0x0f; txd[4] |= 0xd0;}
			if(cmd&ZOOM_TELE)		txd[3] = 0x0e;
			else if(cmd&ZOOM_WIDE)	txd[3] = 0x0c;
			break;
	}

	txd[5] = 0x55;							// End

	for(i=0;i<6;i++) txd[6] += txd[i];		// Byte1~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

static int enc_pkt_InterM_VRx_2201(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = ptz->Addr;				// Address
	txd[1] = 0x02;

	txd[2] = 0x80;
	txd[3] = 0x80;
	txd[4] = 0x80;
	txd[5] = 0x80;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[4] = 0x3f;	break;
		case FOCUS_FAR:	txd[4] = 0xc1;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[3] = 0xc1;
			else if(cmd&TILT_DOWN)	txd[3] = 0x3f;
			else if(cmd&PAN_LEFT)	txd[2] = 0x3f;
			else if(cmd&PAN_RIGHT)	txd[2] = 0xc1;
			if(cmd&ZOOM_TELE)		txd[5] = 0x3f;
			else if(cmd&ZOOM_WIDE)	txd[5] = 0xc1;
			break;
	}

	pkt_length = 6;

    return pkt_length;
}

static int enc_pkt_Panasonic_C(JniContext *ctx, BYTE cmd, BYTE *txd)			// Panasonic conventional Protocol
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = 0x02;							// start 'STX'
	txd[1] = 'A';
	txd[2] = 'D';
	txd[3] = (ptz->Addr/10) | 0x30;
	txd[4] = (ptz->Addr%10) | 0x30;
	txd[5] = ';';

	txd[6] = 'G';	txd[7] = 'C';	txd[8] = '7';	txd[9] = ':';

	if(ptz->Ctrl_Mode == MODE_PT){
		txd[10] = '2';	txd[11] = '0';	txd[12] = '2';	txd[13] = '1';	txd[14] = '3';	txd[15] = '2';

		switch(cmd){
			case PTZ_STOP:	txd[16] = '4';	break;
			case PAN_LEFT:	txd[16] = '8';	break;
			case PTZ_UPLT:	txd[16] = '9';	break;
			case TILT_UP:	txd[16] = 'A';	break;
			case PTZ_UPRT:	txd[16] = 'B';	break;
			case PAN_RIGHT:	txd[16] = 'C';	break;
			case PTZ_DNRT:	txd[16] = 'D';	break;
			case TILT_DOWN:	txd[16] = 'E';	break;
			case PTZ_DNLT:	txd[16] = 'F';	break;
		}
	}
	else if(ptz->Ctrl_Mode == MODE_ZF){
		txd[10] = '2';	txd[11] = '0';	txd[12] = '2';	txd[13] = '1';	txd[14] = '2';	txd[15] = '2';

		switch(cmd){
			case PTZ_STOP:				txd[16] = '4';	break;
			case ZOOM_TELE:				txd[16] = '8';	break;
				//case ZOOM_TELE|FOCUS_FAR:	txd[16] = '9';	break;
			case FOCUS_FAR:				txd[16] = 'A';	break;
				//case ZOOM_WIDE|FOCUS_FAR:	txd[16] = 'B';	break;
			case ZOOM_WIDE:				txd[16] = 'C';	break;
				//case ZOOM_WIDE|FOCUS_NEAR:txd[16] = 'D';	break;
			case FOCUS_NEAR:			txd[16] = 'E';	break;
				//case ZOOM_TELE|FOCUS_NEAR:txd[16] = 'F';	break;
		}
	}
	else if(ptz->Ctrl_Mode == MODE_MENU){
		txd[10] = '0';	txd[11] = '0';	txd[12] = '2';	txd[13] = '1';	txd[14] = '9';	txd[15] = '4';

		switch(cmd){
			case MENU_ON:		txd[16] = '0';	break;
			case MENU_OFF:		txd[16] = '1';	break;
			case MENU_ESC:		txd[16] = '1';	break;
			case MENU_UP:		txd[16] = '2';	break;
			case MENU_RIGHT:	txd[16] = '3';	break;
			case MENU_DOWN:		txd[16] = '4';	break;
			case MENU_LEFT:		txd[16] = '5';	break;
			case MENU_ENTER:	txd[16] = 'A';	break;
			case PTZ_STOP:		txd[16] = 'F';	break;
		}
	}

	txd[17] = 0x03;							// end 'ETX'
	pkt_length = 18;

    return pkt_length;
}

static int enc_pkt_Panasonic_N(JniContext *ctx, BYTE cmd, BYTE *txd)			// Panasonic New Protocol
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = 0x02;							// start 'STX'
	txd[1] = 'A';
	txd[2] = 'D';
	txd[3] = (ptz->Addr/10) | 0x30;
	txd[4] = (ptz->Addr%10) | 0x30;
	txd[5] = ';';

	txd[6] = 'G';	txd[7] = 'C';	txd[8] = '7';	txd[9] = ':';

	if((ptz->Ctrl_Mode == MODE_PT) || (ptz->Ctrl_Mode == MODE_ZF)){
		txd[10] = '9';	txd[11] = '0';	txd[12] = '2';	txd[13] = '8';	txd[14] = '1';	txd[15] = '0';	txd[16] = '0';

		switch(cmd){
			case PTZ_STOP:	if((ptz->prev_cmd==FOCUS_NEAR) || (ptz->prev_cmd==FOCUS_FAR)){ txd[13] = '4';	txd[14] = '0';	txd[16] = '8'; }	break;
			case PAN_LEFT:	txd[14] = '8';	txd[15] = 'A';	txd[16] = '0';	break;
			case PTZ_UPLT:	txd[14] = '9';	txd[15] = 'A';	txd[16] = 'A';	break;
			case TILT_UP:	txd[14] = 'A';	txd[15] = '0';	txd[16] = 'A';	break;
			case PTZ_UPRT:	txd[14] = 'B';	txd[15] = 'A';	txd[16] = 'A';	break;
			case PAN_RIGHT:	txd[14] = 'C';	txd[15] = 'A';	txd[16] = '0';	break;
			case PTZ_DNRT:	txd[14] = 'D';	txd[15] = 'A';	txd[16] = 'A';	break;
			case TILT_DOWN:	txd[14] = 'E';	txd[15] = '0';	txd[16] = 'A';	break;
			case PTZ_DNLT:	txd[14] = 'F';	txd[15] = 'A';	txd[16] = 'A';	break;
			case ZOOM_TELE:	txd[13] = '3';	break;
			case ZOOM_WIDE:	txd[13] = '7';	break;
			case FOCUS_NEAR:txd[13] = '4';	txd[14] = '0';	txd[15] = '0';	txd[16] = '3';	break;
			case FOCUS_FAR:	txd[13] = '4';	txd[14] = '0';	txd[15] = '0';	txd[16] = '7';	break;
		}

        ptz->prev_cmd = cmd;
	}
	else if(ptz->Ctrl_Mode == MODE_MENU){
		txd[10] = '0';	txd[11] = '0';	txd[12] = '2';	txd[13] = '1';	txd[14] = '9';	txd[15] = '4';

		switch(cmd){
			case MENU_ON:		txd[15] = 'C';	txd[16] = '0';	break;
			case MENU_OFF:		txd[15] = 'C';	txd[16] = '1';	break;
			case MENU_ESC:		txd[16] = '1';	break;
			case MENU_UP:		txd[16] = '2';	break;
			case MENU_RIGHT:	txd[16] = '3';	break;
			case MENU_DOWN:		txd[16] = '4';	break;
			case MENU_LEFT:		txd[16] = '5';	break;
			case MENU_ENTER:	txd[16] = 'A';	break;
			case PTZ_STOP:		txd[16] = 'F';	break;
		}
	}

	txd[17] = 0x03;						// end 'ETX'
	pkt_length = 18;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Pelco_D(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xFF;					// Data Packet의 시작
	txd[1] = ptz->Addr;				// receive device Address

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ON:	txd[3] = 0x03; txd[5] = 0x5f;	break;
		case MENU_ESC:	txd[2] = 0x04;	break;
		case MENU_ENTER:txd[2] = 0x02;	break;
		case FOCUS_NEAR:txd[2] = 0x01;	break;
		case FOCUS_FAR:	txd[3] = 0x80;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] |= 0x02; txd[4] = 0x30;}
			else if(cmd&PAN_LEFT)	{txd[3] |= 0x04; txd[4] = 0x30;}
			if(cmd&TILT_UP)			{txd[3] |= 0x08; txd[5] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x10; txd[5] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[3] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[3] |= 0x40;}
			break;
	}

	for(i=1;i<6;i++) txd[6] += txd[i];		 // Byte2~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Pelco_D_CNB1(JniContext *ctx, BYTE cmd, BYTE *txd)				//Pelco_D CNB1 = Box,Zoom Camera
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xFF;					// Data Packet의 시작
	txd[1] = ptz->Addr;				// receive device Address

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ON:
		case MENU_ENTER:txd[3] = 0x07;	txd[5] = 0x5f;	break;
		case MENU_ESC:	txd[3] = 0x23;	txd[5] = 0x5f;	break;
		case MENU_UP:	txd[3] = 0x20;	txd[4] = 0x30;	break;
		case MENU_DOWN:	txd[3] = 0x40;	txd[4] = 0x30;	break;
		case MENU_LEFT:
		case FOCUS_NEAR:txd[2] = 0x01;	break;
		case MENU_RIGHT:
		case FOCUS_FAR:	txd[3] = 0x80;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] |= 0x02; txd[4] = 0x30;}
			else if(cmd&PAN_LEFT)	{txd[3] |= 0x04; txd[4] = 0x30;}
			if(cmd&TILT_UP)			{txd[3] |= 0x08; txd[5] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x10; txd[5] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[3] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[3] |= 0x40;}
			break;
	}

	for(i=1;i<6;i++) txd[6] += txd[i];		 // Byte2~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Pelco_D_CNB2(JniContext *ctx, BYTE cmd, BYTE *txd)				//Pelco_D CNB1 = Box,Zoom Camera
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xFF;					// Data Packet의 시작
	txd[1] = ptz->Addr;				// receive device Address

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ON:	txd[3] = 0x07; txd[5] = 0x5f;	break;
		case MENU_ENTER:
		case FOCUS_NEAR:txd[2] = 0x01;	break;
		case MENU_ESC:
		case FOCUS_FAR:	txd[3] = 0x80;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] |= 0x02; txd[4] = 0x30;}
			else if(cmd&PAN_LEFT)	{txd[3] |= 0x04; txd[4] = 0x30;}
			if(cmd&TILT_UP)			{txd[3] |= 0x08; txd[5] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x10; txd[5] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[3] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[3] |= 0x40;}
			break;
	}

	for(i=1;i<6;i++) txd[6] += txd[i];		 // Byte2~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Pelco_P(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xa0;						// start transmission
	txd[1] = ptz->Addr - 1;				// receive device Address. first receiver is $00

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ON:	txd[3] = 0x03; txd[5] = 0x5f;	break;
		case MENU_ESC:	txd[2] = 0x08;	break;
		case MENU_ENTER:txd[2] = 0x04;	break;
		case FOCUS_NEAR:txd[2] = 0x02;	break;
		case FOCUS_FAR:	txd[2] = 0x01;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] |= 0x02; txd[4] = 0x30;}
			else if(cmd&PAN_LEFT)	{txd[3] |= 0x04; txd[4] = 0x30;}
			if(cmd&TILT_UP)			{txd[3] |= 0x08; txd[5] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x10; txd[5] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[3] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[3] |= 0x40;}
			break;
	}

	txd[6] = 0xaf;							// end transmission
	txd[7] = txd[0];
	for(i=1;i<7;i++) txd[7] ^= txd[i];		// Check Sum : XOR sum of Byte1~Byte7

	pkt_length = 8;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Samsung(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xa0;							// Data Packet의 시작
	txd[1] = 0x01;							// 송신 ptz->Address
	txd[2] = ptz->Addr;			    		// 수신 ptz->Address
	txd[3] = 0x01;							// Pan/Tilt Command:0x01

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ENTER:txd[3] = 0x03;	txd[4] = 0x18;	txd[5] = 0xff;	txd[6] = 0xff;	txd[7] = 0xff;	break;
		case MENU_ESC:	txd[3] = 0x03;	txd[4] = 0x17;	txd[5] = 0x00;	txd[6] = 0xff;	txd[7] = 0xff;	break;
		case MENU_ON:	txd[3] = 0x03;	txd[4] = 0x17;	txd[5] = 0x01;	txd[6] = 0xff;	txd[7] = 0xff;	break;

		case FOCUS_FAR:	txd[4] = 0x01; break;
		case FOCUS_NEAR:txd[4] = 0x02; break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		{txd[5] |= 0x01; txd[6] = 0x30;}
			else if(cmd&PAN_RIGHT)	{txd[5] |= 0x02; txd[6] = 0x30;}
			if(cmd&TILT_UP)			{txd[5] |= 0x04; txd[7] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[5] |= 0x08; txd[7] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[4] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[4] |= 0x40;}
			break;
	}

	for(i=1;i<8;i++) checksum += txd[i];
	txd[8] = ~checksum;						// (0xffff - (Byte2~Byte8을 더한값))의 하위 byte

	pkt_length = 9;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Techwin(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xa0;						// STX(Start of Text = A0h)
	txd[1] = ptz->Addr;					// Receiver ptz->Address
	txd[2] = 0x00;						// Sender ptz->Address

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ENTER:txd[3] = 0x01;	txd[5] = 0x06;	break;
		case MENU_ESC:	txd[3] = 0x02;	txd[5] = 0x07;	break;
		case MENU_OFF:	txd[4] = 0xb1;	txd[5] = 0x01;	break;
		case MENU_ON:	txd[4] = 0xb1;	txd[5] = 0x00;	break;

		case FOCUS_FAR:	txd[3] = 0x01; txd[7] = 0x05; break;
		case FOCUS_NEAR:txd[3] = 0x02; txd[7] = 0x05; break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[4] |= 0x02; txd[5] = 0x30;}
			else if(cmd&PAN_LEFT)	{txd[4] |= 0x04; txd[5] = 0x30;}
			if(cmd&TILT_UP)			{txd[4] |= 0x08; txd[6] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[4] |= 0x10; txd[6] = 0x30;}
			if(cmd&ZOOM_TELE)		{txd[4] |= 0x20; txd[7] = 0x05;}
			else if(cmd&ZOOM_WIDE)	{txd[4] |= 0x40; txd[7] = 0x05;}
			break;
	}

	txd[8] = 0xff;
	txd[9] = 0xaf;

	for(i=1;i<9;i++) checksum += txd[i];
	txd[10] = ~checksum;					// (0xffff - (Byte2~Byte8을 더한값))의 하위 byte

	pkt_length = 11;

	if(cmd == PTZ_STOP){
        // TODO pkt_length?
        sendBuffer(ctx, pkt_length, 0);

        Delay1ms(40);
	}

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Sungjin_SJ_100(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE Checksum = 0;

	txd[0] = '@';							// Data Packet의 시작
	txd[1] = 'C';							// Move Camera
	txd[2] = ptz->Addr;					    // 수신 ptz->Address

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[3] |= 0x40;	break;
		case FOCUS_FAR:	txd[3] |= 0x30;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[3] |= 0x01;
			else if(cmd&TILT_DOWN)	txd[3] |= 0x02;
			if(cmd&PAN_LEFT)		txd[3] |= 0x04;
			else if(cmd&PAN_RIGHT)	txd[3] |= 0x08;
			if(cmd&ZOOM_TELE)		txd[3] |= 0x20;
			else if(cmd&ZOOM_WIDE)	txd[3] |= 0x10;
			break;
	}

	Checksum = txd[0] + txd[1] + txd[2] + txd[3];

	txd[4] = Checksum;

	pkt_length = 5;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Sungjin_SJ_1000(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xa0;							// start transmission
	txd[1] = 0x04;							// 0x04:move camera, receive device ptz->Address high 2bit.
	txd[2] = ptz->Addr;					    // receive device ptz->Address low 8bit.

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ESC:
		case MENU_ENTER:	break;
		case FOCUS_NEAR:txd[5] = 0xc0;	break;
		case FOCUS_FAR:	txd[5] = 0x40;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] = 0xe0;}
			else if(cmd&PAN_LEFT)	{txd[3] = 0x60;}
			if(cmd&TILT_UP)			{txd[4] = 0xe0;}
			else if(cmd&TILT_DOWN)	{txd[4] = 0x60;}
			if(cmd&ZOOM_TELE)		{txd[5] = 0x0c;}
			else if(cmd&ZOOM_WIDE)	{txd[5] = 0x04;}
			break;
	}

	txd[6] = 0x00;							// end transmission
	txd[7] = txd[1];
	for(i=2;i<7;i++) txd[7] ^= txd[i];		// Check Sum : XOR sum of Byte2~Byte7

	pkt_length = 8;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Sysmania(JniContext *ctx, BYTE cmd, BYTE *txd)				// stop은 컨트롤 데이터를 더이상 송신하지 않으면 됨
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i, checksum = 0;

	txd[0] = 0x10 + ptz->Addr/100;		// 100 자리수 또는 GROUP
	txd[1] = 0x20 + ptz->Addr%10;		// 1 자리 수 RX ptz->Address
	txd[2] = 0x30 + (ptz->Addr%100)/10;	// 10 자리 수 RX ptz->Address

	txd[3] = 0x00;
	txd[4] = 0x40;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[3] = 0xAC;	break;
		case FOCUS_FAR:	txd[3] = 0xAF;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[4] |= 0x01;
			else if(cmd&TILT_DOWN)	txd[4] |= 0x02;
			if(cmd&PAN_LEFT)		txd[4] |= 0x04;
			else if(cmd&PAN_RIGHT)	txd[4] |= 0x08;
			if(cmd&ZOOM_WIDE)		txd[3] = 0xA0;
			else if(cmd&ZOOM_TELE)	txd[3] = 0xA3;
			break;
	}

	for(i=0;i<5;i++) checksum += txd[i];

	txd[5] = checksum;							// Byte1~Byte5을 더한값의 하위 byte

	pkt_length = 6;

    return pkt_length;
}

//--------------------------------------------------------------------------------------------------
static int enc_pkt_Vicon_Stn(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

    txd[0] = 0x80 | ((ptz->Addr>>4) & 0x0f);    // Start bit7      | ctx->Address_high
    txd[1] = 0x10 | (ptz->Addr & 0x0f);         // Normal Cmd bit4 | ptz->Address_low

    pkt_length = 2;

    *(txd-1) = pkt_length;
    sendBuffer(ctx, pkt_length, 10);

	txd[0] = 0x00;
	txd[1] = 0x00;
	txd[2] = 0x00;
	txd[3] = 0x00;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[1] |= 0x08;	break;
		case FOCUS_FAR:	txd[1] |= 0x10;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		txd[0] |= 0x40;
			else if(cmd&PAN_RIGHT)	txd[0] |= 0x20;
			if(cmd&TILT_UP)			txd[0] |= 0x10;
			else if(cmd&TILT_DOWN)	txd[0] |= 0x08;
			if(cmd&ZOOM_WIDE)		txd[1] |= 0x40;
			else if(cmd&ZOOM_TELE)	txd[1] |= 0x20;
			break;
	}
	pkt_length = 4;

    return pkt_length;
}

static int enc_pkt_Vicon_Ext(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

    txd[0] = 0x80 | ((ptz->Addr>>4) & 0x0f);	// Start bit7   | ptz->Address_high
    txd[1] = 0x50 | (ptz->Addr & 0x0f);		// Ext Cmd bit6 | ptz->Address_low

    pkt_length = 2;

    *(txd-1) = pkt_length;
    sendBuffer(ctx, pkt_length, 15);

	txd[0] = 0x00;	txd[1] = 0x00;	txd[2] = 0x00;	txd[3] = 0x00;
	txd[4] = 0x00;	txd[5] = 0x00;	txd[6] = 0x00;	txd[7] = 0x00;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[1] |= 0x08;	break;
		case FOCUS_FAR:	txd[1] |= 0x10;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		{txd[0] |= 0x40; txd[4] = 0x07; txd[5] = 0x00;}
			else if(cmd&PAN_RIGHT)	{txd[0] |= 0x20; txd[4] = 0x07; txd[5] = 0x00;}
			if(cmd&TILT_UP)			{txd[0] |= 0x10; txd[6] = 0x07; txd[7] = 0x00;}
			else if(cmd&TILT_DOWN)	{txd[0] |= 0x08; txd[6] = 0x07; txd[7] = 0x00;}
			if(cmd&ZOOM_WIDE)		{txd[1] |= 0x40;}
			else if(cmd&ZOOM_TELE)	{txd[1] |= 0x20;}
			break;
	}

	pkt_length = 8;

    return pkt_length;
}

static int enc_pkt_Ikegami_PCS_35(JniContext *ctx, BYTE cmd, BYTE *txd)			// PCS-35 Control
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i, checksum = 0;

	txd[0] = 0x01;							// Start of Heading
	txd[1] = 0x70;							// 기종 ID : 70h
	txd[2] = ptz->Addr + 0x30;			// CH(Camera ID):31h ~ 93h (CH 1 ~ 99)

	txd[3] = 0x02;							// Start of Text
	txd[4] = 0x80;							// 1byte째는 80h 고정
	txd[5] = 0x80;
	txd[6] = 0x80;
	txd[7] = 0x80;
	txd[8] = 0x80;
	txd[9] = 0x80;

	if(cmd != PTZ_STOP)	txd[5] |= 0x20;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[6] |= 0x08;	break;
		case FOCUS_FAR:	txd[6] |= 0x04;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		txd[7] |= 0x08;
			else if(cmd&PAN_RIGHT)	txd[7] |= 0x04;
			if(cmd&TILT_UP)			txd[7] |= 0x01;
			else if(cmd&TILT_DOWN)	txd[7] |= 0x02;
			if(cmd&ZOOM_WIDE)		txd[6] |= 0x20;
			else if(cmd&ZOOM_TELE)	txd[6] |= 0x10;
			break;
	}

	txd[10] = 0x03;							// End of Text

	checksum = txd[1];
	for(i=2;i<11;i++) checksum ^= txd[i];
	txd[11] = checksum;						// Checksum	: ID ~ ETX 까지 XOR

	pkt_length = 12;

    return pkt_length;
}

static int enc_pkt_Ikegami_PCS_358(JniContext *ctx, BYTE cmd, BYTE *txd)		// PCS-358 Control
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i, checksum = 0;

	txd[0] = 0x01;							// Start of Heading
	txd[1] = 0x70;							// 기종 ID : ID 70h 일때 31h~93h (CH 1~99), ID 49h 일때 31h~FFh (CH 1~207)
	txd[2] = ptz->Addr + 0x30;			// SUB ID (CAMERA CH)
	txd[3] = 0x2d;							// 송신원기종 ID
	txd[4] = 0x80;							// Ack Option : 80h Ack 없음 / 81h Ack 있음
	txd[5] = 0x31;							// 송신원SUB ID : 30h(ID 1) ~ 44h(ID 20)

	txd[6] = 0x02;							// Start of Text
	txd[7] = 0x80;							// Housing 제어 - 0x80 : off
	txd[8] = 0x80;
	txd[9] = 0x80;
	txd[10] = 0x80;
	txd[11] = 0x80;
	txd[12] = 0x80;

	if((cmd != PTZ_STOP) && (cmd&0x0f))	txd[8] |= 0x20;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[9] |= 0x08;	break;
		case FOCUS_FAR:	txd[9] |= 0x04;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		txd[10] |= 0x08;
			else if(cmd&PAN_RIGHT)	txd[10] |= 0x04;
			if(cmd&TILT_UP)			txd[10] |= 0x01;
			else if(cmd&TILT_DOWN)	txd[10] |= 0x02;
			if(cmd&ZOOM_WIDE)		txd[9] |= 0x20;
			else if(cmd&ZOOM_TELE)	txd[9] |= 0x10;
			break;
	}

	txd[13] = 0x03;							// End of Text

	checksum = txd[1];
	for(i=2;i<14;i++) checksum ^= txd[i];
	txd[14] = checksum;						// Checksum	: ID ~ ETX 까지 XOR

	pkt_length = 15;

    return pkt_length;
}

static int enc_pkt_NIKO(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xaa;

	switch(cmd){
		case MENU_ON:		txd[1] = 0x5f;	txd[2] = 0x25;	break;
		case MENU_ENTER:	txd[1] = 0x5f;	txd[2] = 0x27;	break;
		case MENU_ESC:		txd[1] = 0x5f;	txd[2] = 0x28;	break;
		case MENU_UP:		txd[1] = 0x5f;	txd[2] = 0x4f;	break;
		case MENU_DOWN:		txd[1] = 0x5f;	txd[2] = 0x50;	break;
//		case MENU_LEFT:		txd[3] = 0x04;	txd[4] = 0x30;	break;
//		case MENU_RIGHT:	txd[3] = 0x02;	txd[4] = 0x30;	break;
		case PTZ_STOP:
			if(ptz->Ctrl_Mode == MODE_MENU)	break;
			txd[1] = 0xe5;	txd[2] = 0x00;		break;
		case FOCUS_NEAR:	txd[1] = 0x5f;	txd[2] = 0x39;	break;
		case FOCUS_FAR:		txd[1] = 0x5f;	txd[2] = 0x38;	break;
		case PAN_LEFT:		txd[1] = 0x97;	txd[2] = 0x80;	break;
		case PAN_RIGHT:		txd[1] = 0x99;	txd[2] = 0x80;	break;
		case TILT_UP:		txd[1] = 0x9b;	txd[2] = 0x80;	break;
		case TILT_DOWN:		txd[1] = 0x9d;	txd[2] = 0x80;	break;
		case ZOOM_TELE:		txd[1] = 0x5f;	txd[2] = 0x01;	break;
		case ZOOM_WIDE:		txd[1] = 0x5f;	txd[2] = 0x03;	break;
	}

	txd[6] = ptz->Addr;

	for(i=0;i<7;i++) checksum += txd[i];

	txd[7] = checksum;						// Byte1~Byte7을 더한 값(overflow 무시)

	pkt_length = 8;

    return pkt_length;
}

static int enc_pkt_TOKINA_DMP(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = ptz->Addr;
	txd[1] = 0x02;

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[4] |= 0x80;	break;
		case FOCUS_FAR:	txd[4] |= 0x40;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[2] |= 0x80;
			else if(cmd&TILT_DOWN)	txd[2] |= 0x40;
			if(cmd&PAN_LEFT)		txd[3] |= 0x40;
			else if(cmd&PAN_RIGHT)	txd[3] |= 0x80;
			if(cmd&ZOOM_TELE)		txd[5] |= 0x40;
			else if(cmd&ZOOM_WIDE)	txd[5] |= 0x40;
			break;
	}

	pkt_length = 6;

    return pkt_length;
}

static int enc_pkt_LG_MultiX(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xE5;							// Start 0xE5
	txd[1] = 0x01;							// device type. (0x01: Dome Camera, 0x02: DVR, 0x03~0xFF: Reserved)
	txd[2] = ptz->Addr;					    // ID

	switch(cmd){
		case PTZ_STOP:		break;
		case MENU_ENTER:txd[3] = 0xdc;	txd[5] = 0x02;	break;
		case MENU_ESC:	txd[3] = 0xdc;	txd[5] = 0x00;	break;
		case MENU_ON:	txd[3] = 0xdc;	txd[5] = 0x01;	break;
		case FOCUS_NEAR:txd[4] = 0x02;	break;
		case FOCUS_FAR:	txd[4] = 0x01;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_LEFT)		{txd[4] |= 0x80; txd[5] = 0x60;}
			else if(cmd&PAN_RIGHT)	{txd[4] |= 0x40; txd[5] = 0x60;}
			if(cmd&TILT_UP)			{txd[4] |= 0x20; txd[6] = 0x60;}
			else if(cmd&TILT_DOWN)	{txd[4] |= 0x10; txd[6] = 0x60;}

			if(cmd&ZOOM_TELE)		{txd[4] |= 0x08; txd[5] |= 0x80;}
			else if(cmd&ZOOM_WIDE)	{txd[4] |= 0x04; txd[5] |= 0x80;}
			break;
	}

	for(i=0;i<7;i++) txd[7] += txd[i];		// Check Sum : sum of Byte1~Byte7

	pkt_length = 8;

    return pkt_length;
}

static int enc_pkt_LG_LPT_A100L(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0x02;							// STX 0x02
	txd[1] = ptz->Addr;					    // ID(0x01 ~ 0x80)
	txd[2] = 0x00;							// CMD
	txd[3] = 0x30;							// Data
	txd[4] = 0x03;							// ETX

	switch(cmd){
		case TILT_UP:	txd[2] = 0x36;	break;
		case TILT_DOWN:	txd[2] = 0x37;	break;
		case PAN_LEFT:	txd[2] = 0x38;	break;
		case PAN_RIGHT:	txd[2] = 0x39;	break;
		case PTZ_UPLT:	txd[2] = 0x3a;	break;
		case PTZ_DNLT:	txd[2] = 0x3b;	break;
		case PTZ_UPRT:	txd[2] = 0x3c;	break;
		case PTZ_DNRT:	txd[2] = 0x3d;	break;
		case ZOOM_TELE:	txd[2] = 0x3e;	break;
		case ZOOM_WIDE:	txd[2] = 0x3f;	break;
		case FOCUS_NEAR:txd[2] = 0x40;	break;
		case FOCUS_FAR:	txd[2] = 0x41;	break;

		case MENU_ON:	txd[2] = 0x42;	break;
		case MENU_UP:	txd[2] = 0x36;	break;
		case MENU_DOWN:	txd[2] = 0x37;	break;
		case MENU_LEFT:	txd[2] = 0x38;	break;
		case MENU_RIGHT:txd[2] = 0x39;	break;
	}

	for(i=0;i<5;i++) checksum += txd[i];

	txd[5] = checksum & 0x7f;				// Check Sum : (sum of Byte1~Byte5) & 0x7f

	pkt_length = 6;

    return pkt_length;
}

static int enc_pkt_ERNA(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0x02;				//Header
	txd[1] = ptz->Addr;		    //ptz->Address
	txd[2] = 0x01;				//Command

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[3] |= 0x40;	break;
		case FOCUS_FAR:	txd[3] |= 0x80;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		txd[3] |= 0x01;
			else if(cmd&PAN_LEFT)	txd[3] |= 0x02;
			if(cmd&TILT_UP)			txd[3] |= 0x04;
			else if(cmd&TILT_DOWN)	txd[3] |= 0x08;
			if(cmd&ZOOM_WIDE)		txd[3] |= 0x10;
			else if(cmd&ZOOM_TELE)	txd[3] |= 0x20;
			break;
	}

	for(i=0;i<5;i++) txd[5] += txd[i];		// Check Sum : sum of Byte1~5

	pkt_length = 6;

    return pkt_length;
}

static int enc_pkt_Bosch_OSRD(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0x86;								// Packet Length
	txd[1] = (ptz->Addr - 1) >> 7;			    // ptz->Address MSB
	txd[2] = (ptz->Addr - 1) & 0x7f;			//		   LSB
	txd[3] = 0x08;								// Opcode

	switch(cmd){
		case PTZ_STOP:	break;
		case MENU_ON:	txd[3] = 0x12;	txd[4] = 0x3c;	txd[5] = 0x01;	break;
		case MENU_ESC:	txd[3] = 0x12;	txd[4] = 0x3c;	txd[5] = 0x00;	break;
		case MENU_ENTER:break;
//		case MENU_UP:	txd[4] = 0x0b;	txd[6] = 0x08;	break;
//		case MENU_DOWN:	txd[4] = 0x0b;	txd[6] = 0x04;	break;
//		case MENU_LEFT:	txd[4] = 0x0b;	txd[6] = 0x02;	break;
//		case MENU_RIGHT:txd[3] = 0x0b;	txd[6] = 0x01;	break;
		case FOCUS_NEAR:txd[6] = 0x40;	break;
		case FOCUS_FAR:	txd[5] = 0x01;	break;
		case FOCUS_AUTO:break;
		default:
			if(cmd&TILT_UP)			{txd[4] |= 0x0b; txd[6] |= 0x08;}
			else if(cmd&TILT_DOWN)	{txd[4] |= 0x0b; txd[6] |= 0x04;}
			if(cmd&PAN_LEFT)		{txd[5] |= 0xb0; txd[6] |= 0x02;}
			else if(cmd&PAN_RIGHT)	{txd[5] |= 0xb0; txd[6] |= 0x01;}
			if(cmd&ZOOM_TELE)		{txd[4] |= 0x05; txd[6] |= 0x20;}
			else if(cmd&ZOOM_WIDE)	{txd[4] |= 0x05; txd[6] |= 0x10;}
			break;
	}

	for(i=0;i<7;i++) checksum += txd[i];

	txd[7] = checksum & 0x7f;					// Check Sum : (sum of Byte all) & 0x7f

	pkt_length = 8;

    return pkt_length;
}

static int enc_pkt_Bosch_BiCom(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xc0;								// End
	txd[1] = 0x8b;								// Packet Length
	txd[2] = (ptz->Addr - 1) >> 7;			    // Address MSB
	txd[3] = (ptz->Addr - 1) & 0x7f;			//		   LSB
//	txd[4] = 0x00;								// Server ID MSB
	txd[5] = 0x60;								//			 LSB : PTZ Control

	txd[6] = 0x01;
//	txd[7] = 0x00;
	txd[8] = 0x02;								// Operation : Set
//	txd[9] = 0x00;
//	txd[10] = 0x00;
//	txd[11] = 0x00;

	switch(cmd){
		case PAN_LEFT:	txd[7] = 0x12;	txd[9] = 0x12;	txd[10] = 0x34;	break;
		case PAN_RIGHT:	txd[7] = 0x12;	txd[9] = 0x92;	txd[10] = 0x34;	break;
		case TILT_UP:	txd[7] = 0x13;	txd[9] = 0x12;	txd[10] = 0x34;	break;
		case TILT_DOWN:	txd[7] = 0x13;	txd[9] = 0x92;	txd[10] = 0x34;	break;

		case ZOOM_TELE:	txd[7] = 0x14;	txd[9] = 0x00;	txd[10] = 0x10;	break;
		case ZOOM_WIDE:	txd[7] = 0x14;	txd[9] = 0x80;	txd[10] = 0x10;	break;
		case FOCUS_NEAR:txd[7] = 0x15;	txd[9] = 0x12;	txd[10] = 0x34;	break;
		case FOCUS_FAR:	txd[7] = 0x15;	txd[9] = 0x92;	txd[10] = 0x34;	break;
		default:	break;
	}

	for(i=1;i<11;i++) checksum += txd[i];

	txd[11] = (checksum ^ 0xff) + 1;			// Check Sum : sum of Byte1~5
	txd[12] = 0xc0;

	pkt_length = 13;

    return pkt_length;
}

static int enc_pkt_Cyber_Scan1(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xA5;							// Data Packet Start
	txd[1] = ptz->Addr;					    // receive device ptz->Address LSB

	txd[2] = 0x10;							// Command : PTZ Control
	txd[3] = 0x0D;							// Data 1 : Lens control when CMD is 0x10 - 0x0d is zoom stop
	txd[4] = 0x88;							// Data 2 : pan/tilt speed - 0x88 is stop

	switch(cmd){
		case MENU_ON:	txd[2] = 0x19;	txd[3] = 0x01;	break;
		case MENU_ESC:	txd[2] = 0x1f;	txd[3] = 0xdc;	break;
		case MENU_ENTER:txd[2] = 0x40;	txd[3] = 0xdc;	break;
//		case MENU_UP:	txd[4] = 0xfd;	break;
//		case MENU_DOWN:	txd[4] = 0xf3;	break;
//		case MENU_LEFT:	txd[4] = 0x3f;	break;
//		case MENU_RIGHT:txd[4] = 0xdf;	break;
		case FOCUS_NEAR:txd[3] = 0x03;	break;
		case FOCUS_FAR:	txd[3] = 0x04;	break;
		case FOCUS_AUTO:	break;
		case PTZ_STOP:		break;
		default:
			if(cmd&TILT_UP)			{txd[4] &= 0xf0; txd[4] |= 0x0d;}
			else if(cmd&TILT_DOWN)	{txd[4] &= 0xf0; txd[4] |= 0x03;}
			else if(cmd&PAN_LEFT)	{txd[4] &= 0x0f; txd[4] |= 0x30;}
			else if(cmd&PAN_RIGHT)	{txd[4] &= 0x0f; txd[4] |= 0xd0;}
			if(cmd&ZOOM_TELE)		txd[3] = 0x0e;
			else if(cmd&ZOOM_WIDE)	txd[3] = 0x0c;
			break;
	}

	txd[5] = 0x00;							// receive device ptz->Address MSB

	for(i=0;i<6;i++) checksum += txd[i];

	txd[6] = checksum;						// Byte1~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

static int enc_pkt_Yujin_System(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xFF;							// Data Packet의 시작
	txd[1] = ptz->Addr;					    // receive device ptz->Address

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:
		case FOCUS_AUTO:	break;
		case FOCUS_NEAR:txd[2] |= 0x01;	break;
		case FOCUS_FAR:	txd[3] |= 0x80;	break;
		default:
			if(cmd&PAN_LEFT)		{txd[3] |= 0x02;	txd[4] = 0x30;}
			else if(cmd&PAN_RIGHT)	{txd[3] |= 0x04;	txd[4] = 0x30;}
			if(cmd&TILT_UP)			{txd[3] |= 0x08;	txd[5] = 0x30;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x10;	txd[5] = 0x30;}
			else if(cmd&ZOOM_TELE)	txd[3] |= 0x20;
			else if(cmd&ZOOM_WIDE)	txd[3] |= 0x40;
			break;
	}

	for(i=0;i<12;i++) checksum += txd[i];

	txd[12] = checksum;						// Byte1~Byte12을 더한 값(overflow 무시)

	pkt_length = 13;

    return pkt_length;
}

static int enc_pkt_Dynacolor_DSCP(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = ptz->Addr;					    // receive device ptz->Address
	txd[1] = 0x00;							// tx id

	txd[4] = 0x00;
	switch(cmd){
		case PTZ_STOP:
			if((ptz->prev_cmd == FOCUS_NEAR) || (ptz->prev_cmd == FOCUS_FAR))	{txd[2] = 0x25;  txd[3] = 0x04;}
			else if((ptz->prev_cmd&PAN_RIGHT) || (ptz->prev_cmd&PAN_LEFT)){txd[2] = 0x13;  txd[3] = 0x00;}
			else if((ptz->prev_cmd&TILT_UP) || (ptz->prev_cmd&TILT_DOWN))	{txd[2] = 0x14;  txd[3] = 0x00;}
			else if((ptz->prev_cmd&ZOOM_TELE) || (ptz->prev_cmd&ZOOM_WIDE)){txd[2] = 0x24; txd[3] = 0x04;}
			break;
			//case MENU_ESC:	break;
		case MENU_ENTER:
		case MENU_ON:	txd[2] = 0x28; txd[3] = 0x04;	break;
		case MENU_UP:	txd[2] = 0x28; txd[3] = 0x00;	break;
		case MENU_DOWN:	txd[2] = 0x28; txd[3] = 0x01;	break;
		case MENU_LEFT:	txd[2] = 0x28; txd[3] = 0x02;	break;
		case MENU_RIGHT:txd[2] = 0x28; txd[3] = 0x03;	break;

		case PAN_RIGHT:	txd[2] = 0x18; txd[3] = 0x00; txd[4] = 0x0a;	break;
		case PAN_LEFT:	txd[2] = 0x18; txd[3] = 0x01; txd[4] = 0x0a;	break;
		case TILT_UP:	txd[2] = 0x18; txd[3] = 0x02; txd[4] = 0x0a;	break;
		case TILT_DOWN:	txd[2] = 0x18; txd[3] = 0x03; txd[4] = 0x0a;	break;

		case ZOOM_TELE:	txd[2] = 0x24; txd[3] = 0x01;	break;
		case ZOOM_WIDE:	txd[2] = 0x24; txd[3] = 0x00;	break;

		case FOCUS_NEAR:txd[2] = 0x25; txd[3] = 0x00;	break;
		case FOCUS_FAR:	txd[2] = 0x25; txd[3] = 0x01;	break;
		default:	break;
	}

	txd[5] = txd[0]^txd[1]^txd[2]^txd[3]^txd[4];	// checksum = bite 1 XOR bite2 XOR bite3 XOR bite 4 XOR bite5
	pkt_length = 6;

	ptz->prev_cmd = cmd;

    return pkt_length;
}

static int enc_pkt_MCU_1200N(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;
	WORD crc16 = 0;

	txd[0] = 0x10;							// Frame Header : Message start		DLE(0x10)
	txd[1] = 0x02;							// Frame Header : Message start		STX(0x02)
	txd[2] = ptz->Addr;					// receive device ptz->Address

	txd[3] = 0x11;							// OPCODE : 0x11(camera control command)
	txd[4] = 0x01;							// paramenter 1 : control [0]:on/off, [1]:time, [2]:preset, [3]:none, [4]:speed, [5]:미세조정
//	txd[5] = 0x00;							// paramenter 2 : ptzf
//	txd[6] = 0x00;							// paramenter 3 : pan/zoom speed	--> 0xbd(none control speed)
//	txd[7] = 0x00;							// paramenter 4 : tilt/focus speed	--> 0xbd(none control speed)

	txd[8] = 0x10;							// Frame Terminator : Message end	DLE(0x10)
	txd[9] = 0x03;							// Frame Terminator : Message end	ETX(0x03)

	switch(cmd){
		case PTZ_STOP:
			switch(ptz->prev_cmd){
				case MENU_ON:
				case MENU_ENTER:
				case MENU_ESC:
				case FOCUS_AUTO:	break;
				case FOCUS_NEAR:txd[5] = 0x40;	break;
				case FOCUS_FAR:	txd[5] = 0x80;	break;
				default:
					if(ptz->prev_cmd&PAN_RIGHT)			txd[5] |= 0x01;
					else if(ptz->prev_cmd&PAN_LEFT)		txd[5] |= 0x02;
					else if(ptz->prev_cmd&TILT_UP)		txd[5] |= 0x04;
					else if(ptz->prev_cmd&TILT_DOWN)		txd[5] |= 0x08;
					if(ptz->prev_cmd&ZOOM_TELE)			txd[5] |= 0x10;
					else if(ptz->prev_cmd&ZOOM_WIDE)		txd[5] |= 0x20;
					break;
			}
			break;
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:
		case FOCUS_AUTO:	break;
		case FOCUS_NEAR:txd[5] = 0x40;	txd[7] = 0xbd;	break;
		case FOCUS_FAR:	txd[5] = 0x80;	txd[7] = 0xbd;	break;
		default:
			if(cmd&PAN_RIGHT)			{txd[5] |= 0x01;	txd[6] = 0xbd;}
			else if(cmd&PAN_LEFT)		{txd[5] |= 0x02;	txd[6] = 0xbd;}
			else if(cmd&TILT_UP)		{txd[5] |= 0x04;	txd[7] = 0xbd;}
			else if(cmd&TILT_DOWN)	{txd[5] |= 0x08;	txd[7] = 0xbd;}
			if(cmd&ZOOM_TELE)			{txd[5] |= 0x10;	txd[6] = 0xbd;}
			else if(cmd&ZOOM_WIDE)	{txd[5] |= 0x20;	txd[6] = 0xbd;}
			break;
	}

	ptz->prev_cmd = cmd;

	for(i=0;i<6;i++){
		crc16 ^= ((WORD)txd[2+i]);
		crc16 = crc_cal(crc16);
	}

	txd[10] = crc16>>8;						// checksum 2byte : CRC-16
	txd[11] = crc16;

	pkt_length = 12;

    return pkt_length;
}

static int enc_pkt_AD_SpeedDome(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = ptz->Addr;					// receive device ptz->Address

	switch(cmd){
		case MENU_ON:
		case MENU_ENTER:txd[1] = 0xcc;	txd[2] = 0x01; break;
		case MENU_ESC:	txd[1] = 0xcc;	txd[2] = 0x02; break;
		case PTZ_STOP:
			if((ptz->prev_cmd==FOCUS_NEAR) || (ptz->prev_cmd==FOCUS_FAR))		txd[1] = 0x89;
			else if((ptz->prev_cmd&PAN_RIGHT) || (ptz->prev_cmd&PAN_LEFT))	txd[1] = 0x83;
			else if((ptz->prev_cmd&TILT_UP) || (ptz->prev_cmd&TILT_DOWN))		txd[1] = 0x86;
			else if((ptz->prev_cmd&ZOOM_TELE) || (ptz->prev_cmd&ZOOM_WIDE))	txd[1] = 0x8c;
			break;
		case ZOOM_TELE:	txd[1] = 0x8a;	break;
		case ZOOM_WIDE:	txd[1] = 0x8b;	break;
		case FOCUS_NEAR:txd[1] = 0x87;	break;
		case FOCUS_FAR:	txd[1] = 0x88;	break;
		case FOCUS_AUTO:	break;

		case MENU_LEFT:
		case PAN_LEFT:	txd[1] = 0xc0; txd[2] = 0x81; txd[3] = 0x32;	break;
		case MENU_RIGHT:
		case PAN_RIGHT:	txd[1] = 0xc0; txd[2] = 0x82; txd[3] = 0x32;	break;
		case MENU_UP:
		case TILT_UP:	txd[1] = 0xc0; txd[2] = 0x84; txd[3] = 0x32;	break;
		case MENU_DOWN:
		case TILT_DOWN:	txd[1] = 0xc0; txd[2] = 0x85; txd[3] = 0x32;	break;
	}

	// checksum = subtract the sum of the bytes from zero and use the least significant byte of the results.
	if(txd[1] == 0xc0){
		txd[4] = 0x00 - (txd[0]+txd[1]+txd[2]+txd[3]);
		pkt_length = 5;
	}
	else if(txd[1] == 0xcc){
		txd[3] = 0x00 - (txd[0]+txd[1]+txd[2]);
		pkt_length = 4;
	}
	else{
		txd[2] = 0x00 - (txd[0]+txd[1]);
		pkt_length = 3;
	}

	ptz->prev_cmd = cmd;

    return pkt_length;
}

static int enc_pkt_VCLTP(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = 0x80 + ptz->Addr - 1;	// receive device ptz->Address

	switch(cmd){
		case PTZ_STOP:
			switch(ptz->prev_cmd){
				case PAN_LEFT:	txd[1] = 0x6c;	break;
				case PAN_RIGHT:	txd[1] = 0x72;	break;
				case TILT_UP:	txd[1] = 0x75;	break;
				case TILT_DOWN:	txd[1] = 0x6e;	break;
				case ZOOM_TELE:	txd[1] = 0x2a;	break;
				case ZOOM_WIDE:	txd[1] = 0x2b;	break;
				case FOCUS_NEAR:txd[1] = 0x2c;	break;
				case FOCUS_FAR:	txd[1] = 0x2d;	break;
				default:	break;
			}

			pkt_length = 2;
			break;

		case PAN_LEFT:	txd[1] = 0x4c;	txd[2] = 0x3f;	pkt_length = 3;	break;
		case PAN_RIGHT:	txd[1] = 0x52;	txd[2] = 0x3f;	pkt_length = 3;	break;
		case TILT_UP:	txd[1] = 0x55;	txd[2] = 0x3f;	pkt_length = 3;	break;
		case TILT_DOWN:	txd[1] = 0x4e;	txd[2] = 0x3f;	pkt_length = 3;	break;

		case ZOOM_TELE:	txd[1] = 0x3a;	pkt_length = 2;	break;
		case ZOOM_WIDE:	txd[1] = 0x3b;	pkt_length = 2;	break;

		case FOCUS_NEAR:txd[1] = 0x3c;	pkt_length = 2;	break;
		case FOCUS_FAR:	txd[1] = 0x3d;	pkt_length = 2;	break;
		default:	break;
	}

	ptz->prev_cmd = cmd;

    return pkt_length;
}

static int enc_pkt_LILIN_MLP2(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0xe0;							// Data Packet의 시작
	txd[1] = ptz->Addr;					// receive device ptz->Address

	switch(cmd){
		case PTZ_STOP:	txd[5] = 0xff;	break;
		case MENU_ON:	txd[2] = 0x1A;	break;
		case MENU_ENTER:txd[3] = 0x36;	break;
		case MENU_ESC:	txd[3] = 0x37;	break;
		case FOCUS_NEAR:txd[2] = 0x01;	break;
		case FOCUS_FAR:	txd[3] = 0x02;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[3] |= 0x01; txd[4] |= 0x0a;}
			else if(cmd&PAN_LEFT)	{txd[3] |= 0x02; txd[4] |= 0x0a;}
			if(cmd&TILT_UP)			{txd[3] |= 0x04; txd[4] |= 0xa0;}
			else if(cmd&TILT_DOWN)	{txd[3] |= 0x08; txd[4] |= 0xa0;}

			if(cmd&ZOOM_TELE)		{txd[3] |= 0x10; txd[5] |= 0x0a;}
			else if(cmd&ZOOM_WIDE)	{txd[3] |= 0x20; txd[5] |= 0x0a;}

			if(cmd&0x0f)	txd[3] |= 0x40;	// l/r/u/d
			if(cmd&0x30)	txd[3] |= 0x80;	// zoom
			break;
	}

	for(i=1;i<6;i++) txd[6] += txd[i];		// Byte2~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

static int enc_pkt_LILIN_FastDome(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	txd[0] = ptz->Addr;					// receive device ptz->Address

	switch(cmd){
		case PTZ_STOP:	txd[2] = 0xff;	break;
		case MENU_ON:	txd[1] = 0x06;	break;
		case MENU_ENTER:txd[1] = 0x06;	break;
		case MENU_ESC:	txd[1] = 0x03;	break;
//		case MENU_RIGHT:txd[1] = 0x01;	txd[2] |= 0x04; break;
//		case MENU_LEFT:	txd[1] = 0x02;	txd[2] |= 0x04; break;
//		case MENU_UP:	txd[1] = 0x04;	txd[2] |= (0x04<<3); break;
//		case MENU_DOWN:	txd[1] = 0x08;	txd[2] |= (0x04<<3); break;
		case FOCUS_NEAR:txd[1] = 0x80;	break;
		case FOCUS_FAR:	txd[1] = 0x40;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&PAN_RIGHT)		{txd[1] |= 0x01; txd[2] |= 0x04;}
			else if(cmd&PAN_LEFT)	{txd[1] |= 0x02; txd[2] |= 0x04;}
			if(cmd&TILT_UP)			{txd[1] |= 0x04; txd[2] |= (0x04<<3);}
			else if(cmd&TILT_DOWN)	{txd[1] |= 0x08; txd[2] |= (0x04<<3);}

			if(cmd&ZOOM_TELE)		txd[1] |= 0x10;
			else if(cmd&ZOOM_WIDE)	txd[1] |= 0x20;

			if(cmd&0x0f)	txd[2] |= 0x80;	// l/r/u/d
			break;
	}

	pkt_length = 3;

    return pkt_length;
}

static int enc_pkt_NUVICO_DOME(JniContext *ctx, BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
    BYTE i,checksum = 0;

	txd[0] = 0xE5;							// Data Packet Start
	txd[1] = 0x10;							// Device : 0x10 : Dome , 0x20 : DVR
	txd[2] = ptz->Addr;					// receive device ptz->Address LSB

	switch(cmd){
		case PTZ_STOP:
		case MENU_ON:
		case MENU_ENTER:
		case MENU_ESC:		break;
		case FOCUS_NEAR:txd[3] = 0x08;	break;
		case FOCUS_FAR:	txd[3] = 0x04;	break;
		case FOCUS_AUTO:	break;
		default:
			if(cmd&TILT_UP)			txd[5] = 0x0f;
			else if(cmd&TILT_DOWN)	txd[5] = 0x8f;
			if(cmd&PAN_LEFT)		txd[4] = 0x8f;
			else if(cmd&PAN_RIGHT)	txd[4] = 0x0f;

			if(cmd&ZOOM_TELE)		txd[3] = 0x13;
			else if(cmd&ZOOM_WIDE)	txd[3] = 0x1b;
			break;
	}

	for(i=0;i<6;i++) checksum += txd[i];

	txd[6] = checksum^0xA5;						// Byte1~Byte6을 더한 값(overflow 무시)

	pkt_length = 7;

    return pkt_length;
}

static int enc_pkt_SONY_VISCA(JniContext *ctx, BYTE cmd, BYTE *txd)
{
/*
PT:
up		81/01/06/01/05/05/03/01/FF	release 81/01/06/01/05/05/03/03/FF
down	81/01/06/01/05/05/03/02/FF	release 81/01/06/01/05/05/03/03/FF
left	81/01/06/01/05/05/01/03/FF	release 81/01/06/01/05/05/03/03/FF
right	81/01/06/01/05/05/02/03/FF	release 81/01/06/01/05/05/03/03/FF

ZF:
up		81/01/04/07/2p/FF			release 81/01/04/07/00/FF    
down	81/01/04/07/3p/FF			release 81/01/04/07/00/FF                               
left	81/01/04/08/03/FF			release 81/01/04/08/00/FF                               
right	81/01/04/08/02/FF			release 81/01/04/08/00/FF     
The monitor may also need to toggle the auto focus on and off with this command.
		81/01/04/38/10/FF  
*/
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;

	txd[0] = 0x80+ptz->Addr;				// MSB: SENDER'S ptz->Address   LSB: RECEIVER'S ptz->Address
	txd[1] = 0x01;							// 

	switch(cmd){
		case PTZ_STOP:
			if((ptz->prev_cmd==PAN_LEFT) || (ptz->prev_cmd==PAN_RIGHT) || (ptz->prev_cmd==TILT_UP) || (ptz->prev_cmd==TILT_DOWN)){
				txd[6] = 0x03; txd[7] = 0x03;
			}
			else	txd[4] = 0x00;
			break;
		case TILT_UP:		txd[6] = 0x03; txd[7] = 0x01;	break;
		case TILT_DOWN:		txd[6] = 0x03; txd[7] = 0x02;	break;
		case PAN_LEFT:		txd[6] = 0x01; txd[7] = 0x03;	break;
		case PAN_RIGHT:		txd[6] = 0x02; txd[7] = 0x03;	break;
		case ZOOM_TELE:		txd[4] = 0x24;	break;
		case ZOOM_WIDE:		txd[4] = 0x34;	break;
		case FOCUS_FAR:		txd[4] = 0x02;	break;
		case FOCUS_NEAR:	txd[4] = 0x03;	break;
		case FOCUS_AUTO:	txd[3] = 0x18; txd[4] = 0x01;	break;
/*
		case EXT_01:		txd[3] = 0x38; txd[4] = 0x02;	break;	//AF_ON:			
		case EXT_02:		txd[3] = 0x38; txd[4] = 0x03;	break;	//AF_OFF:		    
		case EXT_03:		txd[3] = 0x39; txd[4] = 0x00;	break;	//EXPOSR_AUTO:	
		case EXT_04:		txd[3] = 0x39; txd[4] = 0x03;	break;	//EXPOSR_MANUL:	
		case EXT_05:		txd[3] = 0x0b; txd[4] = 0x02;	break;	//IRIS_UP:		
		case EXT_06:		txd[3] = 0x0b; txd[4] = 0x03;	break;	//IRIS_DN:		
		case EXT_07:		txd[3] = 0x5a; txd[4] = 0x02;	break;	//SHTR_SLOWON:	
		case EXT_08:		txd[3] = 0x5a; txd[4] = 0x03;	break;	//SHTR_SLOWOFF:	
		case EXT_09:		txd[3] = 0x3d; txd[4] = 0x02;	break;	//AUTO_WD_ON:	    
		case EXT_10:		txd[3] = 0x3d; txd[4] = 0x03;	break;	//AUTO_WD_OFF:	
*/
	}

	if(cmd == PTZ_STOP)	cmd = ptz->prev_cmd;
	else				ptz->prev_cmd = cmd;

	switch(cmd){
		case PAN_LEFT:
		case PAN_RIGHT:
		case TILT_UP:
		case TILT_DOWN:		txd[2] = 0x06; txd[3] = 0x01; txd[4] = 0x05; txd[5] = 0x05; txd[8] = 0xff; pkt_length = 9;	break;
		case ZOOM_TELE:
		case ZOOM_WIDE:		txd[2] = 0x04; txd[3] = 0x07; txd[5] = 0xff; pkt_length = 6;	break;
		case FOCUS_NEAR:
		case FOCUS_FAR:		txd[2] = 0x04; txd[3] = 0x08; txd[5] = 0xff; pkt_length = 6;	break;
		case FOCUS_AUTO:	break;
/*
		case EXT_01:
		case EXT_02:
		case EXT_03:
		case EXT_04:
		case EXT_05:
		case EXT_06:
		case EXT_07:
		case EXT_08:
		case EXT_09:
		case EXT_10:	txd[2] = 0x04; txd[5] = 0xff; pkt_length = 6;	break;
*/
	}

    return pkt_length;
}

static int enc_pkt_LG_KPC_Z180(JniContext *ctx, BYTE cmd, BYTE *txd)	//use Key Control command
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xc5;							// Header
	txd[4] = ptz->Addr;					// ID(0x01 ~ 0x80)
	txd[1] = 0x5f;							// CMD
//	txd[2] = 0x00;							// Data
//	txd[3] = 0x00;							// Data

	switch(cmd){
		case MENU_ON:	txd[2] = 0x25;	break;
			//case MENU_ENTER:txd[2] = 0x4f;	break;
		case MENU_ESC:	txd[2] = 0x24;	break;
		case MENU_UP:	txd[2] = 0x4f;	break;
		case MENU_DOWN:	txd[2] = 0x26;	break;
		case MENU_LEFT:	txd[2] = 0x28;	break;
		case MENU_RIGHT:txd[2] = 0x27;	break;
			//case TILT_UP:	txd[2] = 0x36;	break;
			//case TILT_DOWN:	txd[2] = 0x37;	break;
			//case PAN_LEFT:	txd[2] = 0x38;	break;
			//case PAN_RIGHT:	txd[2] = 0x39;	break;
		case FOCUS_FAR:	txd[2] = 0x38;	break;
		case FOCUS_NEAR:txd[2] = 0x39;	break;
		case ZOOM_TELE:	txd[2] = 0x7c;	break;
		case ZOOM_WIDE:	txd[2] = 0x80;	break;
		case PTZ_STOP:	txd[2] = 0x0c;	break;
	}

	for(i=0;i<5;i++) txd[5] += txd[i];		// Check Sum : (sum of Byte1~Byte5) & 0x7f

	pkt_length = 6;

    return pkt_length;
}

static int enc_pkt_CNB_ZxN_20(JniContext *ctx, BYTE cmd, BYTE *txd)		//use Key Control command
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0x2A;							// HEADER

	txd[1] = (ptz->Addr/10)| 0x30;		// CAMERA ID
	txd[2] = (ptz->Addr%10)| 0x30;		// CAMERA ID

	txd[3] = '7';							// COMMAND MESSAGE
	txd[4] = '5';							// COMMAND MESSAGE

	txd[5] = '0';							// COMMAND OPTION
	txd[6] = '0';							// COMMAND OPTION
	txd[7] = '0';							// COMMAND OPTION
	txd[8] = '0';							// COMMAND OPTION

	switch(cmd){
		case MENU_ON:	txd[5] = '1';	txd[6] = '0';	break;
		case MENU_ESC:	txd[5] = '0';	txd[6] = 'F';	break;
		case MENU_ENTER:txd[5] = '1';	txd[6] = '0';	break;
		case MENU_UP:	txd[5] = '0';	txd[6] = '1';	break;
		case MENU_DOWN:	txd[5] = '0';	txd[6] = '3';	break;
//		case MENU_LEFT:	txd[3] = 0x04;	txd[4] = 0x30;	break;
//		case MENU_RIGHT:txd[3] = 0x02;	txd[4] = 0x30;	break;
		case PTZ_STOP:		break;
		case PAN_RIGHT:	txd[5] = '3';	txd[6] = 'C';	break;
		case PAN_LEFT:	txd[5] = '3';	txd[6] = 'D';	break;
		case TILT_UP:	txd[5] = '3';	txd[6] = 'E';	break;
		case TILT_DOWN:	txd[5] = '3';	txd[6] = 'F';	break;
		case ZOOM_TELE:	txd[5] = '0';	txd[6] = '1';	break;
		case ZOOM_WIDE:	txd[5] = '0';	txd[6] = '3';	break;
		case FOCUS_FAR:	txd[5] = '0';	txd[6] = '5';	break;
		case FOCUS_NEAR:txd[5] = '0';	txd[6] = '6';	break;
	}

	for(i=0;i<9;i++) checksum += txd[i];

	txd[9] = HextoAsciiHi(checksum);						// Byte1~Byte6을 더한 값(overflow 무시)
	txd[10]= HextoAsciiLo(checksum);

	pkt_length = 11;

    return pkt_length;
}
/*
void enc_pkt_Heijmans(BYTE cmd, BYTE *txd)		//use Key Control command 
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i,checksum = 0;

	txd[0] = 0x02;							// stx

	txd[1] = HextoAsciiHi(ptz->Addr);	// address
	txd[2] = HextoAsciiLo(ptz->Addr);	//

	txd[3] = '0';							// message type: 1 = ptz
	txd[4] = '1';							// 				 2 = focus

	switch(cmd){
		case PTZ_STOP:
			if((ptz->prev_cmd==FOCUS_FAR) || (ptz->prev_cmd==FOCUS_NEAR) || (ptz->prev_cmd==FOCUS_AUTO)){
				  txd[4] = '2'; txd[5] = '0'; i = 5;
			}
			else{ txd[5] = '0'; txd[6] = '0'; i = 6; }
		break;
		case MENU_ON:	
		case MENU_ESC:	
		case MENU_ENTER:	break;
		case PRST_CALL:
			txd[4] = '4'; 								// message type: Prepos Show
			txd[5] = HextoAsciiHi(ctx->PrstNum);		//
			txd[6] = HextoAsciiLo(ctx->PrstNum);		//
			i = 6;
		break;
		case PRST_SET:
			txd[4] = '5'; 								// message type: Prepos Save
			txd[5] = HextoAsciiHi(ctx->PrstNum);		//
			txd[6] = HextoAsciiLo(ctx->PrstNum);		//
			i = 6;
		break;
		case NORTH_SET:	txd[4] = '8'; i = 4; break;		// message type: Set Current Position as North
		case NORTH_GO:	txd[4] = '9'; i = 4; break;		// message type: Show North Position
		case FOCUS_FAR:
			txd[4] = '2'; txd[5] = '1';					// message type: focus
			txd[6] = '3'; txd[7] = '2';					// Speed: [00x .. 0x64] (for far or near)
			i = 7;
		break;
		case FOCUS_NEAR:
			txd[4] = '2'; txd[5] = '2';
			txd[6] = '3'; txd[7] = '2';
			i = 7;
		break;
		case FOCUS_AUTO:
			txd[4] = '2'; txd[5] = '3';
			txd[6] = '3'; txd[7] = '2';
			i = 7;
		break;
		default:
			txd[5] = HextoAsciiHi(cmd);					// data : control bit - 0 0 ZO ZI TD TU PL PR 
			txd[6] = HextoAsciiLo(cmd);					// 

			txd[7] = '0'; txd[8] = '0';					// Pan Speed  [00 .. 64]
			txd[9] = '0'; txd[10]= '0';					// Tilt Speed [00 .. 64]
			txd[11]= '0'; txd[12]= '0';					// Zoom Speed [00 .. 64]
			if(cmd&PAN_RIGHT)		txd[7] = '4';
			else if(cmd&PAN_LEFT)	txd[7] = '4';
			if(cmd&TILT_UP)			txd[9] = '4';
			else if(cmd&TILT_DOWN)	txd[9] = '4';
		
			if(cmd&ZOOM_TELE)		txd[11]= '4';
			else if(cmd&ZOOM_WIDE)	txd[11]= '4';
			i = 12;
		break;
	}

	checksum = calculate_crc(&txd[1],i++);	// XOR over all previous bytes in the data block
	txd[i++] = 0x03;						// etx
	txd[i++] = HextoAsciiHi(checksum);
	txd[i++] = HextoAsciiLo(checksum);

	ptz->prev_cmd = cmd;

	pkt_length = i;

    return pkt_length;
}

void enc_pkt_DoDAMM(BYTE cmd, BYTE *txd)
{
    const Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;

	txd[0] = 0xFF;							// Data Packet의 시작
	txd[1] = ptz->Addr;					// receive device ptz->Address

	switch(cmd){
		case PTZ_STOP:		break;
		case PAN_RIGHT:		txd[3] = 0x02; txd[4] = 0x30;	break;
		case PAN_LEFT:		txd[3] = 0x04; txd[4] = 0x30;	break;
		case TILT_UP:		txd[3] = 0x08; txd[5] = 0x30;	break;
		case TILT_DOWN:		txd[3] = 0x10; txd[5] = 0x30;	break;
		case ZOOM_TELE:		txd[3] = 0x20;	break;
		case ZOOM_WIDE:		txd[3] = 0x40;	break;
		case FOCUS_FAR:		txd[3] = 0x80;	break;
		case FOCUS_NEAR:	txd[2] = 0x01;	break;

		case EXT_01:		txd[2] = 0x88; break;					//C_CAM_ON:		    
		case EXT_02:		txd[2] = 0x08; break;					//C_CAM_OFF:		
		case EXT_03:		txd[2] = 0x88; txd[4] = 0x01;	break;	//T_CAM_ON:		    
		case EXT_04:		txd[2] = 0x08; txd[4] = 0x01;	break;	//T_CAM_OFF:		
		case EXT_05:		txd[3] = 0x35; txd[4] = 0x00;	break;	//C_CAM_DAY:		
		case EXT_06:		txd[3] = 0x35; txd[4] = 0x01;	break;	//C_CAM_NIGHT:	    
		case EXT_07:		txd[3] = 0x31; txd[4] = 0x0d;	break;	//T_CAM_PAL_N:	    
		case EXT_08:		txd[3] = 0x31; txd[4] = 0x0e;	break;	//T_CAM_PAL_P:	    
		case EXT_09:		txd[2] = 0x88; txd[4] = 0x05;	break;	//WIPER_RUN:
		case EXT_10:		txd[2] = 0x08; txd[4] = 0x05;	break;	//WIPER_OFF:
	}

	for(i=1;i<6;i++) txd[6] += txd[i];		 // Byte2~Byte6을 더한 값(overflow 무시)
	
	pkt_length = 7;

    return pkt_length;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
static void Send_RptSet(Ptz_Config *ptz, BYTE cmd, BYTE repeat_time)
{
	if(cmd == PTZ_STOP){
        ptz->Rpt_Flag = 0;
        ptz->Rpt_Time = 0;
	}
	else{
        ptz->Rpt_Flag = 1;
        ptz->Rpt_Time = repeat_time;
	}
}

static int Send_PtzData(JniContext *ctx, BYTE cmd, BYTE *pkt_data)
{
    Ptz_Config *ptz = ctx->ptz;
    int pkt_length;
	BYTE i;
	BYTE *pkt_ptr = &pkt_data[4];

	pkt_data[0] = 0xa0;
	pkt_data[1] = 0xff;
	pkt_data[2] = 'P';
	pkt_data[3] = ptz->prev_len;

    Delay1ms(10);

    switch(ptz->Protocol)
	{
		case Dongyang_Unitech:		pkt_length = enc_pkt_D_MAX(ctx, cmd, pkt_ptr);			break;
		case Dongyang_DY_255RXC:	pkt_length = enc_pkt_DY_255RXC(ctx, cmd, pkt_ptr);		break;
		case Fine_System:			pkt_length = enc_pkt_FineSystem(ctx, cmd, pkt_ptr);		break;
		case Hitron_HID_2404:
		case Honeywell_ScanDome2:	pkt_length = enc_pkt_Honeywell(ctx, cmd, pkt_ptr);		break;
		case InterM_VRx_2201:		pkt_length = enc_pkt_InterM_VRx_2201(ctx, cmd, pkt_ptr);break;
		case Panasonic_C:			pkt_length = enc_pkt_Panasonic_C(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 50);	break;
		case Panasonic_N:			pkt_length = enc_pkt_Panasonic_N(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 50);	break;
		case Pelco_D:				pkt_length = enc_pkt_Pelco_D(ctx, cmd, pkt_ptr);		break;
		case Pelco_D_CNB1:			pkt_length = enc_pkt_Pelco_D_CNB1(ctx, cmd, pkt_ptr);	break;
		case Pelco_D_CNB2:			pkt_length = enc_pkt_Pelco_D_CNB2(ctx, cmd, pkt_ptr);	break;
		case Pelco_P:				pkt_length = enc_pkt_Pelco_P(ctx, cmd, pkt_ptr);		break;
		case Samsung:				pkt_length = enc_pkt_Samsung(ctx, cmd, pkt_ptr);		break;
		case Samsung_Techwin:		pkt_length = enc_pkt_Techwin(ctx, cmd, pkt_ptr);		break;
		case Sungjin_SJ_100:		pkt_length = enc_pkt_Sungjin_SJ_100(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 30);	break;
		case Sungjin_SJ_1000:		pkt_length = enc_pkt_Sungjin_SJ_1000(ctx, cmd, pkt_ptr);Send_RptSet(ptz, cmd, 30);	break;
		case Sysmania:				pkt_length = enc_pkt_Sysmania(ctx, cmd, pkt_ptr);		Send_RptSet(ptz, cmd, 4);	break;
		case Vicon_Stn:				pkt_length = enc_pkt_Vicon_Stn(ctx, cmd, pkt_ptr);		break;
		case Vicon_Ext:				pkt_length = enc_pkt_Vicon_Ext(ctx, cmd, pkt_ptr);		break;
		case Ikegami_PCS_35:		pkt_length = enc_pkt_Ikegami_PCS_35(ctx, cmd, pkt_ptr);	break;
		case Ikegami_PCS_358:		pkt_length = enc_pkt_Ikegami_PCS_358(ctx, cmd, pkt_ptr);break;
		case New_Born_Hightech:		pkt_length = enc_pkt_NIKO(ctx, cmd, pkt_ptr);			break;
		case TOKINA_DMP:			pkt_length = enc_pkt_TOKINA_DMP(ctx, cmd, pkt_ptr);		break;
		case LG_MultiX:				pkt_length = enc_pkt_LG_MultiX(ctx, cmd, pkt_ptr);		break;
		case LG_LPT_A100L:			pkt_length = enc_pkt_LG_LPT_A100L(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 10);	break;
		case Ernitec:				pkt_length = enc_pkt_ERNA(ctx, cmd, pkt_ptr);			Send_RptSet(ptz, cmd, 50);	break;
		case Bosch_OSRD:			pkt_length = enc_pkt_Bosch_OSRD(ctx, cmd, pkt_ptr);		Send_RptSet(ptz, cmd, 5);	break;
		case Bosch_BiCom:			pkt_length = enc_pkt_Bosch_BiCom(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 5);	break;
		case Cyber_Scan1:			pkt_length = enc_pkt_Cyber_Scan1(ctx, cmd, pkt_ptr);	break;
		case Yujin_System:			pkt_length = enc_pkt_Yujin_System(ctx, cmd, pkt_ptr);	break;
		case Ladon:
		case Dynacolor_DSCP:		pkt_length = enc_pkt_Dynacolor_DSCP(ctx, cmd, pkt_ptr);	break;
		case MCU_1200N:				pkt_length = enc_pkt_MCU_1200N(ctx, cmd, pkt_ptr);		break;
		case AD_SpeedDome:			pkt_length = enc_pkt_AD_SpeedDome(ctx, cmd, pkt_ptr);	Send_RptSet(ptz, cmd, 50);	break;
		case VCLTP:					pkt_length = enc_pkt_VCLTP(ctx, cmd, pkt_ptr);			break;
		case LILIN_MLP2:			pkt_length = enc_pkt_LILIN_MLP2(ctx, cmd, pkt_ptr);		break;
		case LILIN_FastDome:		pkt_length = enc_pkt_LILIN_FastDome(ctx, cmd, pkt_ptr);	break;
		case NUVICO:				pkt_length = enc_pkt_NUVICO_DOME(ctx, cmd, pkt_ptr);	break;
		case SONY_VISCA:			pkt_length = enc_pkt_SONY_VISCA(ctx, cmd, pkt_ptr);		break;
		case LG_KPC_Z180:			pkt_length = enc_pkt_LG_KPC_Z180(ctx, cmd, pkt_ptr);	break;
		case CNB_ZxN_20:			pkt_length = enc_pkt_CNB_ZxN_20(ctx, cmd, pkt_ptr);		break;
//		case Heijmans:				pkt_length = enc_pkt_Heijmans(ctx, cmd, pkt_ptr);		break;
//		case DoDAMM:				pkt_length = enc_pkt_DoDAMM(ctx, cmd, pkt_ptr);			break;
	}

	if(pkt_length > 0)
	{
		pkt_data[3] = pkt_length;
	}
    else
    {
        return -1;
    }

    ptz->prev_len = (BYTE)pkt_length;

    return pkt_length;
}

static int Set_Ptz_Conf(JNIEnv *env, Ptz_Config *config, PtzProtocol ptzProtocol, uint8_t ptzAddress)
{
    if (ptzProtocol >= PTZ_PROTOCOL_MAX) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Could not init PTZ writer context");
        return -1;
    }

    config->Protocol  = (BYTE)ptzProtocol;
    config->Addr      = ptzAddress;
	config->Menu_Ctrl = protocol_conf[ptzProtocol].menu_ctrl;
	if(protocol_conf[ptzProtocol].menu_ctrl >= _ON)
		config->Ctrl_Mode = MODE_MENU;
	else
        config->Ctrl_Mode = MODE_PT;
	config->Ptz_Mode  = MODE_PT;
	config->Command	  = PTZ_END;
	config->Rpt_Flag  = 0;
	config->Rpt_Time  = 0;
	//config->Pkt_Length  = 0;

    // TODO rx1이 아니라 rx0를 초기화 하는 이유는?
//	rx0.buff_cnt = 0;	rx0.read_cnt = 0;	rx0.step = 0;

    return 0;
}

static void
com_sscctv_seeeyes_ptz_PtzWriter_init(JNIEnv *env, jobject thiz, jint protocol, jchar address)
{
    Ptz_Config *context;

    context = new Ptz_Config;
    if (!context) {
        jniThrowException(env, "java/lang/RuntimeException", "Could not alloc PTZ writer context");
        return;
    }
    if (Set_Ptz_Conf(env, context, (PtzProtocol)protocol, (BYTE)address) < 0) {
        delete context;
        return;
    }
    env->SetLongField(thiz, field_context, (long)context);
}

static void
com_sscctv_seeeyes_ptz_PtzWriter_set_mode(JNIEnv *env, jobject thiz, jint mode)
{
	Ptz_Config *context = (Ptz_Config *)env->GetLongField(thiz, field_context);

	context->Ctrl_Mode = mode;
}

static jint
com_sscctv_seeeyes_ptz_PtzWriter_encode_command(JNIEnv *env, jobject thiz, jobject buffer, jchar command)
{
    Ptz_Config *context = (Ptz_Config *)env->GetLongField(thiz, field_context);
    JniContext jni;

    jbyte* buf = (jbyte *)env->GetDirectBufferAddress(buffer);
    jlong capacity = env->GetDirectBufferCapacity(buffer);
    if (!buf || capacity < PACKET_SIZE_MAX) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "ByteBuffer not direct or too small");
        return -1;
    }

    jni.env = env;
    jni.thiz = thiz;
    jni.buffer = buffer;
    jni.ptz = context;

    // 실제 인코딩을 실행
    int pkt_length = Send_PtzData(&jni, command, (BYTE *)buf);
    if (pkt_length < 0) {
        jniThrowException(env, "java/lang/RuntimeException", "Failed to encode command");
        return -1;
    }

    // Java class의 sendBuffer()를 호출한다
    sendBuffer(&jni, pkt_length, 0);

    return pkt_length;
}

static void
com_sscctv_seeeyes_ptz_PtzWriter_exit(JNIEnv *env, jobject thiz)
{
    Ptz_Config *context = (Ptz_Config *)env->GetLongField(thiz, field_context);
    delete context;
    env->SetLongField(thiz, field_context, 0);
}

static JNINativeMethod method_table[] = {
    { "native_init",            "(IC)V",(void *)com_sscctv_seeeyes_ptz_PtzWriter_init },
	{ "native_set_mode",        "(I)V", (void *)com_sscctv_seeeyes_ptz_PtzWriter_set_mode },
    { "native_encode_command",  "(Ljava/nio/ByteBuffer;C)I", (void *)com_sscctv_seeeyes_ptz_PtzWriter_encode_command },
    { "native_exit",            "()V",  (void *)com_sscctv_seeeyes_ptz_PtzWriter_exit },
};

int register_com_sscctv_seeeyes_ptz_PtzWriter(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/sscctv/seeeyes/ptz/PtzWriter");
    if (clazz == NULL) {
        LOGE("Can't find com/sscctv/seeeyes/ptz/PtzWriter");
        return -1;
    }

    // 나중에 참조할 필드 ID를 기억해 둔다.
    field_context = env->GetFieldID(clazz, "mContext", "J");
    if (field_context == NULL) {
        LOGE("Can't find PtzWriter.mContext");
        return -1;
    }

    method_send_buffer = env->GetMethodID(clazz, "sendBuffer", "(Ljava/nio/ByteBuffer;II)V");
    if (method_send_buffer == NULL) {
        LOGE("Can't find PtzWriter.sendBuffer");
        return -1;
    }

    return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/ptz/PtzWriter",
                                    method_table, sizeof(method_table)/sizeof(method_table[0]));
}

//int register_com_sscctv_seeeyes_ptz_PtzUcc(JNIEnv *env)
//{
//	jclass clazz = env->FindClass("com/sscctv/seeeyes/ptz/PtzUcc");
//	if (clazz == NULL) {
//		LOGE("Can't find com/sscctv/seeeyes/ptz/PtzUcc");
//		return -1;
//	}
//
//	// 나중에 참조할 필드 ID를 기억해 둔다.
//	field_context = env->GetFieldID(clazz, "mContext", "J");
//	if (field_context == NULL) {
//		LOGE("Can't find PtzWriter.mContext");
//		return -1;
//	}
//
//	method_send_buffer = env->GetMethodID(clazz, "sendBuffer", "(Ljava/nio/ByteBuffer;II)V");
//	if (method_send_buffer == NULL) {
//		LOGE("Can't find PtzUcc.sendBuffer");
//		return -1;
//	}
//
//	return jniRegisterNativeMethods(env, "com/sscctv/seeeyes/ptz/PtzUcc",
//									method_table, sizeof(method_table)/sizeof(method_table[0]));
//}
