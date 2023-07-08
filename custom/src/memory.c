/*
 * memory.c
 *
 *  Created on: Jul 31, 2019
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
#include "ql_fs.h"
#include "ql_wtd.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_sim.h"
#include "ril_sms.h"
#include "declaration.h"
#include "function.h"
#include "ql_adc.h"
#include "ql_eint.h"
#include "sms.h"
#include "file_system.h"
#include "main.h"


//void write_str_eeprom(char * str, unsigned int start_addr, unsigned char cnt)
//void read_str_eeprom(unsigned char * dat, unsigned int start_addr, unsigned char cnt)

void Write_SAddr_File(u32 start_addr)
{
		u8 memory_str[100];
		u8 addr_array[10];
		File_Ret = Ql_FS_Open("start_addr.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"SAddr\":\"%d\"}",start_addr);
		file_system_write(memory_str,0,Ql_strlen(memory_str));

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

		return;
}

u32 Read_SAddr_File(void)
{
		u8 memory_str[100];
		u8 addr_array[10];
		u32 start_addr;

		File_Ret = Ql_FS_Open("start_addr.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory

		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		readedlen=0;
		readedlen=file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"SAddr\"");


		if(readedlen == 0 || (!p_res))
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"SAddr\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,50);
		}

		p_res=Ql_strstr(response,"{\"SAddr\"");
		p_res+=5+5;

		Ql_memset(addr_array,0x00,sizeof(addr_array));

		for(length=0;length<50;length++)
		{
			if(*p_res == '"')
			{
				addr_array[length]=0x00;
				start_addr=Ql_atoi(addr_array);
				break;
			}
			addr_array[length]=*p_res++;
		}

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

		return start_addr;
}



void Write_EAddr_File(u32 end_addr)
{
		u8 memory_str[100];
		u8 addr_array[10];
		File_Ret = Ql_FS_Open("end_addr.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"EAddr\":\"%d\"}",end_addr);
		file_system_write(memory_str,0,Ql_strlen(memory_str));

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

}

u32 Read_EAddr_File(void)
{
		u8 memory_str[100];
		u8 addr_array[10];
		u32 end_addr;

		File_Ret = Ql_FS_Open("end_addr.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory

		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		readedlen=0;
		readedlen=file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"EAddr\"");


		if(readedlen == 0 || (!p_res))
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"EAddr\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,50);
		}

		p_res=Ql_strstr(response,"{\"EAddr\"");
		p_res+=5+5;

		Ql_memset(addr_array,0x00,sizeof(addr_array));

		for(length=0;length<50;length++)
		{
			if(*p_res == '"')
			{
				addr_array[length]=0x00;
				end_addr=Ql_atoi(addr_array);
				break;
			}
			addr_array[length]=*p_res++;
		}

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

		return end_addr;



}
void fmeter_data_File(bool rw_flag,u8 *str,u32 start_addr,u32 cnt)
{
		u8 memory_str[200];
		File_Ret = Ql_FS_Open("fmeter.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));


		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(str,start_addr,cnt);
			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			for(u32 memory_length=0;memory_length<cnt;memory_length++)
			{
				memory_str[memory_length]=*str++;
			}
			APP_DEBUG("length:%d\r\n",Ql_strlen(memory_str));
			file_system_write(memory_str,start_addr,Ql_strlen(memory_str));
		}
		Ql_FS_Close(filehandle);
		return;
}
//rw_flag = 1 read
//rw_flag = 0 write

void Admin_Number_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("admin_number.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));


		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(&response,0,250);
			p_res=Ql_strstr(response,"{\"ADMIN\"");

			if(!p_res)	//write header field
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"ADMIN\":\"7046960991\"}");
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}

			p_res=Ql_strstr(response,"{\"ADMIN\"");
			p_res+=5+5;

			Ql_memset(Admin_no,0x00,sizeof(Admin_no));

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					Admin_no[length]=0x00;
					//p_res++;
					break;
				}
				Admin_no[length]=*p_res++;
			}




			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"ADMIN\":\"%s\"}",Admin_no);
			file_system_write(memory_str,0,50);
		}
		APP_DEBUG("ADMIN_NO:%s\r\n",Admin_no);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);
		return;
}

void Number_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("number.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(&response,0,250);
			p_res=Ql_strstr(response,"{\"NUMBER\"");

			if(!p_res)	//write header field
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(strmsg,"{\"NUMBER\":\"7046960991\"}");
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}

			p_res=Ql_strstr(response,"{\"NUMBER\"");
			p_res+=6+5;


			Ql_memset(Number,0x00,sizeof(Number));

			for(length=0;length<50;length++)
			{
				if(*p_res == LIB_SMS_CHAR_COMMA || *p_res == '}')
				{
					Number[length]=0x00;
					//p_res++;
					break;
				}
				Number[length]=*p_res++;
			}

			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"NUMBER\":\"%s\"}",Number);
			file_system_write(memory_str,0,50);

		}

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);
		return;
}
void APN_File(bool rw_flag)
{
		u8 memory_str[200];
		u8 str_cmp[10];

		File_Ret = Ql_FS_Open("apn.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}
		Ql_memset(response,0x00,sizeof(response));
		Ql_memset(str_cmp,0x00,sizeof(str_cmp));




		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(&response,0,250);
			//p_res=Ql_strstr(response,"{\"APN\"");
			p_res=Ql_strstr(response,"{\"1\"");

			if(!p_res)	//write header field
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				//Ql_sprintf(memory_str,"{\"APN\":\"AIRTELIOT.COM\"}");
				Ql_sprintf(memory_str,"{\"1\":\"AIRTELIOT.COM\",\"2\":\"WWW\",\"3\":\"m2misafe\"}");
				//Ql_sprintf(memory_str,"{\"APN\":\"internet\"}");
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}

			Ql_sprintf(str_cmp,"\"%d\"",APN_RUN_COUNTER);

			p_res=Ql_strstr(response,str_cmp);
			if(APN_RUN_COUNTER>9)
			{
				p_res+=2+4;
			}
			{
				p_res+=1+4;
			}

			Ql_memset(APN,0x00,sizeof(APN));

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					APN[length]=0x00;
					//p_res++;
					break;
				}
				APN[length]=*p_res++;
			}

			//APN_CHANGE();


			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			//Ql_sprintf(memory_str,"{\"APN\":\"%s\"}",APN);
			Ql_sprintf(memory_str,"%s",APN);
			file_system_write(memory_str,0,50);
		}
		APP_DEBUG("APN:%s\r\n",APN);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);
		return;
}


void PORT_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("port.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}
		Ql_memset(response,0x00,sizeof(response));


		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(&response,0,250);
			p_res=Ql_strstr(response,"{\"PORT\"");

			if(!p_res)	//write header field
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"PORT\":\"1810\"}");		//remove
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}

				p_res=Ql_strstr(response,"{\"PORT\"");
				p_res+=4+5;

				Ql_memset(primary_port,0x00,sizeof(primary_port));

				for(length=0;length<50;length++)
				{
					if(*p_res == '"')
					{
						primary_port[length]=0x00;
						//p_res++;
						break;
					}
					primary_port[length]=*p_res++;
				}

				//Free memory
				Ql_MEM_Free(memory_alloc);
				p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"PORT\":\"%s\"}",primary_port);
			file_system_write(memory_str,0,50);
		}
		APP_DEBUG("PORT:%s\r\n",primary_port);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);
		return;
}

void IP_DNS_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("ip_dns.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}
		Ql_memset(response,0x00,sizeof(response));


		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			file_system_read(&response,0,250);
			p_res=Ql_strstr(response,"{\"IP_DNS\"");

			if(!p_res)	//write header field
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"IP_DNS\":\"35.200.231.106\"}");		//remove
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}

				p_res=Ql_strstr(response,"{\"IP_DNS\"");
				p_res+=6+5;

				Ql_memset(primary_server,0x00,sizeof(primary_server));

				for(length=0;length<50;length++)
				{
					if(*p_res == '"')
					{
						primary_server[length]=0x00;
						//p_res++;
						break;
					}
					primary_server[length]=*p_res++;
				}

				//Free memory
				Ql_MEM_Free(memory_alloc);
				p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"IP_DNS\":\"%s\"}",primary_server);
			file_system_write(memory_str,0,50);
		}
		APP_DEBUG("IP_DNS:%s\r\n",primary_server);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);
		return;
}



//rw_flag = 1 >> read
//rw_flag = 0 >> write
void Password_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("password.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","password.txt",QL_FS_READ_WRITE|QL_FS_CREATE, File_Ret);
		Ql_memset(response,0x00,sizeof(response));


		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen=file_system_read(&response,0,50);
			p_res=Ql_strstr(response,"{\"Password\"");


			if(readedlen == 0 || (!p_res))
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"Password\":\"12345\"}");
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,50);
			}

			p_res=Ql_strstr(response,"{\"Password\"");
			p_res+=8+5;

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					password_str[length]=0x00;
					break;
				}
				password_str[length]=*p_res++;
			}

			//Free memory
				Ql_MEM_Free(memory_alloc);
				p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"Password\":\"%s\"}",password_str);
			file_system_write(memory_str,0,Ql_strlen(memory_str));

		}
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);



}

void INTERVAL_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("interval.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen=file_system_read(&response,0,50);
			p_res=Ql_strstr(response,"{\"Interval\"");


			if(readedlen == 0 || (!p_res))
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"Interval\":\"3600\"}");	//default 20 seconds
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,50);
			}

			p_res=Ql_strstr(response,"{\"Interval\"");
			p_res+=8+5;

			Ql_memset(interval,0x00,sizeof(interval));

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					interval[length]=0x00;
					fmeter_interval=Ql_atoi(interval);
					break;
				}
				interval[length]=*p_res++;
			}

			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{

			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"Interval\":\"%s\"}",interval);
			file_system_write(memory_str,0,Ql_strlen(memory_str));

		}
		APP_DEBUG("fmeter_interval:%d\r\n",fmeter_interval);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);



}

void ERR_File(bool rw_flag)
{
		u8 memory_str[100];
		u8 err_array[4];
		File_Ret = Ql_FS_Open("error.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen=file_system_read(&response,0,50);
			p_res=Ql_strstr(response,"{\"ERR\"");


			if(readedlen == 0 || (!p_res))
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"ERR\":\"0\"}");	//default 20 seconds
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,50);
			}

			p_res=Ql_strstr(response,"{\"ERR\"");
			p_res+=3+5;

			Ql_memset(err_array,0x00,sizeof(err_array));

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					err_array[length]=0x00;
					error=Ql_atoi(err_array);
					break;
				}
				err_array[length]=*p_res++;
			}

			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"ERR\":\"%d\"}",error);
			file_system_write(memory_str,0,Ql_strlen(memory_str));

		}
		APP_DEBUG("error:%d\r\n",error);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);



}

void SMS_File(bool rw_flag)
{
	u8 memory_str[100];
	File_Ret = Ql_FS_Open("sms.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","sms.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));


		file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"SMS\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"SMS\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,50);
		}

		p_res=Ql_strstr(response,"{\"SMS\"");
		p_res+=3+5;

		sms_flag=(*p_res) - 0x30;

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"SMS\":\"%d\"}",sms_flag);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
	}
	APP_DEBUG("sms_flag:%d\r\n",sms_flag);	//remove
	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);

	return;
}

void Config_File(bool rw_flag)
{
	u8 memory_str[100];
	u8 config_array[4];
	File_Ret = Ql_FS_Open("config.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","config.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		file_system_read(&response,0,100);
		p_res=Ql_strstr(response,"{\"CONFIG\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"CONFIG\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,100);
		}

		p_res=Ql_strstr(response,"{\"CONFIG\"");
		p_res+=6+5;

		Ql_memset(config_array,0x00,sizeof(config_array));
		for(length=0;length<50;length++)
		{
			if(*p_res == '"')
			{
				config_array[length]=0x00;
				config_command=Ql_atoi(config_array);
				break;
			}
			config_array[length]=*p_res++;
		}

		//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"CONFIG\":\"%d\"}",config_command);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
	}
	APP_DEBUG("config_command:%d\r\n",config_command);	//remove
	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);
}

//rw_flag = 1 >> read
//rw_flag = 0 >> write
void Update_Parameter_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("update_parameter.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","update_parameter.txt",QL_FS_READ_WRITE|QL_FS_CREATE, File_Ret);
		Ql_memset(response,0,sizeof(response));
		Ql_memset(memory_str,0x00,sizeof(memory_str));

		//write in internal memory
		if(rw_flag == 0)
		{

			Ql_sprintf((char*)memory_str,"{\"UPDATE_FLAG\":\"%d\"},{\"UPDATE_STAT\":\"%d\"}",update_flag,update_stat);
			file_system_write(memory_str,0,Ql_strlen((char*)memory_str));
		}

		// read from internal memory
		else
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen = file_system_read(response,0,250);
			p_res=Ql_strstr((char*)response,"{\"UPDATE_FLAG\"");
			if(readedlen == 0 || !p_res)
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"UPDATE_FLAG\":\"0\"},{\"UPDATE_STAT\":\"0\"}");
				file_system_write(memory_str,0,Ql_strlen((char*)memory_str));
				Ql_memset(response,0,sizeof(response));
				file_system_read(response,0,250);
			}

			// update flag
			//p_res=NULL;
			p_res=Ql_strstr((char*)response,"{\"UPDATE_FLAG\"");
			p_res+=11+5;

			update_flag=(*p_res) - 0x30;
			APP_DEBUG("update_flag:%d\r\n",update_flag);


			// update status
			p_res=NULL;
			p_res=Ql_strstr((char*)response,"{\"UPDATE_STAT\"");
			if(p_res)
			{
				//APP_DEBUG("string found; %s\r\n",response);
				p_res+=11+5;
				update_stat=(*p_res) - 0x30;
				APP_DEBUG("update_stat:%d\r\n",update_stat);
			}

			//Free memory
			Ql_MEM_Free(memory_alloc);
			memory_alloc=p_res=NULL;
		}

		Ql_memset(response,0,sizeof(response));
		Ql_FS_Close(filehandle);

		return;
}

//rw_flag = 1 >> read
//rw_flag = 0 >> write
void Update_File(bool rw_flag)
{
		u8 memory_str[100];
		u8 msg_array[50];
		bool msg_extract_done=0;
		// read or write header
		File_Ret = Ql_FS_Open("update.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","update.txt",QL_FS_READ_WRITE|QL_FS_CREATE, File_Ret);
		Ql_memset(response,0,sizeof(response));
		Ql_memset(memory_str,0x00,sizeof(memory_str));

		if(rw_flag == 0)
		{
			if(upgrade_filename[0] == '#' && upgrade_filename[5] == '#')
			Ql_sprintf(memory_str,"{\"FILE\":\"%s\"}",upgrade_filename);

			else
			{
				Ql_memset(upgrade_filename,0x00,sizeof(upgrade_filename));
				Ql_sprintf(upgrade_filename,"#AIOT#");
				Ql_sprintf(memory_str,"{\"FILE\":\"%s\"}",upgrade_filename);
			}

			file_system_write(memory_str,0,Ql_strlen(memory_str));
		}

		// read from internal memory
		else
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen = file_system_read(&response,0,250);
			p_res=Ql_strstr(response,"{\"FILE\"");

			if(readedlen == 0 || !p_res)
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"FILE\":\"#AIOT#\"}");
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,250);
			}


			// update filename
			p_res=NULL;
			p_res=Ql_strstr(response,"{\"FILE\"");
			if(p_res)
			{
				//APP_DEBUG("string found; %s\r\n",response);
				p_res+=4+5;

				for(length=0;length<50;length++)
				{
					if(*p_res == '"')
					{
						msg_array[length]=0x00;
						msg_extract_done=1;
						break;
					}
					msg_array[length]=*p_res++;
				}


				if(msg_extract_done == 1)
				{
					msg_extract_done=0;
					Ql_memset(upgrade_filename,0x00,10);
					Ql_sprintf(upgrade_filename,msg_array);
				}


			}

			//Free Memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;
		}

		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

		return;



}

void IMSI_File(bool rw_flag)
{
	u8 memory_str[100];
	File_Ret = Ql_FS_Open("imsi.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","password.txt",QL_FS_READ_WRITE|QL_FS_CREATE, File_Ret);
	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		readedlen=0;
		readedlen=file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"IMSI\"");
		Ql_memset(store_imsi,0x00,sizeof(store_imsi));
		if(p_res)
		{
			p_res+=4+5;

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					store_imsi[length]=0x00;
					break;
				}
				store_imsi[length]=*p_res++;
			}
		}

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;


	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"IMSI\":\"%s\"}",Imsi_No);
		file_system_write(memory_str,0,Ql_strlen(memory_str));

	}
	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);

	return;
}

//rw_flag = 1 >> read
//rw_flag = 0 >> write
void Manual_Mode_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("manual_mode.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));
		Ql_memset(memory_str,0x00,sizeof(memory_str));

		// write in memory
		if(rw_flag == 0)
		{
			Ql_sprintf(memory_str,"{\"MANUAL\":\"%d\"}",manual_mode);
			file_system_write(memory_str,0,Ql_strlen(memory_str));

		}

		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen = file_system_read(response,0,250);
			p_res=Ql_strstr((char *)response,"{\"MANUAL\"");

			if(readedlen == 0 || !p_res)	//write
			{
				manual_mode=0;
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf((char *)memory_str,"{\"MANUAL\":\"%d\"}",(manual_mode));
				file_system_write(memory_str,0,Ql_strlen((char *)memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(response,0,250);

			}
			p_res=Ql_strstr((char *)response,"{\"MANUAL\"");
			if(p_res)
			{

				p_res+=6+5;
				manual_mode=(*p_res)-0x30;

			}
			//Free memory
				Ql_MEM_Free(memory_alloc);
				p_res=NULL;
		}


		APP_DEBUG("manual_mode:%d\r\n",manual_mode);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);

		return;
}


void TOT_num_of_fm_File(bool rw_flag)
{
		u8 memory_str[100];
		File_Ret = Ql_FS_Open("TOT_NUM_OF_FM.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
		if(File_Ret >= QL_RET_OK)
		{
			filehandle = File_Ret;
		}

		Ql_memset(response,0x00,sizeof(response));

		// read from internal memory
		if(rw_flag == 1)
		{
			u8 *p_res=NULL,*memory_alloc;

			//Allocate memory
			memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

			readedlen=0;
			readedlen=file_system_read(&response,0,50);
			p_res=Ql_strstr(response,"{\"NOOFHW\"");


			if(readedlen == 0 || (!p_res))
			{
				Ql_memset(memory_str,0x00,sizeof(memory_str));
				Ql_sprintf(memory_str,"{\"NOOFHW\":\"1\"}");	//default 20 seconds
				file_system_write(memory_str,0,Ql_strlen(memory_str));
				Ql_memset(response,0x00,sizeof(response));
				file_system_read(&response,0,50);
			}

			p_res=Ql_strstr(response,"{\"NOOFHW\"");
			p_res+=6+5;

			Ql_memset(NO_OF_HW,0x00,sizeof(NO_OF_HW));

			for(length=0;length<50;length++)
			{
				if(*p_res == '"')
				{
					NO_OF_HW[length]=0x00;
					TOT_DEV_ID=Ql_atoi(NO_OF_HW);
					break;
				}
				NO_OF_HW[length]=*p_res++;
			}

			//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

		}

		else
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"NOOFHW\":\"%d\"}",TOT_DEV_ID);
			file_system_write(memory_str,0,Ql_strlen(memory_str));

		}
		APP_DEBUG("NO_OF_HW:%d\r\n",TOT_DEV_ID);	//remove
		Ql_memset(response,0x00,sizeof(response));
		Ql_FS_Close(filehandle);



}


void APN_TOT_COUNT_File(bool rw_flag)
{
	u8 memory_str[100];
	File_Ret = Ql_FS_Open("APN_TOT_COUNT.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","sms.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));


		file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"APNTOT\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"APNTOT\":\"3\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,50);
		}

		p_res=Ql_strstr(response,"{\"APNTOT\"");
		p_res+=6+5;

		APN_TOT_COUNTER=(*p_res) - 0x30;

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"APNTOT\":\"%d\"}",APN_TOT_COUNTER);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
	}
	APP_DEBUG("APN_TOT_COUNTER:%d\r\n",APN_TOT_COUNTER);	//remove
	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);

	return;
}

void APN_RUN_COUNT_File(bool rw_flag)
{
	u8 memory_str[100];
	File_Ret = Ql_FS_Open("APN_RUN_COUNT.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","sms.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));


		file_system_read(&response,0,50);
		p_res=Ql_strstr(response,"{\"APNCOUNT\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"APNCOUNT\":\"1\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,50);
		}

		p_res=Ql_strstr(response,"{\"APNCOUNT\"");
		p_res+=8+5;

		APN_RUN_COUNTER=(*p_res) - 0x30;

		//Free memory
		Ql_MEM_Free(memory_alloc);
		p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"APNCOUNT\":\"%d\"}",APN_RUN_COUNTER);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
	}
	APP_DEBUG("APN_RUN_COUNTER:%d\r\n",APN_RUN_COUNTER);	//remove
	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);

	return;
}


void PRODUCT_File(bool rw_flag)
{
	u8 memory_str[100];
	u8 config_array[4];
	File_Ret = Ql_FS_Open("product.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","config.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		file_system_read(&response,0,100);
		p_res=Ql_strstr(response,"{\"PRODUCT\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"PRODUCT\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,100);
		}

		p_res=Ql_strstr(response,"{\"PRODUCT\"");
		p_res+=7+5;

		Ql_memset(config_array,0x00,sizeof(config_array));
		for(length=0;length<50;length++)
		{
			if(*p_res == '"')
			{
				config_array[length]=0x00;
				PRODUCT=Ql_atoi(config_array);
				break;
			}
			config_array[length]=*p_res++;
		}

		//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"PRODUCT\":\"%d\"}",PRODUCT);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
		APP_DEBUG("memory_str :%s\r\n ",memory_str);
	}

	if(PRODUCT == 1)
	{
		APP_DEBUG("PRODUCT:PIZEO :%d\r\n ",PRODUCT);	//remove
	}
	else if(PRODUCT == 0)
	{
		APP_DEBUG("PRODUCT:EFM : %d \r\n",PRODUCT);	//remove
	}


	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);
}


void SMS_CLEAR_File(bool rw_flag)
{
	u8 memory_str[100];
	u8 config_array[4];
	File_Ret = Ql_FS_Open("sms_clear.txt",QL_FS_READ_WRITE|QL_FS_CREATE);
	if(File_Ret >= QL_RET_OK)
	{
		filehandle = File_Ret;
	}

	//APP_DEBUG("Ql_FS_OpenCreate(%s,%08x)=%d\r\n\r\n","config.txt",QL_FS_READ_WRITE|QL_FS_CREATE, iRet);

	Ql_memset(response,0x00,sizeof(response));


	// read from internal memory
	if(rw_flag == 1)
	{
		u8 *p_res=NULL,*memory_alloc;

		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

		file_system_read(&response,0,100);
		p_res=Ql_strstr(response,"{\"SMS_CLEAR\"");

		if(!p_res)
		{
			Ql_memset(memory_str,0x00,sizeof(memory_str));
			Ql_sprintf(memory_str,"{\"SMS_CLEAR\":\"0\"}");
			file_system_write(memory_str,0,Ql_strlen(memory_str));
			Ql_memset(response,0x00,sizeof(response));
			file_system_read(&response,0,100);
		}

		p_res=Ql_strstr(response,"{\"SMS_CLEAR\"");
		p_res+=9+5;

		Ql_memset(config_array,0x00,sizeof(config_array));
		for(length=0;length<50;length++)
		{
			if(*p_res == '"')
			{
				config_array[length]=0x00;
				SMS_CLEAR=Ql_atoi(config_array);
				break;
			}
			config_array[length]=*p_res++;
		}

		//Free memory
			Ql_MEM_Free(memory_alloc);
			p_res=NULL;

	}

	else
	{
		Ql_memset(memory_str,0x00,sizeof(memory_str));
		Ql_sprintf(memory_str,"{\"SMS_CLEAR\":\"%d\"}",SMS_CLEAR);
		file_system_write(memory_str,0,Ql_strlen(memory_str));
	}

	if(SMS_CLEAR == 1)
	{
		APP_DEBUG("NOT SMS_CLEAR\r\n");	//remove
	}
	else if(SMS_CLEAR == 0)
	{
		APP_DEBUG("SMS_CLEAR\r\n");	//remove
	}


	Ql_memset(response,0x00,sizeof(response));
	Ql_FS_Close(filehandle);
}
