/*
 * function.c
 *
 *  Created on: Jul 30, 2019
 *      Author: SW
 */
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_system.h"
#include "ql_gpio.h"
#include "ql_timer.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_uart.h"
#include "ql_memory.h"
#include "ql_fs.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_sim.h"
#include "ril_sms.h"
#include "ril_telephony.h"
#include "declaration.h"
#include "function.h"
//#include "file_system.h"
#include "ql_adc.h"
#include "ql_eint.h"
//#include "sms.h"
#include "ril_location.h"
#include "main.h"
#include "MODBUS.h"
//#include "memory.h"

extern  u8 m_RxBuf_Uart_RS[2048];

void GET_MODBUS_DATA(void)
{


//if(get_fmeter_data == 1)
	{
		gsm_error=DATE_TIME();

		if(gsm_error!=0)
		{
			return;
		}
		SET_TIMEOUT_TIMER;
		MODBUS_TRANSMIT = 1;
		//MODBUS_DATA(slave_id,0,10);

		if(datalen_10 == 0)
		{
			MODBUS_DATA(slave_id,0,14);
		}
		else if(datalen_10 == 1)
		{
			MODBUS_DATA(slave_id,0,10);
			//datalen_10 = 0;
		}




		//CLR_TIMEOUT_TIMER;

		//if(modbus_flag==1)
		{
			//get_fmeter_data=0;
		}

	}

	return;
}


void time_delay(u32 delay_cnt)
{
    u32 delay_i = 0;
    u32 delay_j = 0;
    for(delay_i=0;delay_i<delay_cnt;delay_i++)
    {
       // for(delay_j=0;delay_j<1000000;delay_j++);
    	for(delay_j=0;delay_j<1;delay_j++);
        Ql_Sleep(0);


    }
}

void time_delay_micro_sec(u32 delay_cnt)
{
    u32 delay_i = 0;
    u32 delay_j = 0;
    u32 delay_k = 1;

    while(delay_k--)
    {
    	 for(delay_i=0;delay_i<delay_cnt;delay_i++);
    }

    /*
    for(delay_i=0;delay_i<delay_cnt;delay_i++)
    {
    	//for(delay_j=0;delay_j<32760;delay_j++);
    	for(delay_j=0;delay_j<0;delay_j++)
    	{
    		//for(delay_k=0;delay_k<2;delay_k++);
    	}

    }
    */
}


/*void RIL_CURRENT_OPERATOR(void)
{

	iRet=-1;
	Ql_memset(strAT,0x00,sizeof(strAT));
	Ql_memset(response,0x00,sizeof(response));

	Ql_sprintf(strAT,"AT+COPS=3,0\0");
	APP_DEBUG("%s\r\n",strAT);
	iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,(void*)response,0);
	APP_DEBUG("%s\r\n",response);

	iRet=-1;
	Ql_memset(strAT,0x00,sizeof(strAT));
	Ql_memset(response,0x00,sizeof(response));


	Ql_sprintf(strAT,"AT+COPS?\0");
	APP_DEBUG("%s\r\n",strAT);
	iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,(void*)response,0);
	APP_DEBUG("current operator :%s\r\n",response);

	if(iRet == RIL_AT_SUCCESS)
	{

		//p_res=NULL;
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		p_res=Ql_strstr(response,"+COPS: ");

		if(p_res)
		{
			p_res += 7;
			comma_cnt=0;
			Ql_memset(oper_name,0x00,sizeof(oper_name));

			for(length=0;length<100;length++)
			{
				if(*p_res == ',')
				{
					comma_cnt++;
				}

				if(comma_cnt == 2)
				{
					comma_cnt=0;
					break;
				}

				p_res++;
			}

			p_res+=2;

			for(length=0;length<50;length++)
			{
				if(*p_res == 0x22)
				{
					oper_name[length]=0x00;
					break;
				}
				oper_name[length]=*p_res++;

			}




		}

		APP_DEBUG("length : %d,operator_name : %s\r\n",length,oper_name);	//remove
		//Free memory
		Ql_MEM_Free(memory_alloc);
		memory_alloc=p_res=NULL;
	}


}*/

void RIL_LIST_OPERATOR(void)
{

	//*p_res=NULL;
	Ql_memset(response,0x00,sizeof(response));
	Ql_memset(strAT,0x00,sizeof(strAT));
	for(operator_len=0; operator_len<6;operator_len++)
		Ql_memset(operator_code[operator_len],0x00,sizeof(operator_code[operator_len]));

	Ql_sprintf(strAT,"AT+COPS=?\0");
	APP_DEBUG("%s",strAT);



	iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,&response,0);
	APP_DEBUG("<-- Operator:%s, iRet=%d -->\r\n", response, iRet);

	if(iRet == RIL_AT_SUCCESS)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate Memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));


		p_res=Ql_strstr(response,"+COPS: ");
		p_res=p_res+8;
		comma_cnt=0;

		//for(operator_len=0; operator_len<6;operator_len++)
		//Ql_memset(operator_code[operator_len],0x00,10);

		for(operator_len=0; operator_len < 6; operator_len++)
		{
			if(*p_res >= 0x30 && *p_res <= 0x34)
			{
				for(length=0;length<100;length++)
				{
					p_res++;
					if(*p_res == ',')
					{
						comma_cnt++;
					}

					if(comma_cnt == 3)
					{
						p_res+=2;
						comma_cnt=0;
						break;
					}
				}

				for(length=0;length<8;length++)
				{
					if(*p_res != '"')
					{
						//APP_DEBUG("p_res:%c\r\n",*p_res);
						operator_code[operator_len][length]=*p_res++;
					}

					else
					{
						operator_code[operator_len][length]=0x00;
						p_res+=4;
						break;
					}
				}
			}



			else
			break;
		}
		//Free memory
		Ql_MEM_Free(memory_alloc);
		memory_alloc=p_res=NULL;
	}

}

void SYNC_UPDATE(void)
{

	Ql_memset(response,0x00,sizeof(response));
	Ql_memset(strAT,0x00,sizeof(strAT));
	Ql_sprintf(strAT,"AT+CTZU=3\0");	// syn local time
	//APP_DEBUG("%s\r\n",strAT);

	iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,&response,0);
	//APP_DEBUG("%s", response);

}

u32 DATE_TIME(void)
{

	Ql_memset(response,0x00,sizeof(response));
	Ql_memset(strAT,0x00,sizeof(strAT));
	Ql_sprintf(strAT,"AT+CCLK?\0");
	//APP_DEBUG("%s\r\n",strAT);

	iRet = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,&response,0);
	//APP_DEBUG("date time:%s", response);

	if(iRet == RIL_AT_SUCCESS)
	{
		if(Ql_strstr(response,"+CCLK: "))
		{
			u8 *p_res=NULL,*memory_alloc;
			gsm_error=0;
			//Allocate Memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
			p_res=Ql_strstr(response,"+CCLK: ");
			p_res+=8;

			Ql_memset(DATE,0x00,sizeof(DATE));
			for(length=0;length<10;length++)
			{
				if(*p_res == LIB_SMS_CHAR_COMMA)
				{
					DATE[length]=0x00;
					p_res++;
					break;
				}

				if(*p_res == '/')
					p_res++;

				DATE[length]=*p_res++;
			}

			for(length=0;length<6;length++)
			{
				if(DATE[length]<0x30 || DATE[length]>0x39)
				{
					gsm_error++;
					return gsm_error;
				}
			}
			Ql_memset(TIME,0x00,sizeof(TIME));
			for(length=0;length<15;length++)
			{
				if(*p_res == '+')
				{
					TIME[length]=0x00;
					p_res++;
					break;
				}

				if(*p_res == ':')
					p_res++;

				TIME[length]=*p_res++;
			}

			for(length=0;length<6;length++)
			{
				if(TIME[length]<0x30 || TIME[length]>0x39)
				{
					gsm_error++;
					return gsm_error;
				}
			}


		}
		else
			gsm_error++;
	}

	else
	{
		gsm_error++;
	}

	return gsm_error;


}

void write_fmeter_memory(void)
{
	u32 addr;

	// read start addr
	addr=Read_SAddr_File();

	if(addr >= 30000)
		addr=0;

	fmeter_data_File(0,fmeter_data,addr,Ql_strlen(fmeter_data));
	addr+=Ql_strlen(fmeter_data);
	Write_SAddr_File(addr);

}

void publish_history_data(u32 QOS_Pub)
{

	unsigned long int x;
	unsigned char digit,value[4];
	u32 z=0;
	u8 modbus_publish_str[200];
	u8 mqtt_publish[250];
	u8 hardware_id_str[5];
	u8 mqtt_ack[]={0x40,0x02,0x00,0x01};
	u32 publish_length=0;
	u32 length2=0,length3=0;
	u32 addr;
	bool history_data_error=1;


	addr=Read_EAddr_File();
	Ql_memset(modbus_str,0x00,sizeof(modbus_str));
	fmeter_data_File(1,modbus_str,addr,200);
	//read_fmeter_memory();

	for(length3=0;length3<200;length3++)
	{
		if(modbus_str[length3]=='#')//if(modbus_str[0] == '{')
		{
			for(length=length3+1;length<Ql_strlen(modbus_str);length++)
			{
				if(modbus_str[length]=='}')
				{
					modbus_publish_str[publish_length++]=modbus_str[length];
					modbus_publish_str[publish_length]=0x00;
					addr+=length+1;
					history_data_error=0;
					break;
				}

				else
				{
					modbus_publish_str[publish_length++]=modbus_str[length];
				}
			}
		}

		if(history_data_error==0)
			break;
	}

	if(history_data_error==1)
	{
		addr+=length3;
	}

	// write end addr;
	if(addr >= 30000)
	addr=0;

	Write_EAddr_File(addr);


	if(history_data_error == 1)
		return;

	Ql_memset(hardware_id_str,0x00,sizeof(hardware_id_str));
	Ql_memset(mqtt_publish,0x00,sizeof(mqtt_publish));
	for(length=0;length<5;length++)
	{
		if(modbus_publish_str[length]=='{')
		{
			hardware_id_str[length]=0x00;
			break;
		}

		hardware_id_str[length]=modbus_publish_str[length];
	}

	//HEADER
	if(QOS_Pub == 0)
	mqtt_publish[0]=0x31;

	else if(QOS_Pub == 1)
	mqtt_publish[0]=0x33;

	else if(QOS_Pub == 2)
	mqtt_publish[0]=0x35;

	//Topic Length
	length2=3;
	mqtt_publish[length2++]=0x00;							//Topic Length MSB
	mqtt_publish[length2++]=Ql_strlen(hardware_id_str)+2;	//Topic Length LSB

	//Topic Name(HID/0)
	Ql_sprintf(&mqtt_publish[length2],"%s/1",hardware_id_str);

	//Message ID
	length2+=Ql_strlen(hardware_id_str) + 2;

	if(QOS_Pub == 1)
	{
		mqtt_publish[length2++]=0x00;
		mqtt_publish[length2++]=0x01;
	}

	else if(QOS_Pub == 2)
	{
		mqtt_publish[length2++]=0x00;
		mqtt_publish[length2++]=0x02;
	}

	//length2=3;
	//Ql_sprintf(&mqtt_publish[length2],"{\"IMEI\":\"%s\",",Imei_No);
	//length2+=Ql_strlen(&mqtt_publish[length2]);

	Ql_strncpy(&mqtt_publish[length2],&modbus_publish_str[length],Ql_strlen(modbus_publish_str));
	length2+=Ql_strlen(&mqtt_publish[length2]);

	//Ql_sprintf(&mqtt_publish[length2],",\"BAT\":\"0\",\"VER\":\"1\"}");
	//length2+=Ql_strlen(&mqtt_publish[length2]);

	x=length2-3;
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
	//APP_DEBUG("length2:%d\r\n",length2);


	Ql_SOC_Send(Primary_SocketId, &mqtt_publish[0],x);
	Ql_SOC_Send(Primary_SocketId, &mqtt_publish[3],dataLen);
	time_delay(1000);

	SET_TIMEOUT_TIMER;
	mqtt_rcv_flag=MAIN_UART_SOCKET_READ(4,mqtt_ack);
	CLR_TIMEOUT_TIMER;

	if(mqtt_rcv_flag == 1)
	{

	}

	else
	{

	}

}

void read_fmeter_memory(void)
{
	u32 addr;
	//u8 modbus_str[100];
	// read end addr
	addr=Read_EAddr_File();
	fmeter_data_File(1,modbus_str,addr,200);
}

void READ_MEMORY(void)
{
	Admin_Number_File(1);
	Number_File(1);
	Update_Parameter_File(1);
	APN_TOT_COUNT_File(1);
	APN_RUN_COUNT_File(1);
	APN_File(1);
	IP_DNS_File(1);
	PORT_File(1);
	Password_File(1);
	INTERVAL_File(1);
	ERR_File(1);
	Manual_Mode_File(1);
	SMS_File(1);
	Config_File(1);
	PRODUCT_File(1);
	SMS_CLEAR_File(1);
	TOT_num_of_fm_File(1);
}

/************** HANDLER ********************************************************/
s32 ATResponse_Handler(unsigned char* line, unsigned int len, void* userdata)
{

	//char* response = userdata;
	length=Ql_strlen(userdata);
	Ql_memcpy(&userdata[length], line, len);

	if (Ql_RIL_FindLine(line, len, "OK"))
	{


		return RIL_AT_SUCCESS;
	}



	if (Ql_RIL_FindLine(line, len, "ERROR"))
	{

		return RIL_AT_FAILED;
	}

	return RIL_ATRSP_CONTINUE; //continue wait
}

s32 ATSwitch_Handler(unsigned char* line, unsigned int len, void* userdata)
{

	//char* response = userdata;
	length=Ql_strlen(userdata);
	Ql_memcpy(&userdata[length], line, len);

	/*if (Ql_RIL_FindLine(line, len, "OK"))
	{


		return RIL_AT_SUCCESS;
	}*/



	if (Ql_RIL_FindLine(line, len, "ERROR"))
	{

		return RIL_AT_FAILED;
	}

	return RIL_ATRSP_CONTINUE; //continue wait
}

s32 AT_MSG_Handler(unsigned char* line, u32 len, unsigned char* param)
{
	char *head=NULL;
	head = Ql_RIL_FindLine(line, len, "OK"); // find <CR><LF>OK<CR><LF>, <CR>OK<CR>£¬<LF>OK<LF>
	length=Ql_strlen(param);
	Ql_memcpy(&param[length], line, len);

	if(head)
	{

		return  RIL_ATRSP_SUCCESS;
	}

	return RIL_ATRSP_CONTINUE; //continue wait

}

/*s32 ATRsp_COPS_Handler(char* line, u32 len, void* param)
{
	//char* response = userdata;
		length=Ql_strlen(param);
		Ql_memcpy(&param[length], line, len);

		if (Ql_RIL_FindLine(line, len, "OK"))
		{


			return RIL_AT_SUCCESS;
		}



		if (Ql_RIL_FindLine(line, len, "ERROR"))
		{

			return RIL_AT_FAILED;
		}

		return RIL_ATRSP_CONTINUE; //continue wait
}*/

s32 ATRsp_COPS_Handler(unsigned char* line, u32 len, void* param)
{
	char* pHead=NULL;
	length=Ql_strlen(param);
	Ql_memcpy(&param[length], line, len);

	//APP_DEBUG("%s",line);
    /*char* pStr = (char *)param;
    char* pHead = Ql_RIL_FindString(line, len, "+COPS:"); //continue wait



    if (pHead)
    {
        char str[100] = {0};
        char *p = NULL;
        char *q = NULL;
        p = pHead + Ql_strlen("+COPS: ");
        q = Ql_strstr(p, "\"");
        if (p)
        {// the response is like: +COPS: 0,0,"CHINA MOBILE"
            p = q + 1;
            q = Ql_strstr(p, "\"");
            if (q != NULL)
            {
                Ql_memcpy(pStr, p, q - p);
                pStr[q - p] = '\0';
            }
        }
        else
        {//  the response is like +COPS: 0
            *pStr = '\0';
        }
        return  RIL_ATRSP_SUCCESS;
    }*/




  // pHead = Ql_RIL_FindLine(line, len, "OK"); // find <CR><LF>OK<CR><LF>, <CR>OK<CR>£¬<LF>OK<LF>
   if (Ql_RIL_FindLine(line, len, "OK"))
   {

       return  RIL_ATRSP_SUCCESS;
   }

    //pHead = Ql_RIL_FindLine(line, len, "ERROR");// find <CR><LF>ERROR<CR><LF>, <CR>ERROR<CR>£¬<LF>ERROR<LF>
    if (Ql_RIL_FindLine(line, len, "ERROR"))
    {

        return  RIL_ATRSP_FAILED;
    }

   // pHead = Ql_RIL_FindString(line, len, "+CME ERROR:");//fail
    if (Ql_RIL_FindString(line, len, "+CME ERROR:"))
    {

        return  RIL_ATRSP_FAILED;
    }

    return RIL_ATRSP_CONTINUE; //continue wait
}

s32 MAIN_UART_SOCKET_READ(u32 response_length,u8 *ack)
{
	s32 read_ret=-1;
	s32 offset = 0;
	mqtt_rcv_flag=0;

	Ql_memset(m_SocketRcvBuf, 0, SOC_RECV_BUFFER_LEN);

	for(offset=0;offset<=response_length;)
	{
		if(Timeout_Flag == 1)
		{
			Timeout_Flag=0;
			break;
		}
		read_ret = Ql_SOC_Recv(Primary_SocketId, m_SocketRcvBuf + offset, SOC_RECV_BUFFER_LEN - offset);
		if((read_ret < SOC_SUCCESS) && (read_ret != SOC_WOULDBLOCK))
		{
			APP_DEBUG("<-- Fail to receive data, cause=%d.-->\r\n",read_ret);
			APP_DEBUG("<-- Close socket.-->\r\n");
			Ql_SOC_Close(Primary_SocketId);

			// m_SocketId = -1;
			break;
		}

		else // Continue to read...
		{

			if (SOC_RECV_BUFFER_LEN == offset)  // buffer if full
			{
				APP_DEBUG("<-- Receive data from server,len(%d):%s\r\n", offset, m_SocketRcvBuf);
				Ql_memset(m_SocketRcvBuf, 0, SOC_RECV_BUFFER_LEN);
				offset = 0;
				break;
			}

			else if(offset == response_length)
			{
				APP_DEBUG("<-- Receive data from server,len(%d):%s\r\n", offset, m_SocketRcvBuf);
				if(Ql_strncmp(m_SocketRcvBuf,ack,response_length)==0)
				{
					mqtt_rcv_flag=1;
					break;
					//APP_DEBUG("<-- Receive data from server,len(%d):%s\r\n", offset, m_SocketRcvBuf);
				}
				else
					break;

			}
			else
			{
				offset += read_ret;
			}
			//continue;
		}
	}

	APP_DEBUG("MQTT RESPONCE : %s",m_SocketRcvBuf);
	return mqtt_rcv_flag;


}







/**************************** CALL BACK FUNCTION *******************************************/
void Callback_GPRS_Actived(u8 contexId, s32 errCode, void* customParam)
{
    if(errCode == SOC_SUCCESS)
    {
    	//APP_DEBUG("<--CallBack: active GPRS successfully.-->\r\n");
      //  m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
    }
    else
    {
    	//APP_DEBUG("<--CallBack: active GPRS successfully,errCode=%d-->\r\n",errCode);
       // m_tcp_state = STATE_GPRS_ACTIVATE;
    }
}

void CallBack_GPRS_Deactived(u8 contextId, s32 errCode, void* customParam )
{
	//m_nSentLen  = 0;

    if (errCode == SOC_SUCCESS)
    {
    	//APP_DEBUG("<--CallBack: deactived GPRS successfully.-->\r\n");
       // m_tcp_state = STATE_NW_GET_SIMSTATE;
    }
    else
    {
    	//APP_DEBUG("<--CallBack: deactived GPRS failure,(contexid=%d,error_cause=%d)-->\r\n",contextId,errCode);
    }
}


void SLEAVE_SELECTION(void)
{


	slave_id = slave_id +1;


	if(slave_id>(10+TOT_DEV_ID-1))
	{
		slave_id = 10;


		get_fmeter_data=0;
		publish_urc=0;
		//modbus_urc=0;
	}
	else
	{

	}
	send_count = 0;
	APP_DEBUG("slave_id : %d\n",slave_id);

}


void modbus_validity(u8 device_id,u32 address,u32 no_register)
{

	u32 CRC_TEMP,CRC_Calculation;
			u8 transmit[10];

			APP_DEBUG("device_id : %d\r\n",device_id);
	if(modbus_flag == 1)
			{
				//APP_DEBUG("device_id : %d\r\n",device_id);

				if(m_RxBuf_Uart_RS[0]==device_id)	//slave address
				{
					APP_DEBUG("device_id : %d\r\n",device_id);

					CRC_TEMP = ModRTU_CRC(m_RxBuf_Uart_RS ,byte_count-2);
					APP_DEBUG("byte_count : %d\r\n",byte_count);
					CRC_Calculation = 0x0000 | m_RxBuf_Uart_RS[byte_count-1];
					CRC_Calculation = (CRC_Calculation << 8) | m_RxBuf_Uart_RS[byte_count-2];

					APP_DEBUG("CRC_TEMP : %d\r\nCRC_Calculation : %d\r\n",CRC_TEMP,CRC_Calculation);
					if(CRC_TEMP == CRC_Calculation)
					{
						//flow
						numero.num2[3]=m_RxBuf_Uart_RS[3];
						numero.num2[2]=m_RxBuf_Uart_RS[4];

						numero.num2[1]=m_RxBuf_Uart_RS[5];
						numero.num2[0]=m_RxBuf_Uart_RS[6];

						flow=numero.num;

						//TOT1
						numero.num2[3]=m_RxBuf_Uart_RS[7];
						numero.num2[2]=m_RxBuf_Uart_RS[8];

						numero.num2[1]=m_RxBuf_Uart_RS[9];
						numero.num2[0]=m_RxBuf_Uart_RS[10];

						TOT1=numero.num;

						//TOT2
						numero.num2[3]=m_RxBuf_Uart_RS[11];
						numero.num2[2]=m_RxBuf_Uart_RS[12];

						numero.num2[1]=m_RxBuf_Uart_RS[13];
						numero.num2[0]=m_RxBuf_Uart_RS[14];

						TOT2=numero.num;

						// hardware_id
						numero.num2[3]=m_RxBuf_Uart_RS[15];
						numero.num2[2]=m_RxBuf_Uart_RS[16];

						numero.num2[1]=m_RxBuf_Uart_RS[17];
						numero.num2[0]=m_RxBuf_Uart_RS[18];

						hardware_id = numero.num;//numero.num1;

						//unit
						numero.num2[3]=m_RxBuf_Uart_RS[19];
						numero.num2[2]=m_RxBuf_Uart_RS[20];

						numero.num2[1]=m_RxBuf_Uart_RS[21];
						numero.num2[0]=m_RxBuf_Uart_RS[22];

						unit=numero.num;


						if(datalen_10 == 0)
						{

						numero.num2[3]=m_RxBuf_Uart_RS[23];
						numero.num2[2]=m_RxBuf_Uart_RS[24];

						numero.num2[1]=m_RxBuf_Uart_RS[25];
						numero.num2[0]=m_RxBuf_Uart_RS[26];

						RTOT1=numero.num;




						numero.num2[3]=m_RxBuf_Uart_RS[27];
						numero.num2[2]=m_RxBuf_Uart_RS[28];

						numero.num2[1]=m_RxBuf_Uart_RS[29];
						numero.num2[0]=m_RxBuf_Uart_RS[30];

						RTOT2=numero.num;

						}
						else if(datalen_10 == 1)
						{
							numero.num2[3]= 0;
							numero.num2[2]= 0;

							numero.num2[1]= 0;
							numero.num2[0]= 0;

							RTOT1=numero.num;




							numero.num2[3]= 0;
							numero.num2[2]= 0;

							numero.num2[1]= 0;
							numero.num2[0]= 0;

							RTOT2=numero.num;

							datalen_10 = 0;


						}


						//remove
						APP_DEBUG("flow:%f\r\n",flow);
						APP_DEBUG("TOT1:%f\r\n",TOT1);
						APP_DEBUG("TOT2:%f\r\n",TOT2);
						APP_DEBUG("hardware_id:%d\r\n",hardware_id);
						APP_DEBUG("unit:%d\r\n",unit);
						APP_DEBUG("RTOT1:%f\r\n",RTOT1);
						APP_DEBUG("RTOT2:%f\r\n",RTOT2);



						if((flow>0) && (PUMP_ON_DETECT == OFF))
						{
							RS485_READ_QUERY_SEND=0;
							APP_DEBUG("1 RS485_READ_QUERY_SEND = 0\r\n");
							publish_urc = 0;
							PUMP_ON_DETECT = ON;

						}

						if((PUMP_ON_DETECT == ON) &&  (flow<=0))
						{
							RS485_READ_QUERY_SEND=0;
							APP_DEBUG("2 RS485_READ_QUERY_SEND = 0\r\n");
							publish_urc = 0;
							PUMP_ON_DETECT = OFF;
						}


						if(connection_est == 1 && RS485_READ_QUERY_SEND == 0)
						send_flag=1;

						else if	(RS485_READ_QUERY_SEND == 0)
						{
							send_flag=0;
							//IOT_DATA();
							IOT_DATA_WRITE();
							write_fmeter_memory();

						}



						if(connection_est == 1 && publish_urc==0 && send_flag==1 && RS485_READ_QUERY_SEND == 0)
						{

							publish_urc=1;
							modbus_urc=0;
							setState(URC_PUBLISH_PACKET);

						}


						if(RS485_READ_QUERY_SEND == 1)
						{
							setState(PREV_TAB);
							RS485_READ_QUERY_SEND = 0;

						}



					}
					//remove
					else
					{
						APP_DEBUG("crc not match\r\n");	//remove

					}
				}
				else
				{
					setState(URC_RS_485_PACKET_SEND);
					modbus_urc=1;
				}


		}
	else
	{
		setState(URC_RS_485_PACKET_SEND);
		modbus_urc=1;
	}
}


void CONFIG_PACKET(void)
{



	if(Ql_strstr(&m_SocketRcvBuf[22],"TOTHW"))
	{
		NO_OF_HW_CONFIG(&m_SocketRcvBuf[22]);
	}

	if(Ql_strstr(&m_SocketRcvBuf[22],"INTL"))
	{
		TIME_INTERVAL_CONFIG(&m_SocketRcvBuf[22]);
	}

	if(Ql_strstr(&m_SocketRcvBuf[22],"APN"))
	{
		//TIME_INTERVAL_CONFIG(&m_SocketRcvBuf[22]);
		APN_SMS(&m_SocketRcvBuf[22]);
	}



}


unsigned char compare(unsigned char * p1, unsigned char *p2, unsigned int source_len,unsigned int dest_len)
{
	unsigned char *p3,*p4;
	unsigned char x ,y;
	p4=p2;

  for(x = 0; x<source_len; x++)
  {
    if(*p1 == *p2)
      {
          p3 = p1;

          for(y = 0;y<dest_len;y++)
	  {
            if(*p3 == *p2)
            {
              p3++;
	     p2++;
            }
            else
              break;
          }
         // p2 = buff3;
         p2=p4;
          if(y == dest_len )
	  {
		  //cmd_flag=1;
           return 1;
          }
      }
    p1++;
  }

return 0;
}



void value_ftohex_SET_TOT_1()
{
	unsigned char *x = (char *)&SET_TOT_1;
	   for(int i=0; i<4; i++)
	   {
		   hexVals[i] = *x++ & 0x0000ff;
	     //printf("%x ", *x++ & 0x0000FF);
	   }
}


void value_ftohex_SET_TOT_2()
{
	unsigned  char *x = (char *)&SET_TOT_2;
	   for(int i=0; i<4; i++)
	   {
		   hexVals[i] = *x++ & 0x0000ff;
	     //printf("%x ", *x++ & 0x0000FF);
	   }
}


void value_ftohex_SET_RTOT_1()
{
	unsigned char *x = (char *)&RSET_TOT_1;
	   for(int i=0; i<4; i++)
	   {
		   hexVals[i] = *x++ & 0x0000ff;
	     //printf("%x ", *x++ & 0x0000FF);
	   }
}


void value_ftohex_SET_RTOT_2()
{
	unsigned  char *x = (char *)&RSET_TOT_2;
	   for(int i=0; i<4; i++)
	   {
		   hexVals[i] = *x++ & 0x0000ff;
	     //printf("%x ", *x++ & 0x0000FF);
	   }
}


void TOT_DATA_SORT(void)
{
	if(modbus_write == 1)
	{
		//SET_TOT = 196805;

		que = SET_TOT/65535;
		SET_TOT_2 =(float)que;
		SET_TOT_1 =  (que * 65535);
		SET_TOT_1 = SET_TOT - SET_TOT_1;

		APP_DEBUG("TOT_DATA_SORT \r\n");
		APP_DEBUG("SET_TOT 		: %f\n",SET_TOT);
		APP_DEBUG("SET_TOT_1 	: %f\n",SET_TOT_1);
		APP_DEBUG("SET_TOT_2 	: %f\n",SET_TOT_2);

		que = 0;

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



}

void IOT_DATA_WRITE()
{
	//if(connection_est==0)
	{
		// Hardware ID
		if(PRODUCT == 0)
			Ql_sprintf(fmeter_data,"#%d",hardware_id);

		else if(PRODUCT == 1)
			Ql_sprintf(fmeter_data,"#P%d",hardware_id);

		Ql_sprintf(&fmeter_data[Ql_strlen(fmeter_data)],"{\"IMEI\":\"%s\",\"TIME\":\"%s%s\",",Imei_No,DATE,TIME);
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


	APP_DEBUG("fmeter_data : %s",fmeter_data);
}
void APN_CHANGE(void)
 {

 	if(APN_TOT_COUNTER == APN_RUN_COUNTER ||  APN_TOT_COUNTER < APN_RUN_COUNTER )
 	{
 		APN_RUN_COUNTER = 1;
 	}
 	else
 	{
 		APN_RUN_COUNTER++;
 	}


 	APN_RUN_COUNT_File(0);


 }
