/*
 * memory.h
 *
 *  Created on: Jul 31, 2019
 *      Author: SW
 */

#ifndef MEMORY_H_
#define MEMORY_H_

extern void Admin_Number_File(bool);
extern void BAT_STAT_File(bool);
extern void BAT_ALERT_File(bool);
extern void Number_File(bool);
extern void Update_File(bool);
extern void Update_Parameter_File(bool);
extern void IMSI_File(bool);
extern void APN_File(bool);
extern void INTERVAL_File(bool);
extern void ERR_File(bool);
extern void Manual_Mode_File(bool);

extern void fmeter_data_File(bool,u8 *,u32,u32);

extern void Write_SAddr_File(u32);
extern u32 Read_SAddr_File(void);
extern void Write_EAddr_File(u32);
extern u32 Read_EAddr_File(void);
#endif /* MEMORY_H_ */
