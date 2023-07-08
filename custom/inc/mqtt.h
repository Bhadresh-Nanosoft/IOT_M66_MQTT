/*
 * mqtt.h
 *
 *  Created on: Aug 6, 2019
 *      Author: SW
 */

#ifndef MQTT_H_
#define MQTT_H_


extern s32 MQTTConnect(u8 *,u8 *,u8 *,u8 *,u32);
extern s32 MQTTSubscribe(u8 *,u32);
extern s32 MQTTPublish(u32, u8 *,u32);
extern s32 MQTTPing(void);

extern u32 IOT_DATA(void);

extern bool MQTT_CONNECT_FAIL,MQTT_SUB_FAIL,MQTT_PUBLISH_FAIL,MQTT_PING_FAIL;

#define	CLR_MQTT_FLAG			MQTT_CONNECT_FAIL=0;MQTT_SUB_FAIL=0;MQTT_PUBLISH_FAIL=0;MQTT_PING_FAIL=0;
#endif /* MQTT_H_ */
