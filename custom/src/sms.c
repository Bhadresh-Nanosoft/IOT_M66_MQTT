/*
 * sms.c
 *
 *  Created on: Jun 1, 2019
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
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_sim.h"
#include "ril_sms.h"
#include "ril_telephony.h"
#include "declaration.h"
#include "function.h"
#include "ql_adc.h"
#include "ql_eint.h"
#include "sms.h"
#include "lib_ril_sms.h"
#include "main.h"

/******************************************************************************************/
bool SMS_Initialize(void)
{
    s32 iResult = 0;


    // Set SMS storage:
    // By default, short message is stored into SIM card. You can change the storage to ME if needed, or
    // you can do it again to make sure the short message storage is SIM card.

    // comment by christy because it is giving error
   /* #if 0
    {
        iResult = RIL_SMS_SetStorage(RIL_SMS_STORAGE_TYPE_SM,&nUsed,&nTotal);
        if (RIL_ATRSP_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to set SMS storage, cause:%d\r\n", iResult);
            return FALSE;
        }
        APP_DEBUG("<-- Set SMS storage to SM, nUsed:%u,nTotal:%u -->\r\n", nUsed, nTotal);

        iResult = RIL_SMS_GetStorage(&nCurrStorage, &nUsed ,&nTotal);
        if(RIL_ATRSP_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to get SMS storage, cause:%d\r\n", iResult);
            return FALSE;
        }
        APP_DEBUG("<-- Check SMS storage: curMem=%d, used=%d, total=%d -->\r\n", nCurrStorage, nUsed, nTotal);
    }
    #endif*/

    // Enable new short message indication
    // By default, the auto-indication for new short message is enalbed. You can do it again to
    // make sure that the option is open.
   // #if 0	//comment by christy
    {
        iResult = Ql_RIL_SendATCmd("AT+CNMI=2,1",Ql_strlen("AT+CNMI=2,1"),NULL,NULL,0);
        if (RIL_AT_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to send \"AT+CNMI=2,1\", cause:%d\r\n", iResult);
           // return FALSE;	//remove comment
        }
       // APP_DEBUG("<-- Enable new SMS indication -->\r\n");
    }
   // #endif	//comment by christy

    //Set SMS Mode to Text Mode	(add by christy)
	//#if 0	//comment by christy
    {
        iResult = Ql_RIL_SendATCmd("AT+CMGF=1",Ql_strlen("AT+CMGF=1"),NULL,NULL,0);
        if (RIL_AT_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to send \"AT+CMGF=1\", cause:%d\r\n", iResult);
           // return FALSE;	//remove comment
        }
       // APP_DEBUG("<-- Enable SMS mode to Text -->\r\n");
    }
  //  #endif	//comment by christy

    //comment by christy
    /*
    // Delete all existed short messages (if needed)
    iResult = RIL_SMS_DeleteSMS(0, RIL_SMS_DEL_ALL_MSG);
    if (iResult != RIL_AT_SUCCESS)
    {
        APP_DEBUG("Fail to delete all messages, iResult=%d,cause:%d\r\n", iResult, Ql_RIL_AT_GetErrCode());
        return FALSE;
    }
    APP_DEBUG("Delete all existed messages\r\n");
    */
    return TRUE;
}

void SMS_TextMode_Read(u32 nIndex)
{
    s32 iResult;
    ST_RIL_SMS_TextInfo *pTextInfo = NULL;
    ST_RIL_SMS_DeliverParam *pDeliverTextInfo = NULL;
    ST_RIL_SMS_SubmitParam *pSubmitTextInfo = NULL;
    //LIB_SMS_CharSetEnum eCharSet = LIB_SMS_CHARSET_GSM;	//comment by christy
    LIB_SMS_CharSetEnum eCharSet = LIB_SMS_CHARSET_IRA;	//add by christy

    pTextInfo = Ql_MEM_Alloc(sizeof(ST_RIL_SMS_TextInfo));
    if (NULL == pTextInfo)
    {
        return;
    }

    Ql_memset(pTextInfo,0x00,sizeof(ST_RIL_SMS_TextInfo));
    iResult = RIL_SMS_ReadSMS_Text(nIndex, eCharSet, pTextInfo);
    if (iResult != RIL_AT_SUCCESS)
    {
        Ql_MEM_Free(pTextInfo);
        APP_DEBUG("< Fail to read PDU SMS, cause:%d >\r\n", iResult);
        sms_read_error=1;
        return;
    }
    if (RIL_SMS_STATUS_TYPE_INVALID == (pTextInfo->status))
    {
        APP_DEBUG("<-- SMS[index=%d] doesn't exist -->\r\n", nIndex);
        sms_read_error=0;
        return;
    }

    // Resolve the read short message
    if (LIB_SMS_PDU_TYPE_DELIVER == (pTextInfo->type))
    {
    	sms_read_error=0;
        pDeliverTextInfo = &((pTextInfo->param).deliverParam);
        APP_DEBUG("<-- Read short message (index:%u) with charset %d -->\r\n", nIndex, eCharSet);

        if(FALSE == pDeliverTextInfo->conPres) //Normal SMS
        {
            APP_DEBUG(
                "short message info: \r\n\tstatus:%u \r\n\ttype:%u \r\n\talpha:%u \r\n\tsca:%s \r\n\toa:%s \r\n\tscts:%s \r\n\tdata length:%u\r\ncp:0,cy:0,cr:0,ct:0,cs:0\r\n",
                    (pTextInfo->status),
                    (pTextInfo->type),
                    (pDeliverTextInfo->alpha),
                    (pTextInfo->sca),
                    (pDeliverTextInfo->oa),
                    (pDeliverTextInfo->scts),
                    (pDeliverTextInfo->length)
           );
        }
        else
        {
            APP_DEBUG(
                "short message info: \r\n\tstatus:%u \r\n\ttype:%u \r\n\talpha:%u \r\n\tsca:%s \r\n\toa:%s \r\n\tscts:%s \r\n\tdata length:%u\r\ncp:1,cy:%d,cr:%d,ct:%d,cs:%d\r\n",
                    (pTextInfo->status),
                    (pTextInfo->type),
                    (pDeliverTextInfo->alpha),
                    (pTextInfo->sca),
                    (pDeliverTextInfo->oa),
                    (pDeliverTextInfo->scts),
                    (pDeliverTextInfo->length),
                    pDeliverTextInfo->con.msgType,
                    pDeliverTextInfo->con.msgRef,
                    pDeliverTextInfo->con.msgTot,
                    pDeliverTextInfo->con.msgSeg
           );
        }

        APP_DEBUG("\r\n\tmessage content:");
        APP_DEBUG("%s\r\n",(pDeliverTextInfo->data));
        APP_DEBUG("\r\n");



        Ql_sprintf(strmsg,pDeliverTextInfo->data,pDeliverTextInfo->length);	//add by christy
        Ql_sprintf(Number,pDeliverTextInfo->oa,Ql_strlen(pDeliverTextInfo->oa));	//add by christy

    }
    else if (LIB_SMS_PDU_TYPE_SUBMIT == (pTextInfo->type))
    {// short messages in sent-list of drafts-list
    }
    else
    {
        APP_DEBUG("<-- Unkown short message type! type:%d -->\r\n", (pTextInfo->type));
    }
    Ql_MEM_Free(pTextInfo);
}

bool SMS_Delete(void)
{
	s32 iResult;
	// Delete all existed short messages (if needed)
	iResult = RIL_SMS_DeleteSMS(0, RIL_SMS_DEL_ALL_MSG);
	if (iResult != RIL_AT_SUCCESS)
	{
		APP_DEBUG("Fail to delete all messages, iResult=%d,cause:%d\r\n", iResult, Ql_RIL_AT_GetErrCode());
		return FALSE;
	}
	APP_DEBUG("Delete all existed messages\r\n");
	SMS_CLEAR = 0;
	return TRUE;	//add by christy
}




void SMS_READ(void)
{

	if((Ql_strstr((char *)strmsg,(char *)password_str)) || (Ql_strstr((char *)strmsg,"94959")))
	{
		if(Ql_strstr((char *)strmsg,"ALERT:"))
		sms_flag=ADMIN_NO_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"PASS:"))
		sms_flag=PASS_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"APN:"))
		sms_flag=APN_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"IP:"))
		sms_flag=IP_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"DNS:"))
		sms_flag=DNS_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"PORT:"))
		sms_flag=PORT_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"INTL:"))
		sms_flag=INTERVAL_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"TOTHW:"))
		sms_flag=NO_OF_HW_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"TOT:"))
		sms_flag=SET_TOT_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,":CMD"))
		FOTA_SMS(strmsg);

		if(Ql_strstr((char *)strmsg,"RESTART:CMD"))
		{
			device_reset=1;
		}



		if(Ql_strstr((char *)strmsg,"PRODUCT:EFM:"))
		{
			APP_DEBUG("2) PRODUCT:EFM:");
			PRODUCT = 0;
			PRODUCT_File(0);
		}

		if(Ql_strstr((char *)strmsg,"PRODUCT:PIZEO:"))
		{
			PRODUCT = 1;
			PRODUCT_File(0);
		}

		if(Ql_strstr((char *)strmsg,"PRODUCT:PIEZO:"))
		{
			PRODUCT = 1;
			PRODUCT_File(0);
		}


	}

	else if(Ql_strstr((char *)strmsg,"Getinfo") || Ql_strstr((char *)strmsg,"GETINFO") || Ql_strstr((char *)strmsg,"getinfo"))
	{

		//remove 7069897890,8849645365
		//if(Ql_strstr(Number,"7069897890") || Ql_strstr(Number,"8849645365") || Ql_strstr(Number,"9909919095") || Ql_strstr(Number,"9033520529") || Ql_strstr(Number,"9737561301") || Ql_strstr(Number,"8849670588") || Ql_strstr(Number,"7046960991"))
		{
			sms_flag=1;
			config_command=9;
			Number_File(0);
		}

	}


	if(sms_flag == 1)
	{
		SMS_File(0);
		Config_File(0);
	}


}

bool ADMIN_NO_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 temp_admin_no[20];
	u32 number;
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"ALERT:");
	p_res+=6;


	Ql_memset(temp_admin_no,0x00,sizeof(temp_admin_no));


	if(*p_res != LIB_SMS_CHAR_COMMA || *p_res != LIB_SMS_CHAR_COLON)
	{

		for(length=0;length<50;length++)
		{
			if(*p_res == LIB_SMS_CHAR_COLON)
			{

				temp_admin_no[length]=0x00;
				msg_extract_done=1;
				break;
			}
			else
			temp_admin_no[length]=*p_res++;
		}

	}


	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(Admin_no,0x00,sizeof(Admin_no));
		Ql_strcpy(Admin_no,temp_admin_no);

		Admin_Number_File(0);
		sms_flag=1;
		config_command=2;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool PASS_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"PASS:");
	p_res+=5;

	Ql_memset(msg_array,0x00,sizeof(msg_array));


	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(password_str,0x00,sizeof(password_str));
		Ql_strcpy(password_str,msg_array);
		Password_File(0);
		sms_flag=1;
		config_command=1;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}



bool APN_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;
	u8 number = 0;
	u8 number_of_apn = 0;
	u8  l = 0;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"APN:");
	p_res+=4;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	Ql_memset(APN,0x00,sizeof(APN));
	APN[l++] = '{';

		for(number=0;number<8;number++)
		{
			if(*p_res != LIB_SMS_CHAR_COMMA || *p_res != LIB_SMS_CHAR_COLON)
			{

				for(length =0;length<50;length++)
				{
					if(*p_res == LIB_SMS_CHAR_COMMA || *p_res == LIB_SMS_CHAR_COLON)
					{
						//message[length]=0x00;
						msg_array[length]=0x00;
						msg_extract_done=1;
						number_of_apn++;

						Ql_sprintf(&APN[l],"\"%d\":\"%s\",",(number+1),msg_array);
						l = strlen(APN);



						Ql_memset(msg_array,0x00,sizeof(msg_array));
						//*p_res++;
						break;
					}
					else
						msg_array[length]=*p_res++;
				}

				if(*p_res == LIB_SMS_CHAR_COLON)
				{
					msg_extract_done=1;
					break;
				}
				p_res++;

			}
		}

		APN[l-1] = '}';

		APP_DEBUG("APN : %s\n",APN);

		if(msg_extract_done == 1)
		{
			msg_extract_done=0;

			APN_TOT_COUNTER = number_of_apn;
			APN_RUN_COUNTER = 1;

			APN_File(0);

			sms_flag=1;
			config_command=3;
			APN_TOT_COUNT_File(0);


			device_reset=1;

		}

		else
			sms_flag=0;



/*
	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(APN,0x00,sizeof(APN));
		Ql_strcpy(APN,msg_array);
		APN_TOT_COUNTER++;
		APN_File(0);
		sms_flag=1;
		config_command=3;
		device_reset=1;

	}

	else
		sms_flag=0;
*/



	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

/*
bool APN_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"APN:");
	p_res+=4;

	Ql_memset(msg_array,0x00,sizeof(msg_array));


	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(APN,0x00,sizeof(APN));
		Ql_strcpy(APN,msg_array);
		APN_File(0);
		sms_flag=1;
		config_command=3;
		device_reset=1;
		Ql_Reset(0);

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}
*/
bool PORT_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"PORT:");
	p_res+=5;

	Ql_memset(msg_array,0x00,sizeof(msg_array));


	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(primary_port,0x00,sizeof(primary_port));
		Ql_strcpy(primary_port,msg_array);
		PORT_File(0);
		sms_flag=1;
		config_command=4;
		device_reset=1;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool SET_TOT_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"TOT:");
	p_res+=4;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COMMA)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		APP_DEBUG("msg_array : %s\r\n",msg_array);
		Ql_memset(SET_TOT_BUFF,0x00,sizeof(SET_TOT_BUFF));
		Ql_strcpy(SET_TOT_BUFF,msg_array);
		SET_TOT=Ql_atof(SET_TOT_BUFF);
		//SET_TOT_File(0);

		//SET_TOT_2 = SET_TOT/65535;
		//SET_TOT_1 = SET_TOT - (SET_TOT_2 * 65535);
		APP_DEBUG("SET_TOT : %f\r\n",SET_TOT);
		sms_flag=1;
		config_command=6;

		//modbus_write = 1;
		msg_extract_done = 0;
	}

	else
		sms_flag=0;


	for(length=0;length<20;length++)
		{
			if(*p_res == LIB_SMS_CHAR_COLON)
			{
				msg_array[length]=0x00;
				msg_extract_done=1;
				*p_res++;
				break;
			}
			else
				msg_array[length]=*p_res++;
		}



	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(SET_TOT_BUFF,0x00,sizeof(SET_TOT_BUFF));
		Ql_strcpy(SET_TOT_BUFF,msg_array);

		RSET_TOT=Ql_atof(SET_TOT_BUFF);

		APP_DEBUG("RSET_TOT : %f\r\n",RSET_TOT);

		sms_flag=1;
		config_command=6;

		modbus_write = 1;
		msg_extract_done = 0;
	}

	else
		sms_flag=0;






	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}



bool IP_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"IP:");
	p_res+=3;

	Ql_memset(msg_array,0x00,sizeof(msg_array));


	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(primary_server,0x00,sizeof(primary_server));
		Ql_strcpy(primary_server,msg_array);
		IP_DNS_File(0);
		sms_flag=1;
		config_command=5;
		device_reset=1;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool DNS_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"DNS:");
	p_res+=4;

	Ql_memset(msg_array,0x00,sizeof(msg_array));


	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(primary_server,0x00,sizeof(primary_server));
		Ql_strcpy(primary_server,msg_array);
		IP_DNS_File(0);
		sms_flag=1;
		config_command=5;
		device_reset=1;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool INTERVAL_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"INTL:");
	p_res+=5;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(interval,0x00,sizeof(interval));
		Ql_strcpy(interval,msg_array);
		fmeter_interval=Ql_atoi(interval);
		INTERVAL_File(0);
		sms_flag=1;
		config_command=6;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}



bool NO_OF_HW_SMS(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"TOTHW:");
	p_res+=6;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	for(length=0;length<20;length++)
	{
		if(*p_res == LIB_SMS_CHAR_COLON)
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(NO_OF_HW,0x00,sizeof(NO_OF_HW));
		Ql_strcpy(NO_OF_HW,msg_array);
		TOT_DEV_ID=Ql_atoi(NO_OF_HW);
		TOT_num_of_fm_File(0);
		sms_flag=1;
		config_command=6;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool NO_OF_HW_CONFIG(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"\"TOTHW\":\"");
	p_res+=9;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	for(length=0;length<20;length++)
	{
		if(*p_res == '"')
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(NO_OF_HW,0x00,sizeof(NO_OF_HW));
		Ql_strcpy(NO_OF_HW,msg_array);
		TOT_DEV_ID=Ql_atoi(NO_OF_HW);
		TOT_num_of_fm_File(0);
		sms_flag=1;
		config_command=6;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}

bool TIME_INTERVAL_CONFIG(u8 *sms_msg)
{
	u8 *p_res=NULL,*memory_alloc;
	u8 msg_array[20];
	bool msg_extract_done;

	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
	p_res=Ql_strstr(sms_msg,"\"INTL\":\"");
	p_res+=8;

	Ql_memset(msg_array,0x00,sizeof(msg_array));

	for(length=0;length<20;length++)
	{
		if(*p_res == '"')
		{
			msg_array[length]=0x00;
			msg_extract_done=1;
			*p_res++;
			break;
		}
		else
			msg_array[length]=*p_res++;
	}

	if(msg_extract_done == 1)
	{
		msg_extract_done=0;
		Ql_memset(interval,0x00,sizeof(interval));
		Ql_strcpy(interval,msg_array);
		fmeter_interval=Ql_atoi(interval);
		INTERVAL_File(0);
		//sms_flag=1;
		//config_command=6;

	}

	else
		sms_flag=0;

	Ql_MEM_Free(memory_alloc);
	p_res=NULL;
	return sms_flag;
}




void FOTA_SMS(u8 *sms_msg)
{
	u8 msg_array[20];
	bool msg_extract_done;

	if(Ql_strstr(sms_msg,"F:#") && update_stat!=1)
	{
		u8 *p_res=NULL,*memory_alloc;
		//Allocate memory
		memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));
		p_res=Ql_strstr(sms_msg,"F:#");
		p_res+=2;

		Ql_memset(msg_array,0x00,sizeof(msg_array));

		for(length=0;length<20;length++)
		{
			if(*p_res == LIB_SMS_CHAR_COLON)
			{
				msg_array[length]=0x00;
				msg_extract_done=1;
				*p_res++;
				break;
			}
			else
				msg_array[length]=*p_res++;
		}

		if(msg_extract_done == 1)
		{
			msg_extract_done=0;
			if(msg_array[0]=='#' && msg_array[5]=='#')
			{
				Ql_memset(upgrade_filename,0x00,sizeof(upgrade_filename));
				Ql_strcpy(upgrade_filename,msg_array);
				update_flag=1;
				Update_File(0);
				Update_Parameter_File(0);
			}
		}

		Ql_MEM_Free(memory_alloc);
		p_res=NULL;
	}



	return;

}
void SEND_SMS(u32 config_command,bool rs232_config)
{
	//getinfo message
	if(config_command == 9)
	{
		RIL_NW_GetSignalQuality(&gsm_signal, &ber);
		Ql_memset(strImei, 0x0, sizeof(strImei));
		Ql_memset(Imei_No, 0x0, sizeof(Imei_No));
		iRet = RIL_GetIMEI(strImei);
		Ql_memcpy(&Imei_No[0],&strImei[2],15);

		Ql_memset(sms_str,0x00,sizeof(sms_str));
		Ql_sprintf(sms_str,"M:%s#PASS:%s#EI:%s#SI:%s#Q:%d#A:%s#",Admin_no,password_str,Imei_No,Imsi_No,gsm_signal,APN);
		if(primary_server[0] >= 0x30 && primary_server[0] <= 0x39)
		{
			Ql_sprintf(&sms_str[Ql_strlen(sms_str)],"IP:%s#",primary_server);
		}

		else
			Ql_sprintf(&sms_str[Ql_strlen(sms_str)],"DNS:%s#",primary_server);

		Ql_sprintf(&sms_str[Ql_strlen(sms_str)],"P:%s#T:%d#E:%d",primary_port,fmeter_interval,error);
	}

	else
	{
		Ql_memset(sms_str,0x00,sizeof(sms_str));
		Ql_sprintf(sms_str,"Configuration Done.\0");
	}

	if(config_command >=1 && config_command <=9)
	{
		APP_DEBUG("message to send:%s\r\n",sms_str);

		if(config_command == 9)
			iRet = RIL_SMS_SendSMS_Text(Number, Ql_strlen(Number), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), NULL);

		else
			iRet = RIL_SMS_SendSMS_Text(Admin_no, Ql_strlen(Admin_no), LIB_SMS_CHARSET_IRA, sms_str, Ql_strlen(sms_str), NULL);

		if (iRet != RIL_AT_SUCCESS)
		{
			APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iRet, Ql_RIL_AT_GetErrCode());
			return;
		}
		else
		APP_DEBUG("< Send Text SMS successfully >\r\n");
	}

}
/*
void FOTA_SMS(u8 *sms_msg,bool cmd_config)
{
	u8 msg_array[20];
	bool msg_extract_done;

	u8 *p_res=NULL,*memory_alloc;
	//Allocate memory
	memory_alloc=p_res=(u8*)Ql_MEM_Alloc(sizeof(response));

	if(cmd_config == 1)	//config by mqtt server
	{
		p_res=StrStr(sms_msg,"F:#",100);
		p_res+=2;
		msg_extract_done=1;
	}


	else	//config using sms
	{
		if(Ql_strstr(sms_msg,"F:#") && update_stat!=1)
		{
			p_res=Ql_strstr(sms_msg,"F:#");
			p_res+=2;
			msg_extract_done=1;
		}
	}
	if(msg_extract_done == 1)
	{
		msg_extract_done=0;

		Ql_memset(msg_array,0x00,sizeof(msg_array));

		for(length=0;length<20;length++)
		{
			if(*p_res == LIB_SMS_CHAR_COLON)
			{
				msg_array[length]=0x00;
				msg_extract_done=1;
				*p_res++;
				break;
			}
			else
			msg_array[length]=*p_res++;
		}

		if(msg_extract_done == 1)
		{
			msg_extract_done=0;
			if(msg_array[0]=='#' && msg_array[5]=='#')
			{
				Ql_memset(upgrade_filename,0x00,sizeof(upgrade_filename));
				Ql_strcpy(upgrade_filename,msg_array);
				update_flag=1;
				Update_File(0);
				Update_Parameter_File(0);
			}
		}
	}


	Ql_MEM_Free(memory_alloc);
	p_res=NULL;


	return;

}

*/

