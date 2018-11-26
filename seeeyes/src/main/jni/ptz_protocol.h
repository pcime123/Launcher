#ifndef __PTZ_PROTOCOL_H__
#define __PTZ_PROTOCOL_H__

#include "define.h"

//#define	HextoAsciiHi(x)		((x>>4)<10)?((x>>4)+0x30):((x>>4)+0x37)
//#define	HextoAsciiLo(x)		((x&0x0f)<10)?((x&0x0f)+0x30):((x&0x0f)+0x37)

////////////////////////////////////////////////////////////////////////////////////////////////////
// PTZ Protocol Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
//#define	PTZ_CTRL_PORT			UART_0

#define	PROTOCOL_MAX_NUM		41
#define	PROTOCOL_COAX_MAX_NUM	1

#define PACKET_SIZE_MAX			32
#define	HEADER_LENGTH			4

enum{
	MODE_ZF=0,
	MODE_PT,
	MODE_MENU,
	MODE_EXT,
};

enum {
	PAN_RIGHT	= _BIT0,
	PAN_LEFT	= _BIT1,
	TILT_UP		= _BIT2,
	TILT_DOWN	= _BIT3,
	ZOOM_TELE	= _BIT4,
	ZOOM_WIDE	= _BIT5,

	PTZ_UPRT	= _BIT2|_BIT0,
	PTZ_UPLT	= _BIT2|_BIT1,
	PTZ_DNRT	= _BIT3|_BIT0,
	PTZ_DNLT	= _BIT3|_BIT1,

	FOCUS_FAR	= _BIT5|_BIT4,
	FOCUS_NEAR	= _BIT5|_BIT4|_BIT0,
	FOCUS_AUTO	= _BIT5|_BIT4|_BIT1,
/*
	EXT_01		= _BIT5|_BIT4|_BIT2,
	EXT_02		= _BIT5|_BIT4|_BIT2|_BIT0,
	EXT_03		= _BIT5|_BIT4|_BIT2|_BIT1,
	EXT_04		= _BIT5|_BIT4|_BIT2|_BIT1|_BIT0,
	EXT_05		= _BIT5|_BIT4|_BIT3,
	EXT_06		= _BIT5|_BIT4|_BIT3|_BIT0,
	EXT_07		= _BIT5|_BIT4|_BIT3|_BIT1,
	EXT_08		= _BIT5|_BIT4|_BIT3|_BIT1|_BIT0,
	EXT_09		= _BIT5|_BIT4|_BIT3|_BIT2,
	EXT_10		= _BIT5|_BIT4|_BIT3|_BIT2|_BIT0,

	AF_OFF		= _BIT5|_BIT4|_BIT2,
	AF_ON		= _BIT5|_BIT4|_BIT2|_BIT0,
	EXPOSR_AUTO	= _BIT5|_BIT4|_BIT2|_BIT1,
	EXPOSR_MANUL= _BIT5|_BIT4|_BIT2|_BIT1|_BIT0,
	IRIS_DN		= _BIT5|_BIT4|_BIT3,
	IRIS_UP		= _BIT5|_BIT4|_BIT3|_BIT0,
	SHTR_SLOWOFF= _BIT5|_BIT4|_BIT3|_BIT1,
	SHTR_SLOWON	= _BIT5|_BIT4|_BIT3|_BIT1|_BIT0,
	AUTO_WD_OFF	= _BIT5|_BIT4|_BIT3|_BIT2,
	AUTO_WD_ON	= _BIT5|_BIT4|_BIT3|_BIT2|_BIT0,

	C_CAM_OFF	= _BIT5|_BIT4|_BIT2,
	C_CAM_ON	= _BIT5|_BIT4|_BIT2|_BIT0,
	T_CAM_OFF	= _BIT5|_BIT4|_BIT2|_BIT1,
	T_CAM_ON	= _BIT5|_BIT4|_BIT2|_BIT1|_BIT0,
	C_CAM_DAY	= _BIT5|_BIT4|_BIT3,
	C_CAM_NIGHT	= _BIT5|_BIT4|_BIT3|_BIT0,
	T_CAM_PAL_N	= _BIT5|_BIT4|_BIT3|_BIT1,
	T_CAM_PAL_P	= _BIT5|_BIT4|_BIT3|_BIT1|_BIT0,
	WIPER_RUN	= _BIT5|_BIT4|_BIT3|_BIT2,
*/
	PRST_SET	= _BIT5|_BIT4|_BIT3|_BIT0,
	PRST_CALL	= _BIT5|_BIT4|_BIT3|_BIT1,
	NORTH_SET	= _BIT5|_BIT4|_BIT3|_BIT1|_BIT0,
	NORTH_GO	= _BIT5|_BIT4|_BIT3|_BIT2,

	MENU_RIGHT	= _BIT6|_BIT0,
	MENU_LEFT	= _BIT6|_BIT1,
	MENU_UP		= _BIT6|_BIT2,
	MENU_DOWN	= _BIT6|_BIT3,
	MENU_ENTER	= _BIT6|_BIT4,
	MENU_ESC	= _BIT6|_BIT5,
	MENU_ON		= _BIT6|_BIT5|_BIT4,
	MENU_OFF	= _BIT6|_BIT5|_BIT4|_BIT3,

	PTZ_STOP	= _BIT7,
	CS_ERR		= _BIT7|_BIT3,
	PTZ_END		= 0xff
};

typedef enum {
	Dongyang_Unitech = 0,
	Dongyang_DY_255RXC,
	Fine_System,				// 
	Hitron_HID_2404,
	Honeywell_ScanDome2,
	InterM_VRx_2201,
	LG_MultiX,
	LG_LPT_A100L,
	Panasonic_C,
	Panasonic_N,
	Pelco_D,
	Pelco_P,
	Samsung,
	Samsung_Techwin,
	Sungjin_SJ_100,
	Sungjin_SJ_1000,
	Sysmania,
	Vicon_Stn,
	Vicon_Ext,
	Ikegami_PCS_35,
	Ikegami_PCS_358,
	New_Born_Hightech,			// NIKO Korea
	TOKINA_DMP,
	Ernitec,
	Bosch_OSRD,
	Bosch_BiCom,
	Cyber_Scan1,
	Yujin_System,
	Dynacolor_DSCP,
	Ladon,
	MCU_1200N,
	AD_SpeedDome,
	VCLTP,
	LILIN_MLP2,
	LILIN_FastDome,
	Pelco_D_CNB1,
	Pelco_D_CNB2,
	NUVICO,
	SONY_VISCA,
	LG_KPC_Z180,
	CNB_ZxN_20,
//	Heijmans,
//	DoDAMM,

    // 다음이 마지막이어야 함
    PTZ_PROTOCOL_MAX
} PtzProtocol;

enum{
	PELCO_C,                                                                                                                                                       
	A1_CCVC,
};

#define RA_BUFF_SIZE	8
#define RA_BUFF_MASK	RA_BUFF_SIZE-1

typedef struct{
    // Read-Only
	BYTE Protocol;
	BYTE Addr;
	BYTE Command;
	BYTE Menu_Ctrl;		// PTZ osd menu mode control
	BYTE Ctrl_Mode;		// PTZ osd menu mode control
	BYTE Ptz_Mode;		// PTZ mode control

    // Read-Write
	BYTE Rpt_Flag;		// PTZ control signal repeat TX
	BYTE Rpt_Time;		// PTZ control signal repeat next TX time
//	BYTE Pkt_Length;

    BYTE prev_len;
    BYTE prev_cmd;
} Ptz_Config;

typedef struct{
	BYTE menu_ctrl;
	BYTE pkt_len;
} Ptz_Protocol_Conf;

typedef struct{
    // Read-Only
    BYTE Protocol;

	BYTE buff[PACKET_SIZE_MAX];
	BYTE buff_cnt;
	BYTE length;
	BYTE step;
} Rx_Analyze_Conf;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//extern WORD code Protocol_Name_Str[PROTOCOL_MAX_NUM][15];
//extern WORD code Protocol_COAX_Name_Str[2][15];
extern const code Ptz_Protocol_Conf protocol_conf[PROTOCOL_MAX_NUM];

WORD crc_cal(WORD uiCrc);

#endif
