#ifndef GSM_H
#define GSM_H

#include "main.h"


typedef struct{
	double longitude;
	double latitude;
	double utc_time;
}gps_t;

typedef struct{
	char longitude[20];
	char latitude[20];
	char utc_time[20];
}gps_str_t;


int8_t gsm_read(uint8_t *value, uint16_t *len);
int8_t gsm_write(uint8_t *value, uint16_t len);
int8_t gsm_read_signal(uint8_t * val);
int8_t convert_char2uint(char val , uint8_t *out_val);
int8_t gsm_read_gps(gps_t *gps_p,gps_str_t *gps_str);
int8_t ReadGPSExt(uint8_t *value, uint16_t *len);

extern gps_t gps;
extern gps_str_t gps_str;



#endif 