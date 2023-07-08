/*
 * file_system.c
 *
 *  Created on: May 9, 2019
 *      Author: SW
 */

#include "file_system.h"
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
#include "file_system.h"



/**************************************************************************************************************/
u32 writenLen = 0;
s32 position;
s32 filesize;
s32 filehandle;
u32 writeedlen, readedlen ;




/**************************************************************************************************************/
#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define DEBUG_PORT  UART_PORT1
#define DBG_BUF_LEN   512
static char DBG_BUFFER[DBG_BUF_LEN];
#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DBG_BUFFER, 0, DBG_BUF_LEN);\
    Ql_sprintf(DBG_BUFFER,FORMAT,##__VA_ARGS__); \
    if (UART_PORT2 == (DEBUG_PORT)) \
    {\
        Ql_Debug_Trace(DBG_BUFFER);\
    } else {\
        Ql_UART_Write((Enum_SerialPort)(DEBUG_PORT), (u8*)(DBG_BUFFER), Ql_strlen((const char *)(DBG_BUFFER)));\
    }\
}
#else
#define APP_DEBUG(FORMAT,...)
#endif
/************************************************************************************************/





//Ql_FS_Read(s32 fileHandle, u8 *readBuffer, u32 numberOfBytesToRead, u32 *numberOfBytesRead);
void file_system_write(u8 *fs_str,s32 file_pointer,u32 fs_length)
{
	//s32 File_Ret;
	File_Ret=Ql_FS_Seek(filehandle, file_pointer , QL_FS_FILE_BEGIN);
	// write to file
	File_Ret=Ql_FS_Write(filehandle,fs_str,fs_length,&writenLen);

}


u32 file_system_read(u8 *fs_response,s32 file_pointer,u32 fs_length)
{
	//s32 File_Ret;
	// move pointer to apn1 addr and read from file
	File_Ret=Ql_FS_Seek(filehandle, file_pointer , QL_FS_FILE_BEGIN);
	File_Ret = Ql_FS_Read(filehandle, (void*)fs_response, fs_length, &readedlen);
	return readedlen;

}
