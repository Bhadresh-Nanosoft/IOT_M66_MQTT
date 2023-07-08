/*
 * declaration.h
 *
 *  Created on: Mar 23, 2019
 *      Author: SW
 */

#ifndef DECLARATION_H_
#define DECLARATION_H_

#include "ql_type.h"


#define		SET_TIMEOUT_TIMER				Timeout_Flag=0;time_out_timer=20;timer_cnt=0;timer_start=1;
#define 	CLR_TIMEOUT_TIMER				Timeout_Flag=0;timer_cnt=0;timer_start=0;


#define			POWER_PIN					 PINNAME_RI
#define			TEMPER_SWITCH_INT			 PINNAME_DTR

#define 	GPRS_TIMEOUT					120			//	2 M

#define		RS485_TIME_INTERVAL				30
#define		ON								1
#define		OFF								0
#define 	RESET							0

/*********************************** FILE SYSTEM ************************************************/
s32 filehandle;



/*******************************************************************************************/
#define  SOC_RECV_BUFFER_LEN  1460
extern bool timer_start;
extern u32 time_out_timer;
extern u32 timer_cnt;
extern u32 cnt,comma_cnt;
extern s32 Status,iRet,File_Ret;
extern u32 ber,length,dataLen,len;
extern s32 rssi,gsm_signal;
extern s32 ret,pdpCntxtId;
extern s32 Primary_SocketId;

extern u8 strmsg[170],password_str[10];
extern u8 response[350];
extern u8 m_SocketRcvBuf[SOC_RECV_BUFFER_LEN];
extern u8 strAT[50];
extern u8 sms_str[160];
extern u8 strImei[30];
extern u8 volatile Imei_No[30];
extern u8 strImsi[30];
extern u8 volatile Imsi_No[30];
extern u8 store_imsi[30];
extern u8 interval[10];
extern u8 NO_OF_HW[10];
extern u8 config_array[5];
extern u32 config_command;


extern u8 ip_addr[5];
extern u8 history_send_Data[300];
extern u8 operator_code[6][10];
extern u32 operator_len;
extern u8 PhNum[20];
extern u8 Admin_no[20];
extern u8 Number[20];
extern bool Select_Operator;
extern bool sms_flag;

extern u8 primary_server[50];
extern u8 primary_port[10];

extern u8 APN[35];
extern u32 port1;

extern bool msg_extract_done;
extern u32 manual_mode;

extern bool send_flag;
extern u32 watchdog_cnt;
extern bool device_reset;

extern bool sms_init_done,sms_read_error;

extern u8 DATE[15];
extern u8 TIME[15];
extern bool mqtt_rcv_flag,Timeout_Flag;
extern u32 fmeter_length;
extern bool bat_disconnect_stat;
extern u32 error;
extern u32 publish_timer,fmeter_interval;
extern bool get_fmeter_data;

extern bool connection_est,ping_flag;
extern u8 modbus_str[200];
extern u32 gsm_error;
extern bool publish_urc,modbus_urc;
/********************************************** FLOWMETER VARIABLES ******************************************/
extern float flow,TOT1,TOT2,RTOT1,RTOT2;
extern u32 unit;
extern u32 version;
extern u8 fmeter_data[200];


/****************************************** FOTA **********************************************/
//extern volatile u8 FOTA_password[20];
//extern volatile u8 FOTA_path[20];
//extern volatile u8 FOTA_PORT[10];
//extern volatile u8 FOTA_SERVER[20];
//extern volatile u8 FOTA_username[20];
extern volatile u8 upgrade_filename[10];
extern bool update_flag;
extern bool update_stat;
extern u8 FOTA_Buffer[512];
extern u8 slave_id;
extern unsigned int TOT_DEV_ID;
extern unsigned char ON_PACK_FLAG;
extern unsigned int CREG_COUNT;
extern u8 cmd_buf[100];
extern unsigned int send_count;
extern u16 update_trial;
extern u8 SLASH_C_SUB;
extern u8 SLASH_T_SUB;
extern u8 MODBUS_TRANSMIT;
extern u8 serial_transfer;
extern u8 temper_state;
extern u8 temper_send_flag;
extern u8 temper_alert;

extern u8 bat_disc_alert;
extern unsigned int  NETWORK_RST;

extern u8 first_data_packet_send;

extern u8 SET_TOT_BUFF[10];
extern float SET_TOT,SET_TOT_1,SET_TOT_2;
extern float RSET_TOT,RSET_TOT_1,RSET_TOT_2;
extern u8 modbus_write;
extern int que;
extern u8 RS485_COMM_FAIL;
extern unsigned char hexVals[4];
extern unsigned char  RS485_QUERY;
extern unsigned char  RS485_QUERY_COUNT;
extern unsigned char  RS485_READ_QUERY_SEND;
extern unsigned char  PREV_TAB;
extern unsigned char  PUMP_ON_DETECT;

//extern u8 serial_transfer;
extern u8 APN_RUN_COUNTER;
extern u8 APN_TOT_COUNTER;
extern u16 APN_FILE_LENGTH;
extern u8 datalen_10;
extern u8 PRODUCT;
extern u8 SMS_CLEAR;

#endif /* DECLARATION_H_ */
