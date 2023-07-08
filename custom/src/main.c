
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   main.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This app demonstrates how to send AT command with RIL API, and transparently
 *   transfer the response through MAIN UART. And how to use UART port.
 *   Developer can program the application based on this example.
 * 
 ****************************************************************************/
#ifdef __FLOWMETER_GSM__
#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_telephony.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_wtd.h"
#include "ql_system.h"
#include "ql_gpio.h"
#include "ql_timer.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_memory.h"
#include "ql_fs.h"
#include "ril_system.h"
#include "ril_sim.h"
#include "ril_sms.h"
#include "function.h"
#include "declaration.h"
#include "sms.h"
#include "main.h"
#include "mqtt.h"
#include "memory.h"
#include "MODBUS.h"
#include "ql_eint.h"

void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode,  u32 ipAddrCnt, u32* ipAddr);
void Callback_GPRS_Actived(u8 contexId, s32 errCode, void* customParam);
void CallBack_GPRS_Deactived(u8 contextId, s32 errCode, void* customParam );


ST_PDPContxt_Callback     callback_gprs_func =
{
    Callback_GPRS_Actived,
    CallBack_GPRS_Deactived
};


static ST_GprsConfig m_GprsConfig;// = {
    //"internet",    // APN name
   // "",         // User name for APN
   // "",         // Password for APN
   // 0,
   // NULL,
   // NULL,
//};


#define SERIAL_RX_BUFFER_LEN  2048

static u8 primary_ipaddress[50];


static void Callback_Socket_Connect(s32 socketId, s32 errCode, void* customParam );
static void Callback_Socket_Close(s32 socketId, s32 errCode, void* customParam );
static void Callback_Socket_Accept(s32 socketId, s32 errCode, void* customParam );
static void Callback_Socket_Read(s32 socketId, s32 errCode, void* customParam );

static void Callback_Socket_Write(s32 socketId, s32 errCode, void* customParam );
ST_SOC_Callback callback_soc_func = {
		Callback_Socket_Connect, //NULL,
        Callback_Socket_Close,
        Callback_Socket_Accept,// NULL,
        Callback_Socket_Read,
        Callback_Socket_Write
    };


static ST_GprsConfig  m_gprsCfg;

// Define the UART port and the receive data buffer
static Enum_SerialPort MAIN_UART  = UART_PORT1;
static u8 m_RxBuf_Uart1[SERIAL_RX_BUFFER_LEN];
 u8 m_RxBuf_Uart_RS[2048];

 //static Enum_PinName eintPinName = PINNAME_DTR;
 //void callback_eint_handle(Enum_PinName eintPinName, Enum_PinLevel pinLevel, void* customParam);


static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);
void setState(Enum_URCType);
//static void CallBack_UART2_Hdlr(Enum_SerialPort, Enum_UARTEventType,bool, void*);
static u32 socket_error;




void proc_main_task(s32 taskId)
{
    s32 ret;
    ST_MSG msg;

    // Register & open UART port
    Ql_UART_Register(MAIN_UART, CallBack_UART_Hdlr, NULL);
    Ql_UART_Open(MAIN_UART, 115200, FC_NONE);



    Ql_UART_Register(UART_PORT2, CallBack_UART_Hdlr, NULL);
    Ql_UART_Open(UART_PORT2, 9600, FC_NONE);

    Ql_GPIO_Init(PINNAME_PCM_OUT,PINDIRECTION_OUT,PINLEVEL_HIGH,PINPULLSEL_DISABLE);
    Ql_GPIO_Init(POWER_PIN, PINDIRECTION_IN, PINLEVEL_LOW,PINPULLSEL_PULLDOWN);
    Ql_GPIO_Init(TEMPER_SWITCH_INT, PINDIRECTION_IN, PINLEVEL_LOW,PINPULLSEL_DISABLE);

   // Ql_EINT_Register(TEMPER_SWITCH_INT,callback_eint_handle, NULL);
   // Ql_EINT_RegisterFast(TEMPER_SWITCH_INT, callback_eint_handle,NULL);
   // Ql_EINT_Init(TEMPER_SWITCH_INT, EINT_LEVEL_TRIGGERED, 0, 10,0);





    // START MESSAGE LOOP OF THIS TASK
    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
			case MSG_ID_RIL_READY:

				Ql_RIL_Initialize();

				RTOT1 = 0;
				RTOT2 = 0;


				Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);

				if(Ql_GPIO_GetLevel(PINNAME_PCM_OUT))
				{
					//APP_DEBUG("PINNAME_PCM_OUT : 1\r\n");
				}
				else
				{
					//APP_DEBUG("PINNAME_PCM_OUT : 0\r\n");
				}
				setState(URC_MEMORY_INIT);
				break;

			case MSG_ID_URC_INDICATION:
				//APP_DEBUG("<-- Received URC: type: %d, -->\r\n", msg.param1);	//remove
				switch (msg.param1)
				{
					case URC_MEMORY_INIT:
					{
						READ_MEMORY();
						APP_DEBUG("PRODUCT : %d\r\n",PRODUCT);
						setState(URC_SIM_CARD_STATE_IND);

						//setState(URC_MODBUS_DATA);
						break;
					}
					case URC_SIM_CARD_STATE_IND:
					{

						unsigned char sim_count = 0;

						time_delay(1000);
						Status=0;


						APP_DEBUG("Version : %d\n",version);
						SYNC_UPDATE();


						LABLE:


						RIL_SIM_GetSimState(&Status);

						RS485_QUERY 		= 	ON;
						RS485_QUERY_COUNT 	= 	RESET;

						if(Status == SIM_STAT_READY)
						{
							setState(URC_SMS_STATE_IND);
							break;
						}
						else
						{
							APP_DEBUG("SIM abnormal...\r\n");
							sim_count++;
							time_delay(1000);

							if(sim_count<5)
							{
								goto LABLE;
							}
							else
							{
								Ql_Reset(0);
							}


						}

						break;
					}

					case URC_SMS_STATE_IND:
					{
						Ql_memset(response,0x00,sizeof(response));
						Ql_memset(strAT,0x00,sizeof(strAT));

						iRet = SMS_Initialize();
						if (!iRet)
						{
							APP_DEBUG("Fail to initialize SMS\r\n");
						}

						else
						{
							sms_init_done=1;

						}


						//time_delay(2000);


						if(update_stat == 1)
						{
							u32 nMsgRef;
							u8 PhNum[20];

							update_stat=0;
							Update_File(1);
							Update_Parameter_File(0);
							Ql_sprintf(PhNum,"7046960991\0");

							Ql_memset(sms_str,0x00,sizeof(sms_str));
							Ql_sprintf(sms_str,"UPDATED:%cD%s",upgrade_filename[0],&upgrade_filename[1]);
							APP_DEBUG("%s\r\n",sms_str);	//remove

							iRet = RIL_SMS_SendSMS_Text(PhNum, Ql_strlen(PhNum), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), &nMsgRef);
							if (iRet != RIL_AT_SUCCESS)
							{
								APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
								//return;
							}
							APP_DEBUG("Send SMS successfully\r\n");



						}


						setState(URC_SIGNAL_QUAL_IND);
						break;
					}

					case URC_NEW_SMS_IND:

						APP_DEBUG(" New SMS =%d\r\n", msg.param2);

						 u32 nMsgRef;

						 Ql_memset(response,0x00,sizeof(response));
						 Ql_memset(strmsg,0x00,sizeof(strmsg));


						 if(SMS_CLEAR == 0)
							{
								SMS_CLEAR = 1;
								SMS_CLEAR_File(0);
							}
							else
							{
								SMS_Delete();
								SMS_CLEAR_File(0);
							}




						 SMS_TextMode_Read(msg.param2);

						 APP_DEBUG("New SMS :%s\r\n",strmsg);	//add by christy
						 SMS_READ();

						 if(sms_read_error == 0)
						 SMS_Delete();

						if(sms_flag == 1)
						{
							SEND_SMS(config_command,0);
							sms_flag=0;
							config_command=0;

							SMS_File(0);
							Config_File(0);

						}

						if(update_flag == 1)
						{
							u8 PhNum[20];
							Update_File(1);
							Ql_sprintf(PhNum,"07046960991\0");

							Ql_memset(sms_str,0x00,sizeof(sms_str));
							Ql_sprintf(sms_str,"UPDATING:%cD%s",upgrade_filename[0],&upgrade_filename[1]);

							APP_DEBUG("message to send:%s\r\n",sms_str);	//remove


							iRet = RIL_SMS_SendSMS_Text(PhNum, Ql_strlen(PhNum), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), &nMsgRef);
							if (iRet != RIL_AT_SUCCESS)
							{
								APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
							}
							APP_DEBUG("< Send Text SMS successfully, MsgRef:%u >\r\n", nMsgRef);

							//Ql_sprintf(FOTA_Buffer,"http://nanosoft.org.in/hex/D");
							Ql_sprintf(FOTA_Buffer,"http://skyrise.online/hex/D");
							Ql_snprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],4+1,&upgrade_filename[1]);
							Ql_sprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],".bin");
							APP_DEBUG("fota buffer:%s\r\n",FOTA_Buffer);

							time_delay(1000);//Ql_Sleep(5000);
							ret=Ql_FOTA_StartUpgrade(FOTA_Buffer, NULL, NULL);

							if(ret != QL_RET_OK)
							{

								update_stat=0;
								update_flag=0;
								Update_Parameter_File(0);
							}

						}

						TOT_DATA_SORT();

/*
						if(modbus_write == 1)
						{
							//SET_TOT = 196805;

							que = SET_TOT/65535;
							SET_TOT_2 =(float)que;
							SET_TOT_1 =  (que * 65535);
							SET_TOT_1 = SET_TOT - SET_TOT_1;


							APP_DEBUG("SET_TOT 		: %f\n",SET_TOT);
							APP_DEBUG("SET_TOT_1 	: %f\n",SET_TOT_1);
							APP_DEBUG("SET_TOT_2 	: %f\n",SET_TOT_2);

						//	if(connection_est == 1)
							{
								setState(URC_MODBUS_DATA);
							}


						}
*/


						if(device_reset == 1)
						{
							time_delay(500);
							Ql_Reset(0);
						}
						// new_sms_ind=1;
						// Ql_OS_SendMessage(main_task_id, MSG_ID_URC_INDICATION, URC_READ_SMS_IND, msg.param2);
						break;


					case URC_UPDATE_IND :

						if(update_flag == 1)
						{
							u8 PhNum[20];
							Update_File(1);
							Ql_sprintf(PhNum,"07046960991\0");

							Ql_memset(sms_str,0x00,sizeof(sms_str));
							Ql_sprintf(sms_str,"UPDATING:%cD%s",upgrade_filename[0],&upgrade_filename[1]);

							APP_DEBUG("message to send:%s\r\n",sms_str);	//remove


							iRet = RIL_SMS_SendSMS_Text(PhNum, Ql_strlen(PhNum), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), &nMsgRef);
							if (iRet != RIL_AT_SUCCESS)
							{
								APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
							}
							APP_DEBUG("< Send Text SMS successfully, MsgRef:%u >\r\n", nMsgRef);

							//Ql_sprintf(FOTA_Buffer,"http://nanosoft.org.in/hex/D");
							Ql_sprintf(FOTA_Buffer,"http://skyrise.online/hex/D");
							Ql_snprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],4+1,&upgrade_filename[1]);
							Ql_sprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],".bin");
							APP_DEBUG("fota buffer:%s\r\n",FOTA_Buffer);

							time_delay(1000);//Ql_Sleep(5000);
							ret=Ql_FOTA_StartUpgrade(FOTA_Buffer, NULL, NULL);

							if(ret != QL_RET_OK)
							{

								update_stat=0;
								update_flag=0;
								Update_Parameter_File(0);
							}

						}

						break;


					case URC_SIGNAL_QUAL_IND:
						/*s32 nRet = */RIL_NW_GetSignalQuality(&gsm_signal, &ber);
						APP_DEBUG("Signal:%d\r\n", gsm_signal);
						if(gsm_signal==99  || gsm_signal<1)
						{
							if(error == 0)
							{
								error=2;
								ERR_File(0);
							}
						}
						SYNC_UPDATE();
						setState(URC_IMEI_STATE_IND);
						break;

					case URC_IMEI_STATE_IND:
						{
							Ql_memset(strImei, 0x0, sizeof(strImei));
							Ql_memset(Imei_No, 0x0, sizeof(Imei_No));
							iRet = RIL_GetIMEI(strImei);
							Ql_memcpy(&Imei_No[0],&strImei[2],15);
							APP_DEBUG("IMEI : %s\r\n", Imei_No);

							setState(URC_IMSI_STATE_IND);
							break;
						}

					case URC_IMSI_STATE_IND:
						{
							u8 PhNum[20];
							u32 nMsgRef;
							Ql_memset(strImsi, 0x0, sizeof(strImsi));
							Ql_memset(Imsi_No, 0x0, sizeof(Imsi_No));
							iRet = RIL_SIM_GetIMSI(strImsi);
							Ql_memcpy(&Imsi_No[0],&strImsi[2],15);

							APP_DEBUG("IMSI : %s\r\n", Imsi_No);
							IMSI_File(1);
							if((!Ql_strstr(store_imsi,Imsi_No)) || store_imsi[0]==0x00)
							{

								Ql_memset(sms_str,0x00,sizeof(sms_str));


								if(PRODUCT == 0)
									Ql_sprintf(sms_str,"EFM New sim detected. IMEI:%s IMSI:%s",Imei_No,Imsi_No);
								else if(PRODUCT == 1)
									Ql_sprintf(sms_str,"PIZEO New sim detected. IMEI:%s IMSI:%s",Imei_No,Imsi_No);


								//APP_DEBUG("%s\r\n",sms_str);	//remove


								//Ql_sprintf(PhNum,"7046960991\0");
								Ql_sprintf(PhNum,"09998823997\0");
								iRet = RIL_SMS_SendSMS_Text(PhNum, Ql_strlen(PhNum), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), &nMsgRef);
								if (iRet != RIL_AT_SUCCESS)
								{
									APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
								}
								else
								{
									IMSI_File(0);
									APP_DEBUG("Send Text SMS successfully\r\n");
								}
							}


							//if(sms_read_error == 0)
															//SMS_Delete();

							if(SMS_CLEAR == 0)
							{
								SMS_CLEAR = 1;
								SMS_CLEAR_File(0);
							}
							else
							{
								//if(sms_read_error == 0)
									SMS_Delete();
									SMS_CLEAR_File(0);
							}

							Ql_OS_SendMessage(main_task_id, MSG_ID_URC_INDICATION, URC_READ_SMS_IND, 1);
							break;
						}

					case URC_READ_SMS_IND:
						{


							u32 nMsgRef;
							Ql_memset(response,0x00,sizeof(response));
							Ql_memset(strmsg,0x00,sizeof(strmsg));

							//if(sms_read_error == 0)
								//SMS_Delete();

							SMS_TextMode_Read(msg.param2);
							APP_DEBUG("INCOMING_SMS:%s\r\n",strmsg);	//add by christy
							SMS_READ();



							if(sms_read_error == 0)
							SMS_Delete();

							if(sms_flag == 1)
							{

								SEND_SMS(config_command,0);
								sms_flag=0;
								config_command=0;

								SMS_File(0);
								Config_File(0);

							}


							if(update_flag == 1)
							{
								u8 PhNum[20];

								Update_File(1);

								Ql_sprintf(PhNum,"7046960991\0");

								Ql_memset(sms_str,0x00,sizeof(sms_str));
								Ql_sprintf(sms_str,"UPDATING:%cD%s",upgrade_filename[0],&upgrade_filename[1]);

								APP_DEBUG("message to send:%s\r\n",sms_str);	//remove


								iRet = RIL_SMS_SendSMS_Text(PhNum, Ql_strlen(PhNum), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), &nMsgRef);
								if (iRet != RIL_AT_SUCCESS)
								{
									APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
								}
								APP_DEBUG("< Send Text SMS successfully, MsgRef:%u >\r\n", nMsgRef);

								Ql_sprintf(FOTA_Buffer,"http://skyrise.online/hex/D");
								//Ql_sprintf(FOTA_Buffer,"http://nanosoft.org.in/hex/D");
								Ql_snprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],4+1,&upgrade_filename[1]);
								Ql_sprintf(&FOTA_Buffer[Ql_strlen(FOTA_Buffer)],".bin");
								APP_DEBUG("fota buffer:%s\r\n",FOTA_Buffer);

								time_delay(1000);//Ql_Sleep(5000);
								ret=Ql_FOTA_StartUpgrade(FOTA_Buffer, NULL, NULL);

								if(ret != QL_RET_OK)
								{

									update_stat=0;
									update_flag=0;
									//Update_File(0);
									Update_Parameter_File(0);
								}

							}

							TOT_DATA_SORT();


/*
							if(modbus_write == 1)
							{
								//SET_TOT = 196805;

								que = SET_TOT/65535;
								SET_TOT_2 =(float)que;
								SET_TOT_1 =  (que * 65535);
								SET_TOT_1 = SET_TOT - SET_TOT_1;


								APP_DEBUG("SET_TOT 		: %f\n",SET_TOT);
								APP_DEBUG("SET_TOT_1 	: %f\n",SET_TOT_1);
								APP_DEBUG("SET_TOT_2 	: %f\n",SET_TOT_2);


								que = RSET_TOT/65535;
								RSET_TOT_2 =(float)que;
								RSET_TOT_1 =  (que * 65535);
								RSET_TOT_1 = RSET_TOT - RSET_TOT_1;

								APP_DEBUG("RSET_TOT 	: %f\n",RSET_TOT);
								APP_DEBUG("RSET_TOT_1 	: %f\n",RSET_TOT_1);
								APP_DEBUG("RSET_TOT_2 	: %f\n",RSET_TOT_2);



								//if(connection_est == 1)
								{
									setState(URC_MODBUS_DATA);
								}


							}
*/
							if(device_reset == 1)
							{
								time_delay(500);
								Ql_Reset(0);
							}

							if(update_flag == 0)
							setState(URC_GSM_NW_STATE_IND);
							break;

						}

					case URC_GSM_NW_STATE_IND:
						CHECK_CREG:

						RIL_NW_GetGSMState(&Status);
						APP_DEBUG("NETWORK STATUS:%d\r\n", Status);

					 if(Status != 1 && Status != 5)
						 {
							 if(CREG_COUNT<20)
							 {
								 time_delay(400);
								 CREG_COUNT++;
								 goto CHECK_CREG;

							 }

							 if(error == 0)
							 {
								 error=1;
								 ERR_File(0);
							 }
							 Select_Operator=1;
							 manual_mode++;
							 Manual_Mode_File(0);
						 }

						 else
						 {
							 Select_Operator=0;
						 }

						 //RIL_CURRENT_OPERATOR();
						 if(manual_mode >= 1)
						 {
							 manual_mode=0;
							 Manual_Mode_File(0);
							 CREG_COUNT =0;
							 setState(URC_GET_OPERATOR_IND);

							 break;

						 }
						 else
						 {
							 setState(URC_GPRS_NW_STATE_IND);
							 break;
						 }
						break;

					case URC_GPRS_NW_STATE_IND:

						CREG_COUNT = 0;
						CHECK_CREG_1:
						RIL_NW_GetGPRSState(&Status);
						APP_DEBUG("GPRS STATUS:%d\r\n", Status);

						if(Status != 1 && Status != 5)
						 {
							 if(CREG_COUNT<20)
							 {
								 time_delay(400);
								 CREG_COUNT++;
								 goto CHECK_CREG_1;
							 }

							 if(error == 0)
							 {
								 error=3;
								 ERR_File(0);
							 }
							 if(Select_Operator == 0)
							 {
								 Select_Operator=1;
								 manual_mode++;
								 Manual_Mode_File(0);
							 }
						 }

						 else
						 {
							 Select_Operator=0;
						 }

						 if(manual_mode >= 1)
						 {
							 manual_mode=0;
							 Manual_Mode_File(0);
							 RS485_QUERY = OFF;
							 setState(URC_GET_OPERATOR_IND);
							 break;

						 }
						 else
						 {
							 //setState(URC_GPRS_NW_STATE_IND);
							 setState(URC_GET_PDP_IND);
							 break;
						 }

						// setState(URC_GET_PDP_IND);
						// break;

					case URC_GET_OPERATOR_IND:

						watchdog_cnt = 1;
						RIL_LIST_OPERATOR();
						operator_len=0;
						setState(URC_OPERATOR_SELECT_IND);
						break;

					case URC_OPERATOR_SELECT_IND:
						iRet=-1;
						Ql_memset(response,0x00,sizeof(response));
						Ql_memset(strAT,0x00,sizeof(strAT));

						if(operator_code[operator_len][0] != 0x00)
						{
							APP_DEBUG("operator len : %d\r\n",operator_len);
							Ql_sprintf(strAT,"AT+COPS=4,2,\"%s\"",operator_code[operator_len]);
							APP_DEBUG("%s\r\n",strAT);
							iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATRsp_COPS_Handler,(void*)response,0);
							watchdog_cnt=30;
							if(iRet == 0)
							{
								RIL_NW_GetGSMState(&Status);
								APP_DEBUG("GSM Network:%d\r\n", Status);
								if(Status!=1 && Status!=5)
								{
									if(operator_len > 3)
									{
										//APP_DEBUG("opertor list complete\r\n");
										setState(URC_GET_PDP_IND);
										break;
									}
									operator_len++;
									setState(URC_OPERATOR_SELECT_IND);
									break;
								}

								else
								{
									//RIL_CURRENT_OPERATOR();
									setState(URC_IMEI_STATE_IND);
									//setState(URC_GET_PDP_IND);
									break;
								}
							}
							else
							{
								operator_len++;
								setState(URC_OPERATOR_SELECT_IND);
								break;
							}
						}

						else
						{
							setState(URC_GET_PDP_IND);
							break;
						}
						break;

					case URC_GET_PDP_IND:
						pdpCntxtId = Ql_GPRS_GetPDPContextId();
						if (GPRS_PDP_ERROR == pdpCntxtId)
						{
							APP_DEBUG("No PDP available\r\n");
						}
						//APP_DEBUG("pdpCntxtId: %d\r\n",pdpCntxtId);
						setState(URC_REG_PDP_IND);
						break;

					case URC_REG_PDP_IND:
						ret = Ql_GPRS_Register(pdpCntxtId, &callback_gprs_func, NULL);
						if (GPRS_PDP_SUCCESS == ret)
						{
							//APP_DEBUG("<-- Register GPRS callback function -->\r\n");

						}
						else
						{
							APP_DEBUG("Fail to register GPRS, cause=%d\r\n", ret);

						}

						setState(URC_CONFIG_PDP_IND);
						break;

					case URC_CONFIG_PDP_IND:
						Ql_memset(&m_GprsConfig,0x00, sizeof(m_GprsConfig));

						Ql_strcpy(m_GprsConfig.apnName,APN);
						Ql_sprintf(m_GprsConfig.apnUserId,"");
						Ql_sprintf(m_GprsConfig.apnPasswd,"");

						ret = Ql_GPRS_Config(pdpCntxtId, &m_GprsConfig);
						if (GPRS_PDP_SUCCESS == ret)
						{
							//APP_DEBUG("<-- Configure PDP context -->\r\n");

						}
						else
						{
							 if(error == 0)
							 {
								 error=4;
								 ERR_File(0);
							 }
							APP_DEBUG("Fail to configure GPRS PDP, cause=%d\r\n", ret);
							APN_CHANGE();
						}
						setState(URC_ACTIVATE_GPRS);
						break;

					case URC_ACTIVATE_GPRS:
						APP_DEBUG("Activating GPRS\r\n");
						ret = Ql_GPRS_ActivateEx(pdpCntxtId, TRUE);
						if(ret == GPRS_PDP_SUCCESS)
						{
							 RS485_QUERY = ON;
							APP_DEBUG("Activated GPRS\r\n");
							//setState(URC_GET_LOCAL_IP);
							//break;
						}
						else
						{
							 if(error == 0)
							 {
								 error=5;
								 ERR_File(0);
							 }
							APP_DEBUG("Fail GPRS =%d\r\n", ret);
							APN_CHANGE();
						}
						setState(URC_GET_LOCAL_IP);
						break;

					case URC_GET_LOCAL_IP:

						Ql_memset(ip_addr, 0, sizeof(ip_addr));
						ret = Ql_GPRS_GetLocalIPAddress(0, (u32 *)ip_addr);

						if (ret == GPRS_PDP_SUCCESS)
						{
							APP_DEBUG("Local Ip=%d.%d.%d.%d\r\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);

						}
						else
						{
							APP_DEBUG("Local Ip failure,ret=%d\r\n",ret);
						}

						setState(URC_SOCKET_REGISTER);
						break;

					case URC_SOCKET_REGISTER:
						ret = Ql_SOC_Register(callback_soc_func, NULL);
						if (SOC_SUCCESS == ret)
						{
							APP_DEBUG("socket Register\r\n");

						}
						else
						{
							APP_DEBUG("Fail to register socket callback, cause=%d\r\n", ret);

						}
						socket_error=0;
						setState(URC_SOCKET_CREATE);
						break;

					case URC_SOCKET_CREATE:
						Primary_SocketId = Ql_SOC_CreateEx(pdpCntxtId,SOC_TYPE_TCP, Ql_OS_GetActiveTaskId(), callback_soc_func);
						if (Primary_SocketId >= 0)
						{
							APP_DEBUG("Primary_SocketId=%d\r\n", Primary_SocketId);
							setState(URC_CONVERT_IP_ADDR);
						}
						else
						{
							APP_DEBUG("Fail to create primary socket, cause=%d\r\n", Primary_SocketId);

							socket_error++;
							if(socket_error > 1)
							{
								socket_error=0;
								if(error == 0)
								{
									error=13;
									ERR_File(0);
								}
								time_delay(500);
								Ql_Reset(0);
							}
							setState(URC_SOCKET_CREATE);
						}

						break;

					case URC_CONVERT_IP_ADDR:
						ret=-1;
						if(primary_server[0] >= 0x30 && primary_server[0] <= 0x39)
						{
							u32 host_ip=0;
							ret = Ql_IpHelper_ConvertIpAddr(primary_server, &host_ip);

							if(SOC_SUCCESS == ret)
							{
								Ql_memcpy(primary_ipaddress,&host_ip, sizeof(host_ip));
								APP_DEBUG("primary_ipaddress=%d,%d,%d,%d\r\n",primary_ipaddress[0],primary_ipaddress[1],primary_ipaddress[2],primary_ipaddress[3]);
								setState(URC_CONNECT_SERVER);
							}

							else
							{
								APP_DEBUG("Fail to convert IP Address\r\n");
								socket_error++;

								if(socket_error > 1)
								{
									socket_error=0;
									if(error == 0)
									{
										error=14;
										ERR_File(0);
									}
									time_delay(500);
									Ql_Reset(0);
								}
							}
						}

						else	// host name is domain name
						{

							if(socket_error > 2)
							{
								socket_error=0;
								if(error == 0)
								{
									error=14;
									ERR_File(0);
								}
								time_delay(500);
								Ql_Reset(0);
							}

							APP_DEBUG("Ql_IpHelper_GetIPByHostName");
							ret = Ql_IpHelper_GetIPByHostName(pdpCntxtId, 0, primary_server, Callback_GetIpByName);
						}
						break;

					case URC_CONNECT_SERVER:
						ret=-1;
						port1=Ql_atoi(primary_port);
						APP_DEBUG(" Connecting to primary server, socket_id = %d,(IP:%d.%d.%d.%d, port:%d)\r\n", Primary_SocketId,primary_ipaddress[0],primary_ipaddress[1],primary_ipaddress[2],primary_ipaddress[3],port1);
						ret = Ql_SOC_ConnectEx(Primary_SocketId,(u32) primary_ipaddress, port1, TRUE);


						if(SOC_SUCCESS == ret)
						{
							APP_DEBUG("Connect to primary server successfully\r\n");
							if(manual_mode != 0)
							{
								manual_mode=0;
								Manual_Mode_File(0);
							}
							setState(URC_CONNECT_PACKET);
							break;

						}

						else
						{
							APP_DEBUG("Fail to connect to primary server, cause=%d\r\n", ret);
							Ql_SOC_Close(Primary_SocketId);
							Primary_SocketId=-1;
							socket_error++;
							if(socket_error > 2)
							{
								socket_error=0;
								if(error == 0)
								{
									error=8;
									ERR_File(0);
								}

								time_delay(500);
								Ql_Reset(0);
							}

							else
							{
								setState(URC_SOCKET_CREATE);
								break;
							}


						}
						break;


					case URC_CONNECT_PACKET:
						ret=MQTTConnect(Imei_No,"0,OF","pub_usr_kptt","652|]og%4z34)@r",180);
						APP_DEBUG("MQTTConnect = ret : %d\r\n",ret);
						if(ret == SOC_SUCCESS)
						{
							if(error != 0)
							{
								error=0;
								ERR_File(0);
							}
							setState(URC_SUBSCRIBE_PACKET_C);
							break;
						}

						else
						{
							if(MQTT_CONNECT_FAIL==1)
							{
								 if(error == 0)
								 {
									 error=8;
									 ERR_File(0);
								 }
								CLR_MQTT_FLAG;
								setState(URC_CONNECT_SERVER);
								break;
							}
							else
							{
								time_delay(500);
								setState(URC_CONNECT_PACKET);
								break;
							}
						}


					case URC_SUBSCRIBE_PACKET_C :

						APP_DEBUG("URC_SUBSCRIBE_PACKET_C\r\n");
						SLASH_C_SUB = 1;
						ret=MQTTSubscribe(Imei_No,1);
						//APP_DEBUG("MQTTSubscribe = ret : %d\r\n",ret);
					//	SLASH_C_SUB= 0;

						if(ret == SOC_SUCCESS)
						{
							//connection_est=1;
							//ON_PACK_FLAG = 1;
							publish_timer = 0;
							setState(URC_SUBSCRIBE_PACKET_T);
							break;
						}

						else
						{
							if(MQTT_SUB_FAIL==1)
							{
								 if(error == 0)
								 {
									 error=9;
									 ERR_File(0);
								 }
								send_flag=0;
								connection_est=0;
								CLR_MQTT_FLAG;
								setState(URC_CONNECT_SERVER);
								break;
							}
							else
							{
								time_delay(500);
								setState(URC_SUBSCRIBE_PACKET_C);
								break;
							}
						}



						break;


					case URC_SUBSCRIBE_PACKET_T:
						APP_DEBUG("URC_SUBSCRIBE_PACKET_T\r\n");
						SLASH_T_SUB = 1;
						ret=MQTTSubscribe(Imei_No,1);
						//APP_DEBUG("MQTTSubscribe = ret : %d\r\n",ret);
						SLASH_T_SUB= 0;

						if(ret == SOC_SUCCESS)
						{
							//connection_est=1;
							//ON_PACK_FLAG = 1;
							publish_timer = 0;
							setState(URC_SUBSCRIBE_PACKET);
							break;
						}

						else
						{
							if(MQTT_SUB_FAIL==1)
							{
								 if(error == 0)
								 {
									 error=9;
									 ERR_File(0);
								 }
								send_flag=0;
								connection_est=0;
								CLR_MQTT_FLAG;
								setState(URC_CONNECT_SERVER);
								break;
							}
							else
							{
								time_delay(500);
								setState(URC_SUBSCRIBE_PACKET_C);
								break;
							}
						}

					case URC_SUBSCRIBE_PACKET:
						ret=MQTTSubscribe(Imei_No,1);
						APP_DEBUG("URC_SUBSCRIBE_PACKET MQTTSubscribe = ret : %d\r\n",ret);
						if(ret == SOC_SUCCESS)
						{
							//connection_est=1;
							NETWORK_RST =0;
							ON_PACK_FLAG = 1;
							//publish_timer = fmeter_interval;
							send_flag = 1;
							setState(URC_ON_PACKET);
							break;
						}

						else
						{
							if(MQTT_SUB_FAIL==1)
							{
								 if(error == 0)
								 {
									 error=9;
									 ERR_File(0);
								 }
								send_flag=0;
								connection_est=0;
								CLR_MQTT_FLAG;
								setState(URC_CONNECT_SERVER);
								break;
							}
							else
							{
								time_delay(500);
								setState(URC_SUBSCRIBE_PACKET);
								break;
							}
						}



						/*----------------------*/


					case URC_ON_PACKET:
						//if(publish_urc==1)
						{
							//GET_MODBUS_DATA();
							//publish_urc=0;
							APP_DEBUG("URC_ON_PACKET\r\n");

							if(send_flag == 1)
							{
								send_flag=0;
								ret=MQTTPublish(0,Imei_No,1);
								ON_PACK_FLAG=0;
							//	APP_DEBUG("MQTTPublish ret : %d\r\n",ret);
								if(ret == SOC_SUCCESS)
								{
									modbus_urc = 0;
									publish_urc=0;
									send_flag=0;
									connection_est=1;
									NETWORK_RST =0;
									ON_PACK_FLAG = 0;

								//	setState(URC_MODBUS_DATA);

									publish_timer = (fmeter_interval);
									break;
								}
								else
								{
									if(MQTT_PUBLISH_FAIL == 1)
									{
										if(error == 0)
										{
											error=10;
											ERR_File(0);
										}
										send_flag=0;
										connection_est=0;
										publish_urc=0;

										get_fmeter_data=1;
										GET_MODBUS_DATA();
										CLR_MQTT_FLAG;
										setState(URC_CONNECT_SERVER);
										break;
									}
									else
									{
										time_delay(500);
										setState(URC_PUBLISH_PACKET);
										break;
									}
								}
							}

							else
							{

								break;
							}
						}



								/*------------*/
					case URC_MODBUS_DATA:

						if(modbus_write == 1)
						{

							SLEAVE_SELECTION();
							MODBUS_TOT_DATA_WRITE(slave_id,2,2);
							modbus_write = 0;
							time_delay(1000);
							MODBUS_RTOT_DATA_WRITE(slave_id,10,2);
							time_delay(1000);

						}



						if(modbus_urc == 1)
						{
							u8 transmit[10];

								//MODBUS_DATA(slave_id,0,10);
								SLEAVE_SELECTION();
								serial_transfer = 1;
								GET_MODBUS_DATA();
							//	SLEAVE_SELECTION();



					//		APP_DEBUG("connection_est : %d\r\n",connection_est);
					//		APP_DEBUG("publish_urc : %d\r\n",publish_urc);
					//		APP_DEBUG("send_flag : %d\r\n",send_flag);
/*
							if(connection_est == 1 && publish_urc==0 && send_flag==1)
							{
								publish_urc=1;
								modbus_urc=0;
								setState(URC_PUBLISH_PACKET);
								break;
							}
*/
							modbus_urc=0;
							break;
						}
						break;

					case URC_PUBLISH_PACKET:

						ON_PACK_FLAG =0;

						if(temper_send_flag == 1)
						{
							RIL_NW_GetSignalQuality(&gsm_signal, &ber);
							APP_DEBUG("Signal:%d\r\n", gsm_signal);
							DATE_TIME();
							if(send_flag == 1)
							{
								//send_flag=0;
								ret=MQTTPublish(0,Imei_No,1);
								APP_DEBUG("MQTTPublish ret : %d\r\n",ret);
								if(ret == SOC_SUCCESS)
								{
									publish_timer=0;
									publish_urc=0;
									send_flag=0;
									connection_est=1;
									MODBUS_TRANSMIT= 0;
									temper_send_flag = 0;

								}
								else
								{
									if(MQTT_PUBLISH_FAIL == 1)
									{
										if(error == 0)
										{
											error=10;
											ERR_File(0);
										}
										send_flag=0;
										connection_est=0;
										publish_urc=0;

										get_fmeter_data=1;
										GET_MODBUS_DATA();
										CLR_MQTT_FLAG;
										setState(URC_CONNECT_SERVER);
										break;
									}
									else
									{
										time_delay(500);
										setState(URC_PUBLISH_PACKET);
										break;
									}
								}

							}

						}





						if(publish_urc==1)
						{
							//GET_MODBUS_DATA();
							//publish_urc=0;
							u8 hardware_id_str[6];
							APP_DEBUG("PRODUCT : %d\r\n",PRODUCT);

							if(PRODUCT == 0)
							{
								Ql_sprintf(hardware_id_str,"%d",hardware_id);
							}
							else if(PRODUCT == 1)
							{
								Ql_sprintf(hardware_id_str,"P%d",hardware_id);
							}




							APP_DEBUG("send_flag : %d\r\n",send_flag);
							RIL_NW_GetSignalQuality(&gsm_signal, &ber);
							APP_DEBUG("Signal:%d\r\n", gsm_signal);
							DATE_TIME();
							if(send_flag == 1)
							{
								//send_flag=0;
								ret=MQTTPublish(0,hardware_id_str,1);
								APP_DEBUG("MQTTPublish ret : %d\r\n",ret);

								if(ret == SOC_SUCCESS)
								{
									publish_timer=0;
									publish_urc=0;
									send_flag=0;
									connection_est=1;
									first_data_packet_send = 1;
									NETWORK_RST =0;
									MODBUS_TRANSMIT= 0;
									u32 addr1,addr2;
									addr1=Read_SAddr_File();
									addr2=Read_EAddr_File();

									if(addr1 > addr2)
									setState(URC_HISTORY_PACKET);

									break;
								}
								else
								{
									if(MQTT_PUBLISH_FAIL == 1)
									{
										if(error == 0)
										{
											error=10;
											ERR_File(0);
										}
										send_flag=0;
										connection_est=0;
										publish_urc=0;

										get_fmeter_data=1;
										GET_MODBUS_DATA();
										CLR_MQTT_FLAG;
										setState(URC_CONNECT_SERVER);
										break;
									}
									else
									{
										time_delay(500);
										setState(URC_PUBLISH_PACKET);
										break;
									}
								}
							}
							else
							{

								break;
							}
						}

						break;

					case URC_HISTORY_PACKET:
					{
						u32 addr1,addr2;
						addr1=Read_SAddr_File();
						addr2=Read_EAddr_File();

						if(addr2>addr1)
						{
							addr2=addr1;
							Write_EAddr_File(addr2);

						}

						if(addr1 > addr2)
						{
							publish_history_data(1);
							time_delay(500);
							setState(URC_HISTORY_PACKET);

							break;
						}

						else
						{
							break;
						}
					}

					case URC_RS_485_PACKET_SEND:


						if(MODBUS_TRANSMIT == 1)
						{
							 if(send_count>1)
							{
								modbus_urc=0;
								send_count = 0;
								MODBUS_TRANSMIT = 0;
								datalen_10 = 0;
							}

							else if(send_count>0)
							{
								//modbus_urc=0;
								//send_count = 0;
								//MODBUS_TRANSMIT = 0;
								datalen_10 = 1;
								GET_MODBUS_DATA();
								send_count++;

							}
							else
							{

								//time_delay(200);
								GET_MODBUS_DATA();
								send_count++;
							}

						}
						else
						{
							modbus_urc = 0;
						}




						break;


					case URC_CMD_PACKET:

						APP_DEBUG("URC_CMD_PACKET \n");
						APP_DEBUG("cmd_buf : %s \n",cmd_buf);

						if(compare(cmd_buf,"F:#",100,3))
						{
							FOTA_SMS(cmd_buf);

							//Update_Trial_File(1);
							///update_trial++;
							//Update_Trial_File(0);
							setState(URC_UPDATE_IND);

						}
						if(compare(cmd_buf,"TOT:",100,4))
							{
								SET_TOT_SMS(cmd_buf);
								TOT_DATA_SORT();
							}


						if(compare(cmd_buf,"RESTART:",100,8))
							{

								device_reset=1;
								Ql_Reset(0);

							}
						if(compare(cmd_buf,"L:",100,2))
						{
							publish_timer = fmeter_interval+5;
						}

						if(compare(cmd_buf,"PRODUCT:EFM:",100,12))
						{
							APP_DEBUG("1) PRODUCT:EFM:");
							PRODUCT = 0;
							PRODUCT_File(0);
							publish_timer = fmeter_interval+5;
						}

						if(compare(cmd_buf,"PRODUCT:PIZEO:",100,14))
						{
							PRODUCT = 1;
							PRODUCT_File(0);
							publish_timer = fmeter_interval+5;
						}

						if(compare(cmd_buf,"PRODUCT:PIEZO:",100,14))
						{
							PRODUCT = 1;
							PRODUCT_File(0);
							publish_timer = fmeter_interval+5;
						}

						break;

					case URC_PING_PACKET:
						if(ping_flag == 1)
						{

							if(serial_transfer == 1)
							{
								serial_transfer = 0;
								watchdog_cnt = 100;
							}



							ret=MQTTPing();
							if(ret == SOC_SUCCESS)
							{
								ping_flag=0;
								connection_est=1;
								watchdog_cnt=0;
								break;
							}
							else
							{
								if(MQTT_PING_FAIL == 1)
								{
									ping_flag=0;
									send_flag=0;
									connection_est=0;
									CLR_MQTT_FLAG;
									setState(URC_CONNECT_SERVER);
									break;
								}
								else
								{
									time_delay(500);
									setState(URC_PING_PACKET);
									break;
								}
							}

							//break;
						}

				default:	//switch (msg.param1)
					break;
				}
			default:		//switch(msg.message)
				break;
        }
    }

}
static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen)
{
    s32 rdLen = 0;
    s32 rdTotalLen = 0;
    if (NULL == pBuffer || 0 == bufLen)
    {
        return -1;
    }
    Ql_memset(pBuffer, 0x0, bufLen);
    while (1)
    {
        rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
        if (rdLen <= 0)  // All data is read out, or Serial Port Error!
        {
            break;
        }
        rdTotalLen += rdLen;
        // Continue to read...
    }
    if (rdLen < 0) // Serial Port Error!
    {
        APP_DEBUG("Fail to read from port[%d]\r\n", port);
        return -99;
    }
    return rdTotalLen;
}

static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
    //APP_DEBUG("CallBack_UART_Hdlr: port=%d, event=%d, level=%d, p=%x\r\n", port, msg, level, customizedPara);
    switch (msg)
    {
    case EVENT_UART_READY_TO_READ:
        {
            if (MAIN_UART == port)
            {
                s32 totalBytes = ReadSerialPort(port, m_RxBuf_Uart1, sizeof(m_RxBuf_Uart1));
                if (totalBytes <= 0)
                {
                    APP_DEBUG("<-- No data in UART buffer! -->\r\n");
                    return;
                }
                /*{// Read data from UART
                    s32 ret;
                    char* pCh = NULL;

                    // Echo
                    Ql_UART_Write(MAIN_UART, m_RxBuf_Uart1, totalBytes);

                    pCh = Ql_strstr((char*)m_RxBuf_Uart1, "\r\n");
                    if (pCh)
                    {
                        *(pCh + 0) = '\0';
                        *(pCh + 1) = '\0';
                    }

                    // No permission for single <cr><lf>
                    if (Ql_strlen((char*)m_RxBuf_Uart1) == 0)
                    {
                        return;
                    }
                    ret = Ql_RIL_SendATCmd((char*)m_RxBuf_Uart1, totalBytes, ATResponse_Handler, NULL, 0);
                }*/
            }

            if (UART_PORT2 == port)
			{

            	u8  phy_uart_buff[512] = {0};

            	Ql_memset(m_RxBuf_Uart_RS,0x00,sizeof(m_RxBuf_Uart_RS));
            	Ql_memset(phy_uart_buff,0x00,sizeof(phy_uart_buff));
				s32 totalBytes = ReadSerialPort(UART_PORT2,&phy_uart_buff[0],512);
				serial_transfer = 0;
				APP_DEBUG("__________--CALL-BACK---_________\n");
				//for(int i=0;i<50;i++)
				{
						//APP_DEBUG("%02X|",phy_uart_buff[i]);
				}
				//APP_DEBUG("\n");


				//byte_count=((10*2)+5);
				if(datalen_10 == 0)
				{
					byte_count=((14*2)+5);
				}
				else if(datalen_10 == 1)
				{
					byte_count=((10*2)+5);

				}
			/*------------------------------------*/


				//for(totalBytes2=0;totalBytes2<=byte_count;)
				{
					if(Timeout_Flag == 1)
					{
						Timeout_Flag=0;
						break;

					}

					//totalBytes2 = ReadSerialPort2(UART_PORT2, m_RxBuf_Uart2,byte_count);
					static int i=0;
					if(phy_uart_buff[1] == slave_id )
					{
					for(i=1;i<=(byte_count);i++)
						{
						m_RxBuf_Uart_RS[i-1] = 0;
						m_RxBuf_Uart_RS[i-1] = phy_uart_buff[i];
						}
					//APP_DEBUG("%02X\r\n",m_RxBuf_Uart_RS[0]);
					//APP_DEBUG("m_RxBuf_UartRS1 :");
					//for(i=0;i<=byte_count;i++)
						{
								//APP_DEBUG("%02X|",m_RxBuf_Uart_RS[i]);
						}
					}

					if(phy_uart_buff[0] == slave_id )
					{
					for(i=0;i<=(byte_count);i++)
						{
						//m_RxBuf_Uart_RS[i-1] = 0;
							m_RxBuf_Uart_RS[i] = phy_uart_buff[i];
						}
					//APP_DEBUG("%02X\r\n",m_RxBuf_Uart_RS[0]);
					//APP_DEBUG("m_RxBuf_UartRS2 :");
					//for(i=0;i<=byte_count;i++)
						{
								//APP_DEBUG("%02X|",m_RxBuf_Uart_RS[i]);
						}
					}

					APP_DEBUG("\r\n");

					APP_DEBUG("totalBytes : %d\r\n",totalBytes);
					APP_DEBUG("byte_count : %d\r\n",byte_count);

					if (totalBytes <= 0)
					{
						APP_DEBUG("<-- No data in UART buffer! -->\r\n");

					//	break;
					}



					if(((totalBytes-1) == byte_count) || (totalBytes == byte_count))
					{
						modbus_flag=1;
					//	break;

					}
					else
					{
						APP_DEBUG("NO RESPONSE FROM MODBUS\r\n");//remove
						modbus_urc=1;
						setState(URC_RS_485_PACKET_SEND);

						break;
					}
				}

				modbus_validity(slave_id,0,10);

				if (totalBytes <= 0)
				{
					APP_DEBUG("<-- No data in UART buffer! -->\r\n");
					return;
				}

				  return;

			}





            break;
        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}



void setState(Enum_URCType state)
{
	Ql_OS_SendMessage(main_task_id, MSG_ID_URC_INDICATION, state, 0);
	PREV_TAB = state;
	//Ql_OS_SendMessage(0, MSG_ID_URC_INDICATION, state, 0);

}

static void Callback_Socket_Connect(s32 socketId, s32 errCode, void* customParam )
{
	return;
}

static void Callback_Socket_Accept(s32 socketId, s32 errCode, void* customParam )
{
	return;
}
// This callback function is invoked when socket data arrives.
// The program should call Ql_SOC_Recv to read all data out of the socket buffer.


static void Callback_Socket_Close(s32 socketId, s32 errCode, void* customParam )
{
    if (errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: close socket successfully.-->\r\n");


    }
    else if(errCode == SOC_BEARER_FAIL)
    {
        APP_DEBUG("<--CallBack: fail to close socket,(socketId=%d,error_cause=%d)-->\r\n", socketId, errCode);
    }
    else
    {

        APP_DEBUG("<--CallBack: close socket failure,(socketId=%d,error_cause=%d)-->\r\n", socketId, errCode);


    }
   /* if (1 == m_SocketConnState)
    {
        APP_DEBUG("<-- Socket connection is disconnected -->\r\n");
        APP_DEBUG("<-- Close socket at module side -->\r\n");
        Ql_SOC_Close(socketId);
        m_SocketConnState = 0;
    }*/

	if(socketId == Primary_SocketId)
	{

		Ql_SOC_Close(socketId);
		Primary_SocketId=-1;
	}




}

// This callback function is invoked in the following case:
// The return value is less than the data length to send when calling Ql_SOC_Send(), which indicates
// the socket buffer is full. Application should stop sending socket data till this callback function
// is invoked, which indicates application can continue to send data to socket.
static void Callback_Socket_Write(s32 socketId, s32 errCode, void* customParam)
{

    if (errCode < 0)
    {
        APP_DEBUG("<-- Socket error(error code:%d), close socket.-->\r\n", errCode);
        Ql_SOC_Close(socketId);

        if(socketId == Primary_SocketId)
		{
			Primary_SocketId=-1;
		}


        //m_SocketId = -1;
    }
    else
    {
        APP_DEBUG("<-- You can continue to send data to socket -->\r\n");
    }
}

static void Callback_Socket_Read(s32 socketId, s32 errCode, void* customParam )
{
		s32 ret;
	    s32 offset = 0;
	    u8 rcvd_buff[100];
	    if (errCode)
	    {
	        APP_DEBUG("<-- Close socket -->\r\n");
	        Ql_SOC_Close(socketId);
	        socketId = -1;
	        return;
	    }

	    Ql_memset(m_SocketRcvBuf, 0, SOC_RECV_BUFFER_LEN);
	    do
	    {
	    	//APP_DEBUG("offset : %d\n",offset);
	        ret = Ql_SOC_Recv(socketId, m_SocketRcvBuf + offset, SOC_RECV_BUFFER_LEN - offset);

	        APP_DEBUG("DATA FROM SOCKET :");

	        for(u32 i=0;i<100;i++)
			{
				APP_DEBUG("%c",m_SocketRcvBuf[i]);
			}
			APP_DEBUG("\r\n");

			CONFIG_PACKET();


			if(compare(&m_SocketRcvBuf[23],":CMD",100,4))
			{
				//cmd_flag=1;
				Ql_memset(cmd_buf,0x00,100);
				APP_DEBUG("CMD string received SOC_RECV_BUFFER_LEN == offset\r\n");	//remove
				Ql_memcpy(cmd_buf,&m_SocketRcvBuf[23],100);
				setState(URC_CMD_PACKET);

				break;

			}


			if(compare(&m_SocketRcvBuf[15],"/T",100,1))
			{

				 for(u32 i=15;i<100;i++)
					{
						if(m_SocketRcvBuf[i] == ',')
						{
							if(m_SocketRcvBuf[i-1] == 'N')
							{
								temper_alert = 1;
							}
							else if(m_SocketRcvBuf[i-1] == 'F')
							{
								temper_alert = 0;
							}
						}
					}




			}



	        //   APP_DEBUG(" Ql_SOC_Recv ret : %d\r\n",ret);
	        if((ret < SOC_SUCCESS) && (ret != SOC_WOULDBLOCK))
	        {
	            APP_DEBUG("<-- Fail to receive data, cause=%d.-->\r\n",ret);
	            APP_DEBUG("<-- Close socket.-->\r\n");
	            Ql_SOC_Close(socketId);
	            socketId = -1;
	            break;
	        }
	        else if(SOC_WOULDBLOCK == ret)  // Read finish
	        {
	            APP_DEBUG("<-- Receive data from server,len(%d):%s\r\n", offset, m_SocketRcvBuf);

	            Ql_strcpy(&rcvd_buff[0],&m_SocketRcvBuf[0]);
	            APP_DEBUG("rcvd_buff : %s\n",rcvd_buff);

	            break;
	        }
	        else // Continue to read...
	        {
	            if (SOC_RECV_BUFFER_LEN == offset)  // buffer if full
	            {


	                APP_DEBUG("<-- SOC_RECV_BUFFER_LEN Receive data from server,len(%d):%s\r\n", offset, m_SocketRcvBuf);
	                Ql_memset(m_SocketRcvBuf, 0, SOC_RECV_BUFFER_LEN);
	                offset = 0;
	            }else{
	                offset += ret;
	            }
	            continue;
	        }
	    } while (TRUE);
}


void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode,  u32 ipAddrCnt, u32* ipAddr)
{
    u8 i=0;
    u8* ipSegment = (u8*)ipAddr;

    APP_DEBUG("<-- %s:contexid=%d, requestId=%d,error=%d,num_entry=%d -->\r\n", __func__, contexId, requestId,errCode,ipAddrCnt);
    if (errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: get ip by name successfully.-->\r\n");
        for(i=0;i<ipAddrCnt;i++)
        {
            ipSegment = (u8*)(ipAddr + i);
           // APP_DEBUG("<--Entry=%d, ip=%d.%d.%d.%d-->\r\n",i,ipSegment[0],ipSegment[1],ipSegment[2],ipSegment[3]);
        }

        // Fetch the first ip address as the valid IP
        Ql_memcpy(primary_ipaddress, ipAddr, 4);
        APP_DEBUG("<-- Convert Ip Address successfully,primary_ipaddress=%d,%d,%d,%d -->\r\n",primary_ipaddress[0],primary_ipaddress[1],primary_ipaddress[2],primary_ipaddress[3]);
        setState(URC_CONNECT_SERVER);
        return;

    }

    else
    {
    	APP_DEBUG("<-- Fail to convert IP Address --> \r\n");
    	socket_error++;
    	setState(URC_CONVERT_IP_ADDR);
    	return;
    }


}

#endif // __FLOWMETER_GSM__
