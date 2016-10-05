#ifndef NETWORK_H
#define NETWORK_H

#include "main.h"

#define NET_MQTT_PTC	(uint8_t)0x10

typedef struct{
	uint8_t ptc;
	uint8_t connected;
}net_t;

int8_t net_run();
void net_ptc(uint8_t ptc);
int8_t net_connect();
void net_callback(uint8_t *topic_val, uint16_t topic_len, uint8_t *payload_val, uint16_t payload_len, uint16_t packet_id);
uint8_t Get_ADC(char*input);
int8_t convert2Degrees(char* input);
uint8_t ReadBett(void);
int8_t convertTime(char* input);
extern int newADC_int;
#endif 