/*
 * MODBUS.c
 *
 *  Created on: Aug 9, 2019
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

u32 byte_count=0;
u32 hardware_id=0;
u8 m_RxBuf_Uart2[50];

bool modbus_flag=0;

union conta numero;


static s32 ReadSerialPort2(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen);
u16 ModRTU_CRC(u8 buf[],u32 len)
{
	u16 crc = 0xFFFF;
	u32 pos,i;

	for(pos = 0; pos < len; pos++)
	{
		crc ^= (u16)buf[pos];          	// XOR byte into least sig. byte of crc

		for (i = 8; i != 0; i--)
		{    			      		// Loop over each bit
			if ((crc & 0x0001) != 0)
			{      					// If the LSB is set
				crc >>= 1;                    		// Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else                            		// Else LSB is not set
			crc >>= 1;                    		// Just shift right
		}
	}
	// Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
	return crc;
}


u8 MODBUS_DATA(u8 device_id,u32 address,u32 no_register)
{
		u32 CRC_TEMP,CRC_Calculation;
		u8 transmit[10];
		s32 totalBytes2;
		int i=0;
		Ql_memset(transmit,0x00,sizeof(transmit));
		memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
		modbus_flag=0;

		//back_TO_uart:

		start:
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);
		time_delay(3);

		/*
		if(Ql_GPIO_GetLevel(PINNAME_PCM_OUT))
		{
			APP_DEBUG("PINNAME_PCM_OUT : 1\r\n");
		}
		else
		{
			APP_DEBUG("PINNAME_PCM_OUT : 0\r\n");
		}
*/


		transmit[0]=device_id;
		transmit[1]=0x03;
		transmit[2]=address>>8; 		//MSB
		transmit[3]=address>>0; 		//LSB
		transmit[4]=0x00;
		transmit[5]=no_register;
		CRC_TEMP=ModRTU_CRC(transmit,6);
		transmit[7]=CRC_TEMP>>8;		//CRC 1
		transmit[6]=CRC_TEMP>>0;		//CRC 2



		/*---------------------------------
		transmit[0]=0x0A;
		transmit[1]=0x03;
		transmit[2]=0x00; 		//MSB
		transmit[3]=0x02; 		//LSB
		transmit[4]=0x00;
		transmit[5]=0x04;
		//CRC_TEMP=ModRTU_CRC(transmit,6);
		//transmit[7]=CRC_TEMP>>8;		//CRC 1
		//transmit[6]=CRC_TEMP>>0;		//CRC 2
		transmit[6] = 0xE4;
		transmit[7] = 0xB2;


		-----------------------------*/



		byte_count=(no_register*2)+5;

		if(!(Ql_GPIO_GetLevel(PINNAME_PCM_OUT)))
		{
			/*
			APP_DEBUG("transmit :");
			for(i=0;i<8;i++)
			{
				APP_DEBUG("%x|",transmit[i]);

			}
			APP_DEBUG("\r\n");
			*/

		Ql_UART_Write(UART_PORT2,transmit,8);
	//	time_delay(2);
		Ql_Sleep(10);
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);


		APP_DEBUG("TX : ");
		for(i=0;i<8;i++)
		{
			APP_DEBUG("%x|",transmit[i]);
		}
		APP_DEBUG("\r\n");



	//	totalBytes2 = ReadSerialPort2(UART_PORT2, m_RxBuf_Uart2,byte_count);

		//APP_DEBUG("transmit[0] : %x\r\n",transmit[0]);
		/*APP_DEBUG("transmit :");
		for(i=0;i<8;i++)
		{
			APP_DEBUG("%x",transmit[i]);

		}
		APP_DEBUG("\r\n");
		time_delay(20);*/
		}
		else
		{
			APP_DEBUG("transmit fail");
		}

		//}


		/*-----------------------*/

         //time_delay(500);
		//for(i=0;i<20;i++)
		{
			//Ql_UART_Read(UART_PORT2, m_RxBuf_Uart2,25);
		}


		//time_delay(1000);


		//START

		/*-------------------------------



		Ql_memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));

		for(totalBytes2=0;totalBytes2<=byte_count;)
		{
			if(Timeout_Flag == 1)
			{
				Timeout_Flag=0;
				break;

			}

			//totalBytes2 = ReadSerialPort2(UART_PORT2, m_RxBuf_Uart2,byte_count);
			i=0;
			if(m_RxBuf_Uart2[1] == device_id )
			{
			for(i=1;i<(byte_count);i++)
				{
					m_RxBuf_Uart2[i-1] = 0;
					m_RxBuf_Uart2[i-1] = m_RxBuf_Uart2[i];
				}
			APP_DEBUG("%02X\r\n",m_RxBuf_Uart2[0]);
			APP_DEBUG("m_RxBuf_Uart2 :");
			for(i=0;i<byte_count;i++)
				{
						APP_DEBUG("%02X",m_RxBuf_Uart2[i]);
				}
			}
			APP_DEBUG("\r\n");

			for(i=0;i<byte_count;i++)
			{
					APP_DEBUG("%02X|",m_RxBuf_Uart2[i]);
			}

			APP_DEBUG("m_RxBuf_Uart2 : %s\r\n",m_RxBuf_Uart2);
			APP_DEBUG("totalBytes2 : %d\r\n",totalBytes2);


			if (totalBytes2 <= 0)
			{
				APP_DEBUG("<-- No data in UART buffer! -->\r\n");

				break;
			}

			if(totalBytes2 == byte_count)
			{
				modbus_flag=1;
				break;

			}
			else
			{
				APP_DEBUG("NO RESPONSE FROM MODBUS\r\n");//remove

				break;
			}
		}


		*/


		//END


		/*do
		{
			Ql_memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
			s32 totalBytes2 = ReadSerialPort2(UART_PORT2, m_RxBuf_Uart2,byte_count);
			if (totalBytes2 <= 0)
			{
				APP_DEBUG("<-- No data in UART buffer! -->\r\n");
				break;
			}

			// Echo
			//Ql_UART_Write(UART_PORT1, m_RxBuf_Uart2, totalBytes);	//remove
			if(totalBytes2 == byte_count)
			{
				modbus_flag=1;

			}
			else
			{
				APP_DEBUG("NO RESPONSE FROM MODBUS\r\n");//remove
				break;
			}

		}while(modbus_flag==0);
		*/



		/*
		if(modbus_flag == 1)
		{
			//APP_DEBUG("device_id : %d\r\n",device_id);

			if(m_RxBuf_Uart2[0]==device_id)	//slave address
			{
				APP_DEBUG("device_id : %d\r\n",device_id);

				CRC_TEMP = ModRTU_CRC(m_RxBuf_Uart2 ,byte_count-2);
				APP_DEBUG("byte_count : %d\r\n",byte_count);
				CRC_Calculation = 0x0000 | m_RxBuf_Uart2[byte_count-1];
				CRC_Calculation = (CRC_Calculation << 8) | m_RxBuf_Uart2[byte_count-2];

				APP_DEBUG("CRC_TEMP : %d\r\nCRC_Calculation : %d\r\n",CRC_TEMP,CRC_Calculation);
				if(CRC_TEMP == CRC_Calculation)
				{
					//flow
					numero.num2[3]=m_RxBuf_Uart2[3];
					numero.num2[2]=m_RxBuf_Uart2[4];

					numero.num2[1]=m_RxBuf_Uart2[5];
					numero.num2[0]=m_RxBuf_Uart2[6];

					flow=numero.num;

					//TOT1
					numero.num2[3]=m_RxBuf_Uart2[7];
					numero.num2[2]=m_RxBuf_Uart2[8];

					numero.num2[1]=m_RxBuf_Uart2[9];
					numero.num2[0]=m_RxBuf_Uart2[10];

					TOT1=numero.num;

					//TOT2
					numero.num2[3]=m_RxBuf_Uart2[11];
					numero.num2[2]=m_RxBuf_Uart2[12];

					numero.num2[1]=m_RxBuf_Uart2[13];
					numero.num2[0]=m_RxBuf_Uart2[14];

					TOT2=numero.num;

					// hardware_id
					numero.num2[3]=m_RxBuf_Uart2[15];
					numero.num2[2]=m_RxBuf_Uart2[16];

					numero.num2[1]=m_RxBuf_Uart2[17];
					numero.num2[0]=m_RxBuf_Uart2[18];

					hardware_id = numero.num;//numero.num1;

					//unit
					numero.num2[3]=m_RxBuf_Uart2[19];
					numero.num2[2]=m_RxBuf_Uart2[20];

					numero.num2[1]=m_RxBuf_Uart2[21];
					numero.num2[0]=m_RxBuf_Uart2[22];

					unit=numero.num;

					//remove
					APP_DEBUG("flow:%f\r\n",flow);
					APP_DEBUG("TOT1:%f\r\n",TOT1);
					APP_DEBUG("TOT2:%f\r\n",TOT2);
					APP_DEBUG("hardware_id:%d\r\n",hardware_id);
					APP_DEBUG("unit:%d\r\n",unit);

					if(connection_est == 1)
					send_flag=1;

					else
					{
						send_flag=0;
						IOT_DATA();
						write_fmeter_memory();

					}
				}
				//remove
				else
				{
					APP_DEBUG("crc not match\r\n");	//remove
					goto start;
				}
			}


		}

*/

		return 0;
}

static s32 ReadSerialPort2(Enum_SerialPort port, /*[out]*/u8* pBuffer2, /*[in]*/u32 bufLen2)
{
    s32 rdLen2 = 0;
    s32 rdTotalLen2 = 0;
    int  i =0;
    if (NULL == pBuffer2 || 0 == bufLen2)
    {
        return -1;
    }
    Ql_memset(pBuffer2, 0x0, bufLen2);
    while (1)
    {
        rdLen2 = Ql_UART_Read(port, pBuffer2 + rdTotalLen2, bufLen2 - rdTotalLen2);
      //  if (rdLen2 <= 0)  // All data is read out, or Serial Port Error!
        if (rdLen2 <= 10)
        {

            break;
        }
        rdTotalLen2 += rdLen2;
        // Continue to read...
    }
    if (rdLen2 < 0) // Serial Port Error!
    {
        APP_DEBUG("Fail to read from port[%d]\r\n", port);
        return -99;
    }
    return rdTotalLen2;
}


u8 MODBUS_TOT_DATA_WRITE(u8 device_id,u32 address,u8 variable)
{
		u32 CRC_TEMP,CRC_Calculation;
		u8 transmit[10];
		s32 totalBytes2;
		int i=0;
		Ql_memset(transmit,0x00,sizeof(transmit));
		memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
		modbus_flag=0;

		//back_TO_uart:

		start:
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);
		time_delay(3);

		transmit[0]=device_id;
		transmit[1]=0x10;
		transmit[2]=address>>8; 		//MSB
		transmit[3]=address>>0; 		//LSB
		transmit[4]=variable>>8; 		//MSB
		transmit[5]=variable>>0; 		//LSB
		transmit[6]=(variable *2);

		value_ftohex_SET_TOT_1();

		APP_DEBUG("hexVals[0] 		: 	%x\n",hexVals[0]);
		APP_DEBUG("hexVals[1] 		: 	%x\n",hexVals[1]);
		APP_DEBUG("hexVals[2] 		: 	%x\n",hexVals[2]);
		APP_DEBUG("hexVals[3] 		: 	%x\n",hexVals[3]);

		transmit[7]= hexVals[3];
		transmit[8]= hexVals[2];
		transmit[9]= hexVals[1];
		transmit[10]= hexVals[0];



		CRC_TEMP=ModRTU_CRC(transmit,11);
		transmit[11]=CRC_TEMP>>0;		//CRC 2
		transmit[12]=CRC_TEMP>>8;		//CRC 1



		Ql_UART_Write(UART_PORT2,transmit,13);
		Ql_Sleep(10);
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);
		APP_DEBUG("transmit :");
		for(i=0;i<13;i++)
		{
			APP_DEBUG("%x|",transmit[i]);
		}
		APP_DEBUG("\r\n");
		RS485_COMM_FAIL = 0;




		time_delay(200);


		/*-------------------------------------------------*/
		address  = address  +	2;
		//variable = variable +	2;

		Ql_memset(transmit,0x00,sizeof(transmit));
		memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
		modbus_flag=0;

			//back_TO_uart:


			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);
			time_delay(3);

			transmit[0]=device_id;
			transmit[1]=0x10;
			transmit[2]=address>>8; 		//MSB
			transmit[3]=address>>0; 		//LSB
			transmit[4]=variable>>8; 		//MSB
			transmit[5]=variable>>0; 		//LSB
			transmit[6]=(variable *2);


			value_ftohex_SET_TOT_2();

			APP_DEBUG("hexVals[0] 		: 	%x\n",hexVals[0]);
			APP_DEBUG("hexVals[1] 		: 	%x\n",hexVals[1]);
			APP_DEBUG("hexVals[2] 		: 	%x\n",hexVals[2]);
			APP_DEBUG("hexVals[3] 		: 	%x\n",hexVals[3]);

			transmit[7]= hexVals[3];
			transmit[8]= hexVals[2];
			transmit[9]= hexVals[1];
			transmit[10]= hexVals[0];

			CRC_TEMP=ModRTU_CRC(transmit,11);
			transmit[11]=CRC_TEMP>>0;		//CRC 2
			transmit[12]=CRC_TEMP>>8;		//CRC 1


			Ql_UART_Write(UART_PORT2,transmit,13);
			Ql_Sleep(10);
			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);
			APP_DEBUG("transmit :");
			for(i=0;i<13;i++)
			{
				APP_DEBUG("%x|",transmit[i]);
			}
			APP_DEBUG("\r\n");
			RS485_COMM_FAIL = 0;



		return 0;
}

u8 MODBUS_RTOT_DATA_WRITE(u8 device_id,u32 address,u8 variable)
{
		u32 CRC_TEMP,CRC_Calculation;
		u8 transmit[10];
		s32 totalBytes2;
		int i=0;
		Ql_memset(transmit,0x00,sizeof(transmit));
		memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
		modbus_flag=0;

		//back_TO_uart:

		start:
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);
		time_delay(3);

		transmit[0]=device_id;
		transmit[1]=0x10;
		transmit[2]=address>>8; 		//MSB
		transmit[3]=address>>0; 		//LSB
		transmit[4]=variable>>8; 		//MSB
		transmit[5]=variable>>0; 		//LSB
		transmit[6]=(variable *2);

		value_ftohex_SET_RTOT_1();

		APP_DEBUG("hexVals[0] 		: 	%x\n",hexVals[0]);
		APP_DEBUG("hexVals[1] 		: 	%x\n",hexVals[1]);
		APP_DEBUG("hexVals[2] 		: 	%x\n",hexVals[2]);
		APP_DEBUG("hexVals[3] 		: 	%x\n",hexVals[3]);

		transmit[7]= hexVals[3];
		transmit[8]= hexVals[2];
		transmit[9]= hexVals[1];
		transmit[10]= hexVals[0];



		CRC_TEMP=ModRTU_CRC(transmit,11);
		transmit[11]=CRC_TEMP>>0;		//CRC 2
		transmit[12]=CRC_TEMP>>8;		//CRC 1



		Ql_UART_Write(UART_PORT2,transmit,13);
		Ql_Sleep(10);
		Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);
		APP_DEBUG("transmit :");
		for(i=0;i<13;i++)
		{
			APP_DEBUG("%x|",transmit[i]);
		}
		APP_DEBUG("\r\n");
		RS485_COMM_FAIL = 0;




		time_delay(200);


		/*-------------------------------------------------*/
		address  = address  +	2;
		//variable = variable +	2;

		Ql_memset(transmit,0x00,sizeof(transmit));
		memset(m_RxBuf_Uart2,0x00,sizeof(m_RxBuf_Uart2));
		modbus_flag=0;

			//back_TO_uart:


			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);
			time_delay(3);

			transmit[0]=device_id;
			transmit[1]=0x10;
			transmit[2]=address>>8; 		//MSB
			transmit[3]=address>>0; 		//LSB
			transmit[4]=variable>>8; 		//MSB
			transmit[5]=variable>>0; 		//LSB
			transmit[6]=(variable *2);


			value_ftohex_SET_RTOT_2();

			APP_DEBUG("hexVals[0] 		: 	%x\n",hexVals[0]);
			APP_DEBUG("hexVals[1] 		: 	%x\n",hexVals[1]);
			APP_DEBUG("hexVals[2] 		: 	%x\n",hexVals[2]);
			APP_DEBUG("hexVals[3] 		: 	%x\n",hexVals[3]);

			transmit[7]= hexVals[3];
			transmit[8]= hexVals[2];
			transmit[9]= hexVals[1];
			transmit[10]= hexVals[0];

			CRC_TEMP=ModRTU_CRC(transmit,11);
			transmit[11]=CRC_TEMP>>0;		//CRC 2
			transmit[12]=CRC_TEMP>>8;		//CRC 1


			Ql_UART_Write(UART_PORT2,transmit,13);
			Ql_Sleep(10);
			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);
			APP_DEBUG("transmit :");
			for(i=0;i<13;i++)
			{
				APP_DEBUG("%x|",transmit[i]);
			}
			APP_DEBUG("\r\n");
			RS485_COMM_FAIL = 0;



		return 0;
}
