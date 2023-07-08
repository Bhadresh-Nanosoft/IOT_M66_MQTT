/*
 * main.h
 *
 *  Created on: Aug 5, 2019
 *      Author: SW
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_sim.h"
#include "ril_sms.h"
#include "ril_telephony.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_gpio.h"
#include "ql_timer.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_memory.h"
#include "ql_fs.h"

#include "watchdog_task.h"



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

/*********************************** HARDWARE PIN OUT *******************************************/

//#define		_read_write		PINNAME_PCM_OUT
//#define		_read			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,1);
//#define		_write			Ql_GPIO_SetLevel(PINNAME_PCM_OUT,0);

#endif /* MAIN_H_ */
