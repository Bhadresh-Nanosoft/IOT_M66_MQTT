/*
 * sms.h
 *
 *  Created on: Jun 1, 2019
 *      Author: SW
 */

#ifndef SMS_H_
#define SMS_H_

extern bool SMS_Initialize(void);
extern void SMS_TextMode_Read(u32);
extern void SMS_READ(void);
extern bool SMS_Delete(void);

extern bool ADMIN_NO_SMS(u8 *);
extern bool PASS_SMS(u8 *);
extern bool APN_SMS(u8 *);
extern bool PORT_SMS(u8 *);
extern bool IP_SMS(u8 *);
extern bool DNS_SMS(u8 *);
extern void FOTA_SMS(u8 *);
extern bool INTERVAL_SMS(u8 *);
extern bool NO_OF_HW_SMS(u8 *);
extern bool NO_OF_HW_CONFIG(u8 *);
extern void FOTA_SMS(u8 *);
extern void SEND_SMS(u32,bool);
extern bool TIME_INTERVAL_CONFIG(u8 *);
extern bool SET_TOT_SMS(u8 *);
#endif /* SMS_H_ */
