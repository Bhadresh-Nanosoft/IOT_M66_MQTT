/*
 * function.h
 *
 *  Created on: Jul 30, 2019
 *      Author: SW
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_


extern void time_delay(u32);
extern void SYNC_UPDATE(void);
extern u32 DATE_TIME(void);
extern void READ_MEMORY(void);
extern void GET_MODBUS_DATA(void);

// AT Handler
extern s32 ATResponse_Handler(unsigned char*, unsigned int, void*);
extern s32 AT_MSG_Handler(unsigned char*, u32, unsigned char*);
extern s32 ATRsp_COPS_Handler(unsigned char* , u32, void*);
extern s32 ATSwitch_Handler(unsigned char* line, unsigned int len, void* userdata);


extern s32 MAIN_UART_SOCKET_READ(u32,u8 *);

extern void publish_history_data(u32);
extern void time_delay_micro_sec(u32 delay_cnt);


extern void value_ftohex_SET_TOT_1();
extern void value_ftohex_SET_TOT_2();

extern void value_ftohex_SET_RTOT_1();
extern void value_ftohex_SET_RTOT_2();


#endif /* FUNCTION_H_ */
