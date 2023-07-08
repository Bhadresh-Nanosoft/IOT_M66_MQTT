/*
 * mqtt.c
 *
 *  Created on: Aug 6, 2019
 *      Author: SW
 */
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
#include "MODBUS.h"
/**********************************************************************************************************************/
bool MQTT_CONNECT_FAIL,MQTT_SUB_FAIL,MQTT_PUBLISH_FAIL,MQTT_PING_FAIL;





/**********************************************************************************************************************/
s32 MQTTConnect(u8 *ClientID , u8 *Will_msg,u8 *User_Name,u8 *Password,u32 KeepAlive)
{
	static u8 err_cnt;
	u8 mqtt_connect[200];
	u8 protocol_name[]="MQIsdp\0";
	u8 mqtt_ack[]={0x20,0x02,0x00,0x00};
	CLR_MQTT_FLAG;
	Ql_memset(mqtt_connect,0x00,sizeof(mqtt_connect));

	mqtt_connect[0]=0x10;

	mqtt_connect[2]=(Ql_strlen(protocol_name)) >> 8;	//MSB protocol name
	mqtt_connect[3]=Ql_strlen(protocol_name);			//LSB protocol name

	//PROTOCOL NAME
	Ql_sprintf(&mqtt_connect[4],"%s",protocol_name);
	//PROTOCOL VERSION
	length=4;
	length+=Ql_strlen(protocol_name);
	mqtt_connect[length++]=0x03;

	// USERNAME PASSWORD RETAIN
	mqtt_connect[length++]=0xEE;

	mqtt_connect[length++]=KeepAlive>>8;	//Keep Alive Timer MSB
	mqtt_connect[length++]=KeepAlive;		//Keep Alive Timer LSB

	mqtt_connect[length++]=(Ql_strlen(ClientID)) >> 8;		//Client ID length MSB
	mqtt_connect[length++]=(Ql_strlen(ClientID));			//Client ID length LSB

	Ql_sprintf(&mqtt_connect[length],"%s",ClientID);	//Client ID name

	length+=(Ql_strlen(ClientID));
	mqtt_connect[length++]=(Ql_strlen(ClientID)) >> 8;		//Will Topic length MSB
	mqtt_connect[length++]=(Ql_strlen(ClientID))+2;			//Will Topic length LSB + 2

	//Will Topic	(IMEI/3)
	Ql_sprintf(&mqtt_connect[length],"%s/3",ClientID);

	//Will message length
	length+=(Ql_strlen(ClientID))+2;
	mqtt_connect[length++]=(Ql_strlen(Will_msg))>>8;		//MSB
	mqtt_connect[length++]=Ql_strlen(Will_msg);				//LSB

	//Will message
	Ql_sprintf(&mqtt_connect[length],"%s",Will_msg);

	// Username length
	length+=Ql_strlen(Will_msg);
	mqtt_connect[length++]=(Ql_strlen(User_Name))>>8;		//MSB
	mqtt_connect[length++]=Ql_strlen(User_Name);			//LSB

	// Username
	Ql_sprintf(&mqtt_connect[length],"%s",User_Name);

	//Password length
	length+=Ql_strlen(User_Name);
	mqtt_connect[length++]=(Ql_strlen(Password))>>8;		//MSB
	mqtt_connect[length++]=Ql_strlen(Password);				//LSB

	//Password
	Ql_sprintf(&mqtt_connect[length],"%s",Password);
	length+=Ql_strlen(Password);

	mqtt_connect[1]=length-2;

	dataLen=length;
	ret=Ql_SOC_Send(Primary_SocketId, mqtt_connect,dataLen);

	APP_DEBUG("ret|dataLen :%d|%d\r\n",ret,dataLen);

	if(ret == dataLen)
	{
		time_delay(500);

		SET_TIMEOUT_TIMER;
		mqtt_rcv_flag=MAIN_UART_SOCKET_READ(4,mqtt_ack);
		CLR_TIMEOUT_TIMER;
		APP_DEBUG("mqtt_rcv_flag : %d\r\n",mqtt_rcv_flag);
		if(mqtt_rcv_flag == 1)
		{
			err_cnt=0;
			return SOC_SUCCESS;
		}

		else
		{
			err_cnt++;
			if(err_cnt > 2)
			{
				err_cnt=0;
				MQTT_CONNECT_FAIL=1;
			}
			return SOC_ERROR;
		}
	}



}

s32 MQTTPublish(u32 flag, u8 *Topic,u32 QOS_Pub)
{
	static u8 err_cnt;
	unsigned long int x;
	unsigned char digit,value[4];
	u8 mqtt_publish[250];
	u32 z=0;
	u8 mqtt_ack[]={0x40,0x02,0x00,0x01};

	ret = 0;
	CLR_MQTT_FLAG;

	Ql_memset(mqtt_publish,0x00,sizeof(mqtt_publish));

	//HEADER
	if(QOS_Pub == 0)
	mqtt_publish[0]=0x31;

	else if(QOS_Pub == 1)
	mqtt_publish[0]=0x33;

	else if(QOS_Pub == 2)
	mqtt_publish[0]=0x35;

	//Topic Length
	length=3;
	mqtt_publish[length++]=0x00;						//Topic Length MSB
	mqtt_publish[length++]=Ql_strlen(Topic)+2;			//Topic Length LSB

	//Topic Name(HID/0)

	if(ON_PACK_FLAG == 1)
	{
		APP_DEBUG("Topic: %s\r\n len : %d\r\n",Topic,Ql_strlen(Topic));
		Ql_sprintf(&mqtt_publish[length],"%s/3",Imei_No);

	}


	if(temper_send_flag == 1)
	{
		APP_DEBUG("Topic: %s\r\n len : %d\r\n",Topic,Ql_strlen(Topic));
		Ql_sprintf(&mqtt_publish[length],"%s/T",Imei_No);

	}

	else
	{
		APP_DEBUG("Topic: %s\r\n",Topic);
		Ql_sprintf(&mqtt_publish[length],"%s/0",Topic);
	}



	//Message ID
	length+=Ql_strlen(Topic) + 2;

	if(QOS_Pub == 1)
	{
		mqtt_publish[length++]=0x00;
		mqtt_publish[length++]=0x01;
	}

	else if(QOS_Pub == 2)
	{
		mqtt_publish[length++]=0x00;
		mqtt_publish[length++]=0x02;
	}

	fmeter_length=IOT_DATA();
	APP_DEBUG("\r\nmqtt_publish : \r\n");

	for(int i=0;i<150;i++)
	{
		APP_DEBUG("%c ",mqtt_publish[i]);
	}

	APP_DEBUG("\r\n");
	APP_DEBUG("fmeter_length: %d\r\n",fmeter_length);
	Ql_strncpy(&mqtt_publish[length],&fmeter_data[0],fmeter_length);
	length+=fmeter_length;
	APP_DEBUG("mqtt_publish: %s\r\n",mqtt_publish);
	x=length-3;
	dataLen=x;

	z=0;
	if(x>127)
	{
		do
		{
			digit=x%128;
			x=x/128;

			if(x>0)
				digit=digit|0x80;

			value[z++]=digit;
		}while(x>0);
		Ql_strncpy(&mqtt_publish[1],&value[0],2);
		x=3;
	}

	else
	{
		mqtt_publish[1]=x;
		x=2;

	}
	APP_DEBUG("mqtt_publish : %s\r\n",mqtt_publish);
	Ql_SOC_Send(Primary_SocketId, &mqtt_publish[0],x);
	ret = Ql_SOC_Send(Primary_SocketId, &mqtt_publish[3],dataLen);
	time_delay(100);

	//SET_TIMEOUT_TIMER;
//	mqtt_rcv_flag=MAIN_UART_SOCKET_READ(4,mqtt_ack);
//	CLR_TIMEOUT_TIMER;

	//if(mqtt_rcv_flag == 1)
	if(ret == dataLen)
	{
		err_cnt=0;
		return SOC_SUCCESS;
	}

	else
	{
		err_cnt++;
		if(err_cnt > 2)
		{
			err_cnt=0;
			MQTT_PUBLISH_FAIL=1;
		}
		return SOC_ERROR;
	}

}

s32 MQTTSubscribe(u8 *Topic,u32 QOS_Sub)
{
	static u8 err_cnt;
	u8 mqtt_sub[250];
	u8 mqtt_ack[]={0x90,0x03,0x00,0x01,0x01};
	CLR_MQTT_FLAG;
	Ql_memset(mqtt_sub,0x00,sizeof(mqtt_sub));

	//HEADER
	if(QOS_Sub == 0)
	mqtt_sub[0]=0x80;

	else if(QOS_Sub == 1)
	mqtt_sub[0]=0x82;

	else if(QOS_Sub == 2)
	mqtt_sub[0]=0x84;

	//MESSAGE ID
	length=2;
	if(QOS_Sub == 1)
	{
		mqtt_sub[length++]=0x00;
		mqtt_sub[length++]=0x01;
	}

	else if(QOS_Sub == 2)
	{
		mqtt_sub[length++]=0x00;
		mqtt_sub[length++]=0x02;
	}

	//Topic Length
	mqtt_sub[length++]=0x00;						//Topic Length MSB
	mqtt_sub[length++]=Ql_strlen(Topic)+2;			//Topic Length LSB

	//Topic Name(IMEI/2)

	if(SLASH_C_SUB == 1)
	{
		Ql_sprintf(&mqtt_sub[length],"%s/S",Topic);
		SLASH_C_SUB = 0;
	}
	if(SLASH_T_SUB == 1)
	{
		Ql_sprintf(&mqtt_sub[length],"%s/T",Topic);
		SLASH_T_SUB = 0;
	}
	else
	{
		Ql_sprintf(&mqtt_sub[length],"%s/2",Topic);
	}


	length+=Ql_strlen(Topic) + 2;

	//Qos level
	if(QOS_Sub==1)
	{
		mqtt_sub[length++]=0x01;
	}

	else if(QOS_Sub==2)
	{
		mqtt_sub[length++]=0x02;
	}

	mqtt_sub[1]=length-2;

	dataLen=length;
	ret = Ql_SOC_Send(Primary_SocketId, mqtt_sub,dataLen);
	time_delay(200);

	//SET_TIMEOUT_TIMER;
	//mqtt_rcv_flag=MAIN_UART_SOCKET_READ(5,mqtt_ack);
	//CLR_TIMEOUT_TIMER;

	//if(mqtt_rcv_flag == 1)
	if(ret == dataLen)
	{
		err_cnt=0;
		return SOC_SUCCESS;
	}

	else
	{
		err_cnt++;
		if(err_cnt > 2)
		{
			err_cnt=0;
			MQTT_SUB_FAIL=1;
		}
		return SOC_ERROR;
	}

}

s32 MQTTPing(void)
{
	static u8 err_cnt;
	u8 mqtt_ping[]={0xC0,0x00};
	u8 mqtt_ack[]={0xD0,0x00};

	CLR_MQTT_FLAG;

	Ql_SOC_Send(Primary_SocketId, mqtt_ping,2);
	time_delay(500);

	SET_TIMEOUT_TIMER;
	mqtt_rcv_flag=MAIN_UART_SOCKET_READ(2,mqtt_ack);
	CLR_TIMEOUT_TIMER;

	if(mqtt_rcv_flag == 1)
	{
		err_cnt=0;
		return SOC_SUCCESS;
	}

	else
	{
		err_cnt++;

		if(err_cnt > 2)
		{
			err_cnt=0;
			MQTT_PING_FAIL=1;
		}
		return SOC_ERROR;
	}

}

u32 IOT_DATA(void)
{

	//long double fmeter_value;
	//DATE_TIME();

	Ql_memset(fmeter_data,0x00,sizeof(fmeter_data));

	if(ON_PACK_FLAG == 1)
	{

		Ql_sprintf(&fmeter_data[0],"0,ON");
		fmeter_length=Ql_strlen(fmeter_data);


	}
	else if(temper_send_flag == 1)
	{
		Ql_sprintf(&fmeter_data[0],"ON");
		fmeter_length=Ql_strlen(fmeter_data);

		if(bat_disconnect_stat == 0)
		{
			Ql_sprintf(&fmeter_data[fmeter_length],",OFF");
			fmeter_length=Ql_strlen(fmeter_data);
		}
		else if(bat_disconnect_stat == 1)
		{
			Ql_sprintf(&fmeter_data[fmeter_length],",ON");
			fmeter_length=Ql_strlen(fmeter_data);
		}

	}

	else
	{

	if(connection_est==0)
	{
		// Hardware ID
		Ql_sprintf(fmeter_data,"#%d",hardware_id);
		Ql_sprintf(&fmeter_data[Ql_strlen(fmeter_data)],"{\"IMEI\":\"%s\",\"TIME\":\"%s%s\",",Imei_No,DATE,TIME);
	}

	else
	{
		// IMEI, TIME,
		Ql_sprintf(fmeter_data,"{\"IMEI\":\"%s\",\"IMSI\":\"%s\",\"SIGNAL\":\"%d\",\"TIME\":\"%s%s\",",Imei_No,Imsi_No,gsm_signal,DATE,TIME);
	}
	fmeter_length=Ql_strlen(fmeter_data);

	//FLOW
	Ql_sprintf(&fmeter_data[fmeter_length],"\"FLOW\":\"%.2lf\",",flow);
	fmeter_length=Ql_strlen(fmeter_data);

	//TOT1
	Ql_sprintf(&fmeter_data[fmeter_length],"\"TOT1\":\"%.2lf\",",TOT1);
	fmeter_length=Ql_strlen(fmeter_data);

	//TOT2
	Ql_sprintf(&fmeter_data[fmeter_length],"\"TOT2\":\"%.2lf\",",TOT2);
	fmeter_length=Ql_strlen(fmeter_data);

	Ql_sprintf(&fmeter_data[fmeter_length],"\"RTOT1\":\"%.2lf\",",RTOT1);
	fmeter_length=Ql_strlen(fmeter_data);

	Ql_sprintf(&fmeter_data[fmeter_length],"\"RTOT2\":\"%.2lf\",",RTOT2);
	fmeter_length=Ql_strlen(fmeter_data);
	//UNIT
	Ql_sprintf(&fmeter_data[fmeter_length],"\"UNT\":\"%d\",",unit);
	fmeter_length=Ql_strlen(fmeter_data);

	//BATTERY(MAIN SUPPLY STATUS)

	Ql_sprintf(&fmeter_data[fmeter_length],"\"POW\":\"%d\",",bat_disconnect_stat);
	fmeter_length=Ql_strlen(fmeter_data);

	Ql_sprintf(&fmeter_data[fmeter_length],"\"TEMPER\":\"%d\",",temper_state);
	fmeter_length=Ql_strlen(fmeter_data);

	//VERSION
	Ql_sprintf(&fmeter_data[fmeter_length],"\"VER\":\"%d\"}",version);
	fmeter_length=Ql_strlen(fmeter_data);
	}

	APP_DEBUG("fmeter_data : %s",fmeter_data);

	return fmeter_length;

}
