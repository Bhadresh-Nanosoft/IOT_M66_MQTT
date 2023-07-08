/*
 * watchdog_task.c
 *
 *  Created on: Aug 5, 2019
 *      Author: SW
 */
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




static void Callback_Wdt_Timer(u32, void*);
void setState_wdt(Enum_RS485Type);

extern ST_MSG msg;
//static void Callback_Timer(u32, void*);

void proc_wdt_task(s32 TaskId)
{
	ST_MSG wdt_task_msg;
	s32 wtdid;

	//Ql_GPIO_Init(PINNAME_PCM_OUT, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	/** WATCHDOG **/
	Ql_WTD_Init(0, PINNAME_DCD, 200);
	wtdid = Ql_WTD_Start(4000);

	Ql_Timer_Register(LOGIC_WTD1_TMR_ID, Callback_Wdt_Timer, &wtdid);
	Ql_Timer_Start(LOGIC_WTD1_TMR_ID, 2000,TRUE);

	Ql_Timer_Register(ONE_SEC_TMR_ID, Callback_Timer, NULL);
	Ql_Timer_Start(ONE_SEC_TMR_ID, 1000,TRUE);

	//setState_wdt(RS485_SYS_BEGIN);

	//APP_DEBUG("WATCH_DOG\\r\n");

	while(1)
	{
		Ql_OS_GetMessage(&wdt_task_msg);

		switch(wdt_task_msg.message)
		{

			case MSG_ID_USER_DATA:
			{



				break;	//case MSG_ID_USER_DATA:
			}

			case MSG_ID_USER_RS485:
			{




				break;	//case MSG_ID_USER_DATA:
			}

			default:	//switch(wdt_task_msg.message)
			break;
		}
	}
}

void setState_wdt(Enum_RS485Type state)
{
	Ql_OS_SendMessage(wdt_task_id, MSG_ID_USER_DATA, state, 0);
	//Ql_OS_SendMessage(0, MSG_ID_URC_INDICATION, state, 0);

}


static void Callback_Wdt_Timer(u32 timerId, void* param)
{
	if(timerId == LOGIC_WTD1_TMR_ID /*&& sms_read_error == 0*/)
	{
		if(watchdog_cnt < 90)
		{
			s32* wtdid = (s32*)param;
			Ql_WTD_Feed(*wtdid);

		}

		watchdog_cnt++;
	//	APP_DEBUG("-------------------> WD :%d\n",watchdog_cnt);
		NETWORK_RST++;
		if(watchdog_cnt >= 60 && connection_est==1 && update_flag == 0)
		{
			if(ping_flag == 0)
			{
				ping_flag=1;
				modbus_urc=0;
				NETWORK_RST =0;
				setState(URC_PING_PACKET);

			}
		}

		if(connection_est != 1)
		{
			if(NETWORK_RST<GPRS_TIMEOUT)
			{
				NETWORK_RST++;
				//APP_DEBUG("NETWORK_RST : %d\n ",NETWORK_RST);
			}
			else
			{
				APP_DEBUG("RESTART : NETWORK_RST\n ");
				NETWORK_RST = 0;
				Ql_Reset(0);
			}
		}


	//	APP_DEBUG("RS485_QUERY_COUNT : %d\n",RS485_QUERY_COUNT);

		if((RS485_QUERY_COUNT > 30) && (RS485_QUERY == ON) )
		{
			modbus_urc=1;
			RS485_READ_QUERY_SEND = 1;
			setState(URC_MODBUS_DATA);
			RS485_QUERY_COUNT = 0;

		}



	}

}

void Callback_Timer(u32 timerId, void* param)
{

	if(timerId == ONE_SEC_TMR_ID)
	{
		publish_timer++;
		RS485_QUERY_COUNT++;


		if(publish_timer >= fmeter_interval)
		{
			publish_timer=0;
			get_fmeter_data=1;
		}

		if(get_fmeter_data == 1)
		{
			if(modbus_urc==0 && ping_flag == 0 && publish_urc==0 && update_flag == 0 /*&& connection_est == 1*/)
			{
				modbus_urc=1;
				setState(URC_MODBUS_DATA);
			}

		}


		if(Ql_GPIO_GetLevel(POWER_PIN))
		{
			//APP_DEBUG("<-- POWER_PIN : %d-->\r\n",Ql_GPIO_GetLevel(POWER_PIN));
			bat_disconnect_stat = 1;

			if(bat_disc_alert == 1 && first_data_packet_send == 1 )
			{
				temper_send_flag = 1;
				send_flag = 1;
				bat_disc_alert = 0;
				setState(URC_PUBLISH_PACKET);
			}


		}
		else
		{
			//APP_DEBUG("<-- POWER_PIN : %d-->\r\n",Ql_GPIO_GetLevel(POWER_PIN));
			bat_disconnect_stat = 0;
			if(temper_send_flag ==  0 && first_data_packet_send == 1 && bat_disc_alert == 0 )
			{
				//fmeter_interval = 15;
				temper_send_flag = 1;
				send_flag = 1;
				bat_disc_alert = 1;
				bat_disconnect_stat = 0;
				setState(URC_PUBLISH_PACKET);

			}


		}

		if(Ql_GPIO_GetLevel(TEMPER_SWITCH_INT))
		{
			//APP_DEBUG("<-- TEMPER_SWITCH_INT : %d-->\r\n",Ql_GPIO_GetLevel(TEMPER_SWITCH_INT));

			if(temper_send_flag ==  0 && temper_alert == 0 && first_data_packet_send == 1)
			{
				//temper_state = 1;
				//fmeter_interval = 15;
				temper_send_flag = 1;
				send_flag = 1;
				temper_alert = 1;
				setState(URC_PUBLISH_PACKET);
			}

			temper_state = 1;
		}
		else
		{
			temper_state = 0;
		}




		if(timer_start==1)
		{
			timer_cnt++;
			if(timer_cnt >=time_out_timer)
			{
				APP_DEBUG("__________TIMEOUT__________\r\n");
				Timeout_Flag=1;
				timer_start=0;
			}
		}
	}

}

void Callback_Socket_Read(s32 socketId, s32 errCode, void* customParam )
{

}
#endif
