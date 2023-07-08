/*
 * watchdog_task.h
 *
 *  Created on: Aug 5, 2019
 *      Author: SW
 */

#ifndef WATCHDOG_TASK_H_
#define WATCHDOG_TASK_H_


#define LOGIC_WTD1_TMR_ID  		(TIMER_ID_USER_START + 1)
#define TIMEOUT_TMR_ID			(TIMER_ID_USER_START + 2)
#define ONE_SEC_TMR_ID			(TIMER_ID_USER_START + 3)
#define MSG_ID_USER_DATA         MSG_ID_USER_START+0x100
#define MSG_ID_USER_RS485		 MSG_ID_USER_START+0x101



extern void Callback_Timer(u32, void*);
#endif /* WATCHDOG_TASK_H_ */
