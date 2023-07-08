/*
 * MODBUS.h
 *
 *  Created on: Aug 9, 2019
 *      Author: SW
 */

#ifndef MODBUS_H_
#define MODBUS_H_

extern u32 byte_count;
extern u32 hardware_id;
extern u8 m_RxBuf_Uart2[50];
extern bool modbus_flag;
extern u8 slave_id;

extern u16 ModRTU_CRC(u8 buf[],u32 len);
//extern u8 MODBUS_DATA(u8,u32,u32);
extern u8 MODBUS_TOT_DATA_WRITE(u8 device_id,u32 address,u8 variable);
extern u8 MODBUS_RTOT_DATA_WRITE(u8 device_id,u32 address,u8 variable);




union conta
{
	float num;
	u32 num0;
	unsigned long int num1;
	u8 num2[4];
};
extern union conta numero;
#endif /* MODBUS_H_ */
