/*
 * declaration.c
 *
 *  Created on: Mar 23, 2019
 *      Author: SW
 */

#include "declaration.h"
#include "ql_type.h"

/*********************************** FILE SYSTEM ************************************************/
s32 filehandle;
/*************************************************************************************************/
bool timer_start=0;
u32 time_out_timer=5;
u32 timer_cnt=0;
s32 Status=0,iRet=0,File_Ret=0;
u32 /*rssi=0,*/ber=0,length=0,dataLen=0,len=0;
s32 rssi,gsm_signal;
s32 ret,pdpCntxtId;
s32 Primary_SocketId=0;

u32 cnt,comma_cnt;
u8 strmsg[170],password_str[10];

u8 response[350];
u8 strImei[30];
u8 volatile Imei_No[30];
u8 strImsi[30];
u8 volatile Imsi_No[30];
u8 store_imsi[30];
u8 strAT[50];
u8 sms_str[160];
u8 interval[10];
u8 NO_OF_HW[10];
u8 config_array[5];
u32 config_command;
u8 m_SocketRcvBuf[SOC_RECV_BUFFER_LEN];
u8 ip_addr[5];

u8 history_send_Data[300];
u8 operator_code[6][10];
u32 operator_len;
u8 PhNum[20];

u8 Admin_no[20];
u8 Number[20];


bool Select_Operator;
bool sms_flag;

u8 primary_server[50];
u8 primary_port[10];

u8 APN[35];
u32 port1;

bool msg_extract_done=0;
u32 manual_mode=0;
bool send_flag=0;
u32 watchdog_cnt=0;
bool device_reset=0;

bool sms_init_done=0,sms_read_error=0;

u8 DATE[15];
u8 TIME[15];
bool mqtt_rcv_flag=0,Timeout_Flag=0;
u32 dly_cnt=30;
u32 fmeter_length=0;
bool bat_disconnect_stat =1;
u32 error=0;
u32 publish_timer=0,fmeter_interval=0;
bool get_fmeter_data=0;
bool connection_est=0,ping_flag=0;
u8 modbus_str[200];
u32 gsm_error=0;
bool publish_urc=0,modbus_urc=0;
/********************************************** FLOWMETER VARIABLES ******************************************/
float flow,TOT1,TOT2,RTOT1,RTOT2;
u32 unit;
u32 version=11;

u8 fmeter_data[200];

/****************************************** FOTA **********************************************/
//volatile u8 FOTA_password[20];
//volatile u8 FOTA_path[20];
//volatile u8 FOTA_PORT[10];
//volatile u8 FOTA_SERVER[20];
//volatile u8 FOTA_username[20];
volatile u8 upgrade_filename[10];
bool update_flag=0;
bool update_stat=0;
u8 FOTA_Buffer[512];

u8 slave_id=10;
unsigned int TOT_DEV_ID = 2;
unsigned char ON_PACK_FLAG = 0;

unsigned int CREG_COUNT = 0;
unsigned int send_count=0;
u8 cmd_buf[100];
u16 update_trial=0;
u8 SLASH_C_SUB	=0;
u8 SLASH_T_SUB	=0;
u8 MODBUS_TRANSMIT =0;
u8 serial_transfer = 0;


u8 temper_state = 0;
u8 temper_send_flag = 0;
u8 temper_alert = 0;
u8 first_data_packet_send = 0;
u8 bat_disc_alert = 0;
unsigned int  NETWORK_RST = 0;

u8 SET_TOT_BUFF[10];
float SET_TOT = 0,SET_TOT_1 = 0,SET_TOT_2 =0;
float RSET_TOT = 0,RSET_TOT_1 = 0,RSET_TOT_2 =0;
u8 modbus_write = 0;
int que = 0;
u8 RS485_COMM_FAIL = 0;
unsigned char hexVals[4];
unsigned char  RS485_QUERY = 0;
unsigned char  RS485_QUERY_COUNT = 0;
unsigned char  RS485_READ_QUERY_SEND = 0;
unsigned char  PREV_TAB = 0;
unsigned char  PUMP_ON_DETECT = 0 ;


//u8 serial_transfer = 0;
u8 APN_RUN_COUNTER = 1;
u8 APN_TOT_COUNTER = 3;
u16 APN_FILE_LENGTH =0;
u8 datalen_10 = 0;
u8 PRODUCT = 0;		// 0 FOR EFM , 1 FOR PIZEO
u8 SMS_CLEAR = 0;
