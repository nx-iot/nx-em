#include "gsm.h"
#include "main.h"

extern data_t data_gsm;

gps_t gps;
gps_str_t gps_str;

int8_t gsm_read(uint8_t *value, uint16_t *len){
	int8_t ret = 0;
	
	ret = uart1_read(value,len,255);
	uart1_flush();
	return ret;
}

int8_t gsm_write(uint8_t *value, uint16_t len){
	int8_t ret = 0;
	
	ret = uart1_write(value, len);
	return ret;
}

int8_t gsm_connect(){
	int8_t ret = 0;
	
	return ret; 
}

int8_t gsm_read_signal(uint8_t * val){
	int8_t ret = 0;
	uint8_t gsm_sig = 0;
	char *token = NULL;
	char str_sig_buff[4];
	int i = 0;
	
	HAL_Delay(1000);
	while(SsendATcommand((uint8_t*)"+++\r\n",5,(uint8_t*)"\r\nCLOSED\r\n",10,800)==0);
	HAL_Delay(1000);
	
	print_debug(0,"(GSM Signal)AT+CSQ\r\n");
	ret = gsm_write((uint8_t*)"AT+CSQ\r\n",8);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\n+CSQ: ",8)==0){
				
				token = strstr((const char*)&data_gsm.value[7],(const char*)" ");
				if(token!=NULL){
					print_debug(0,"GSM Signal(%s) is OK.\r\n",(token+1));
					memset(str_sig_buff,0,sizeof(str_sig_buff));
					for(i=0;i<4;i++){
						if(*((token+1)+i) != ','){
							str_sig_buff[i] = *((token+1)+i);
						}else{
							break;
						}
					}
					print_debug(0,"GSM Signal(%s) is OK.\r\n",str_sig_buff);
					gsm_sig = atoi(str_sig_buff);
					print_debug(0,"GSM Signal uint(%d) is OK.\r\n",gsm_sig);
					memcpy(val,&gsm_sig,sizeof(uint8_t));
					
					HAL_Delay(1000);
					
					if(SsendATcommand((uint8_t*)"ATO\r\n",5,(uint8_t*)"\r\nCONNECT\r\n",11,1000)==0)
					{
						Test_Con();
					}
					HAL_Delay(1000);
					ret = 1;
				}
				
			}
		}
	}
	
	return ret;
}



int8_t gsm_read_gps(gps_t *gps_p,gps_str_t *gps_str_p)
{
	int8_t ret = 0;
	char *token;
	int i = 0;
	
	
//	print_debug(0,"(Powers GPS)AT+CGPSPWR=1\r\n");
//	ret = gsm_write((uint8_t*)"AT+CGPSPWR=1\r\n",14);
//	HAL_Delay(1000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Powers GPS) is OK.\r\n");
//			}
//		}
//	}
//	
//	print_debug(0,"(Set Reset mode)AT+CGPSRST=0\r\n");
//	ret = gsm_write((uint8_t*)"AT+CGPSRST=0\r\n",14);
//	HAL_Delay(1000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Set Reset mode) is OK.\r\n");
//			}
//		}
//	}
//	
//	//HAL_Delay(1000);
//	//print_debug(0,"(Set Reset mode)AT+CGPSRST=0\r\n");
//	ret = gsm_write((uint8_t*)"AT+CGPSSTATUS?\r\n",16);
//	HAL_Delay(1000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){

//		print_debug(0,"(Set Reset mode)-------------------------->%s.\r\n",data_gsm.value);
//	}
	

	
	
	memset(gps_str_p,0,sizeof(gps_str_t));
	print_debug(0,"(Get GPS)AT+CGPSINF=0\r\n");
	ret = gsm_write((uint8_t*)"AT+CGPSINF=0\r\n",15);
	HAL_Delay(1000);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			
			
			//Mode
			token = strtok((char*)data_gsm.value,",");
			if(token != NULL){
				print_debug(0,"(Get GPS) Mode => (%s).\r\n",token);
			}else{
				return 0;
			}
			
			//Longitude
			token = strtok(NULL, ",");
			if(token != NULL){
				print_debug(0,"(Get GPS) Longitude => (%s)(len:%d).\r\n",token,strlen(token));
				memcpy(&gps_str_p->longitude[0],token,strlen(token));
				print_debug(0,"(Get GPS) Longitude => (%s).\r\n",gps_str_p->longitude);
			}else{
				return 0;
			}
			
			//Latitude
			token = strtok(NULL, ",");
			if(token != NULL){
				print_debug(0,"(Get GPS) Latitude => (%s)(len:%d).\r\n",token),strlen(token);
				memcpy(&gps_str_p->latitude[0],token,strlen(token));
				print_debug(0,"(Get GPS) Latitude => (%s).\r\n",gps_str_p->latitude);
			}else{
				return 0;
			}
			
			//Altitude
			token = strtok(NULL, ",");
			if(token != NULL){
				print_debug(0,"(Get GPS) Altitude => (%s).\r\n",token);
			}else{
				return 0;
			}
			
			//UTC time
			token = strtok(NULL, ",");
			if(token != NULL){
				print_debug(0,"(Get GPS) UTC time => (%s)(len:%d).\r\n",token,strlen(token));
				memcpy(&gps_str_p->utc_time[0],token,strlen(token));
				print_debug(0,"(Get GPS) UTC time => (%s).\r\n",gps_str_p->utc_time);
			}else{
				return 0;
			}
			
			ret = 1;
			
		}
	}
	
//	Send_Topic = (uint8_t *)"em001/Data/GPS";	
//	Send_Topic_len = 14;
//	Send_payload = (uint8_t *)data_gsm.value;
//	Send_payload_len = data_gsm.length;
//	
	return ret;
}

int8_t ReadGPSExt(uint8_t *value, uint16_t *len)
{
	int8_t ret = 0;
	
	ret = uart2_read(value,len,512);
	uart2_flush();
	return ret;
}
