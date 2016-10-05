#include "mqtt_ptc.h"
#include "network.h"
#include "gsm.h"
#include "main.h"

extern data_t data_gsm;
extern net_t net;
mqtt_t mqtt;

uint8_t ctrl_z = 0x1A;
uint8_t mqtt_buf[256];
mqtt_dat_t mqtt_data_rx;

int8_t mqtt_connect(uint8_t *user_val, uint16_t user_len, uint8_t *pwd_val, uint16_t pwd_len, uint8_t *client_id_val, uint16_t client_id_len){
	
	char at_buf[30];
	char *token;
	uint16_t receiv_len = 0;
	uint8_t i = 0;
	int8_t ret = 0;
	uint16_t idx_w = 0;
	char * will_Topic = NULL;
	uint8_t heder_ctl_code = 0x00;
	//
	// Leave room in the buffer for header and variable length field
  uint16_t length = 2;
	
	//unsigned int length = 0;
	uint8_t v;	
	
#if MQTT_VERSION == MQTT_VERSION_3_1
            char d[9] = {0x00,0x06,'M','Q','I','s','d','p', MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 9
#elif MQTT_VERSION == MQTT_VERSION_3_1_1
            char d[7] = {0x00,0x04,'M','Q','T','T',MQTT_VERSION};
#define MQTT_HEADER_VERSION_LENGTH 7
#endif

	memset(mqtt_buf,0,sizeof(mqtt_buf));
	

						
	mqtt_buf[idx_w++] = 0x10;
	idx_w += 1;//reserv len 1 byte
	for (i = 0;i<MQTT_HEADER_VERSION_LENGTH;i++) {
			mqtt_buf[idx_w++] = d[i];
	}
	
	//MQTT_Conncect->will_topic = 0x00; //test null data
	if (will_Topic) {
			v = 0x06|(MQTT_QOS1 << 3)|(MQTT_RETAIN <<5);
	} else {
			v = 0x02;
	}
	
	if(user_len>0) {
			v = v|0x80;

			if(pwd_len>0) {
					v = v|(0x80>>1);
			}
			//v = v&0xFD;
	}
	v = 0xC2;
	
	mqtt_buf[idx_w++] = v;
	mqtt_buf[idx_w++] = ((MQTT_KEEPALIVE) >> 8);
	mqtt_buf[idx_w++] = ((MQTT_KEEPALIVE) & 0xFF);
	
	//Client ID Len
	mqtt_buf[idx_w++] = ((client_id_len) >> 8);
	mqtt_buf[idx_w++] = ((client_id_len) & 0xFF);
	
	//Client ID
	memcpy(&mqtt_buf[idx_w],client_id_val,client_id_len);
	idx_w += client_id_len;
	
	
	//User name len
	mqtt_buf[idx_w++] = ((user_len) >> 8);
	mqtt_buf[idx_w++] = ((user_len) & 0xFF);
	//User Name
	memcpy(&mqtt_buf[idx_w],user_val,user_len);
	idx_w += user_len;
	
	//Password len
	mqtt_buf[idx_w++] = ((pwd_len) >> 8);
	mqtt_buf[idx_w++] = ((pwd_len) & 0xFF);
	//Password
	memcpy(&mqtt_buf[idx_w],pwd_val,pwd_len);
	idx_w += pwd_len;
	
	mqtt_buf[1] = (idx_w - 2);
	
	
	
	mqtt.msg_id = 0;
	
	
//	memset(at_buf,0,sizeof(at_buf));
//	sprintf(at_buf,"AT+CIPSEND=%d\r\n",idx_w);
//	print_debug(0,"(Status)%s\r\n",at_buf);
//	ret = gsm_write((uint8_t*)at_buf,strlen(at_buf));
//	HAL_Delay(1000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\n> ",4)==0){
//				ret = 1;
//			}else{
//				ret = 0;
//			}
//		}else{
//			ret = 0;
//		}
//	}
	
	
//	if(ret){
//		print_debug(0,"Data Init Transmit is OK.\r\n");
//		print_debug(0,"[INFO]MQTT Packet Len(%d):\r\n",idx_w);
//		print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
//		print_debug(0,"\n");
//		ret = gsm_write(mqtt_buf,idx_w);
//		ret = gsm_write(&ctrl_z,1);
//		HAL_Delay(2000);
//		
//		
//		print_debug(0,"(Get data manual)AT+CIPRXGET=1\r\n");
//		ret = gsm_write((uint8_t*)"AT+CIPRXGET=1,256\r\n",19);
//		HAL_Delay(1000);
//		ret = gsm_read(data_gsm.value,&data_gsm.length);
//		if(ret){
//			if(data_gsm.length > 0){
//				print_payload(0,data_gsm.value,data_gsm.length);
//				if(strncmp((char*)data_gsm.value,"\r\nSEND OK\r\n",11)==0){
//					if(data_gsm.value[11] == (uint8_t)0x20){
//						if(data_gsm.value[12] == (uint8_t)2){
//							if((data_gsm.value[13] == (uint8_t)0x00) && (data_gsm.value[14] == (uint8_t)0x00)){
//								print_debug(0,"MQTT connect is OK.\r\n");
//								mqtt.connected = 1;
//								ret = 1;
//							}else{
//								mqtt.connected = 0;
//								ret = 0;
//							}
//						}
//						else{
//								mqtt.connected = 0;
//								ret = 0;
//							}
//					}
//					else{
//								mqtt.connected = 0;
//								ret = 0;
//							}
//					HAL_Delay(1000);
//				}
//				else{
//								mqtt.connected = 0;
//								ret = 0;
//							}
//				
//			}
//		}
//	}
	
	ret = gsm_write(mqtt_buf,idx_w);
	
	HAL_Delay(2000);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			
			if(data_gsm.value[0] == (uint8_t)0x20){
				if(data_gsm.value[1] == (uint8_t)2){
					if((data_gsm.value[2] == (uint8_t)0x00) && (data_gsm.value[3] == (uint8_t)0x00)){
						print_debug(0,"MQTT connect is OK.\r\n");
						while(mqtt_subscribe((uint8_t*)"m001/data/buzzer", (uint16_t)16, (uint8_t)MQTT_QOS1)==0);
						mqtt.connected = 1;
						ret = 1;
					}else{
						mqtt.connected = 0;
						ret = 0;
					}
				}
				else{
						mqtt.connected = 0;
						ret = 0;
					}
			}
			else{
						mqtt.connected = 0;
						ret = 0;
					}
			HAL_Delay(1000);
			
		}
	}
	
	return ret;
	
	//while(1);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nSEND OK\r\n",11)==0){
//				if(data_gsm.value[11] == (uint8_t)0x20){
//					if(data_gsm.value[12] == (uint8_t)2){
//						if((data_gsm.value[13] == (uint8_t)0x00) && (data_gsm.value[14] == (uint8_t)0x00)){
//							print_debug(0,"MQTT connect is OK.\r\n");
//							mqtt.connected = 1;
//						}else{
//							mqtt.connected = 0;
//						}
//					}
//				}
//			}
//		}
//	}
//	
//		
//	return ret;
}


int8_t mqtt_publish(uint8_t *topic_value, uint16_t topic_len, uint8_t *payload_value, uint16_t payload_len, uint8_t qos, uint8_t retain){
	int8_t ret;
	uint16_t idx_w = 0;
	char at_buf[30];
		uint16_t i = 0;
	
	mqtt_buf[idx_w++] = MQTT_PUBLISH|qos;
	//Retain
	if (retain) {
		 mqtt_buf[0] |= 1;
	}
	
	//Length Remain
	mqtt_buf[idx_w++] = 0;
	
	//Topic len
	mqtt_buf[idx_w++] = ((topic_len) >> 8);
	mqtt_buf[idx_w++] = ((topic_len) & 0xFF);
	
	
	//Topic value
	memcpy(&mqtt_buf[idx_w],topic_value,topic_len);
	idx_w += topic_len;
	
	//Message ID or Packet ID
	mqtt_buf[idx_w++] = ((mqtt.msg_id) >> 8);
	mqtt_buf[idx_w++] = ((mqtt.msg_id) & 0xFF);
	mqtt.msg_id++;
	if(mqtt.msg_id>=10000){
		 mqtt.msg_id = 0;
	}
	
	//Payload
	memcpy(&mqtt_buf[idx_w],payload_value,payload_len);
	idx_w += payload_len;
	
	//Update Length Remaining
	mqtt_buf[1] = (idx_w - 2);
	
	print_debug(0,"[INFO]MQTT Publish Packet Len(%d):\n",idx_w);
	print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
	print_debug(0,"\n");
	
	
//	memset(at_buf,0,sizeof(at_buf));
//	sprintf(at_buf,"AT+CIPSEND=%d\r\n",idx_w);
//	print_debug(0,"(Status)%s\r\n",at_buf);
//	ret = gsm_write((uint8_t*)at_buf,strlen(at_buf));
//	HAL_Delay(2000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\n> ",4)==0){
//				ret = 1;
//			}else{
//				ret = 0;
//			}
//		}else{
//			ret = 0;
//		}
//	}
//	
//	if(ret){
//		print_debug(0,"Data Init Transmit is OK.\r\n");
//		print_debug(0,"[INFO]MQTT Packet Len(%d):\r\n",idx_w);
//		print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
//		print_debug(0,"\n");
//		ret = gsm_write(mqtt_buf,idx_w);
//		ret = gsm_write(&ctrl_z,1);
//		HAL_Delay(1000);
//	}
	
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nSEND OK\r\n",11)==0){
//				if(data_gsm.value[11] && (uint8_t)0xF0 == (uint8_t)0x40){
//					if(data_gsm.value[12] == (uint8_t)2){
//						if((data_gsm.value[13] == (uint8_t)0x00) && (data_gsm.value[14] == (uint8_t)0x00)){
//							print_debug(0,"MQTT connect is OK.\r\n");
//							mqtt.connected = 1;
//						}else{
//							mqtt.connected = 0;
//						}
//					}
//				}
//			}
//		}
//	}	

	ret = gsm_write(mqtt_buf,idx_w);
	
	for(i=0;i<4;i++)
	{
		//GetGPS();
		HAL_Delay(500);
	}
//	HAL_Delay(2000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			
//			if(data_gsm.value[0] == (uint8_t)0x40){
//				if(data_gsm.value[1] == (uint8_t)2){
//					print_debug(0,"MQTT Publish is OK.\r\n");
//					mqtt.connected = 1;
//					ret = 1;
//					return ret;
//				}
//				else{
//						mqtt.connected = 0;
//						ret = 0;
//				}
//			}
//			else{
//					mqtt.connected = 0;
//					ret = 0;
//			}
//		}
//		else{
//				mqtt.connected = 0;
//				ret = 0;
//		}
//	}
	
	//ret = 1;
	return ret;
}

int8_t mqtt_subscribe(uint8_t *topic_value, uint16_t topic_len, uint8_t qos){
	int8_t ret;
	uint16_t idx_w = 0;
	uint8_t heder_ctl_code = 0x00;
	char at_buf[30];
	
	//Control Packet
	mqtt_buf[idx_w++] = MQTT_SUBSCRIBE|0x02;
		
	mqtt_buf[idx_w++] = 0;//Remaining Length
		
	//Message ID or Packet ID
	mqtt_buf[idx_w++] = ((mqtt.msg_id) >> 8);
	mqtt_buf[idx_w++] = ((mqtt.msg_id) & 0xFF);
	mqtt.msg_id++;
	if(mqtt.msg_id>=10000){
		 mqtt.msg_id = 0;
	}
	
	//Topic len
	mqtt_buf[idx_w++] = ((topic_len) >> 8);
	mqtt_buf[idx_w++] = ((topic_len) & 0xFF);
	
	
	//Topic value
	memcpy(&mqtt_buf[idx_w],topic_value,topic_len);
	idx_w += topic_len;
	
	//QOS Request
	if(qos == (uint8_t)MQTT_QOS0){
		mqtt_buf[idx_w++] = (uint8_t)0x00;
	}else if(qos == (uint8_t)MQTT_QOS1){
		mqtt_buf[idx_w++] = (uint8_t)0x01;
	}else if(qos == (uint8_t)MQTT_QOS2){
		mqtt_buf[idx_w++] = (uint8_t)0x02;
	}
	
	//Update Length Remaining
	mqtt_buf[1] = (idx_w - 2);
	
	print_debug(0,"[INFO]MQTT Subscribe Packet Len(%d):\n",idx_w);
	print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
	print_debug(0,"\n");
	
	
//	memset(at_buf,0,sizeof(at_buf));
//	sprintf(at_buf,"AT+CIPSEND=%d\r\n",idx_w);
//	print_debug(0,"(Status)%s\r\n",at_buf);
//	ret = gsm_write((uint8_t*)at_buf,strlen(at_buf));
//	HAL_Delay(1000);
//	
//	
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\n> ",4)==0){
//				ret = 1;
//			}else{
//				ret = 0;
//			}
//		}else{
//			ret = 0;
//		}
//	}
//	
//	if(ret){
//		print_debug(0,"Data Init Transmit is OK.\r\n");
//		print_debug(0,"[INFO]MQTT Packet Len(%d):\r\n",idx_w);
//		print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
//		print_debug(0,"\n");
//		ret = gsm_write(mqtt_buf,idx_w);
//		ret = gsm_write(&ctrl_z,1);
//		HAL_Delay(1000);
//	}
	
	
	
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nSEND OK\r\n",11)==0){
//				if(data_gsm.value[11] && (uint8_t)0xF0 == (uint8_t)0x40){
//					if(data_gsm.value[12] == (uint8_t)2){
//						if((data_gsm.value[13] == (uint8_t)0x00) && (data_gsm.value[14] == (uint8_t)0x00)){
//							print_debug(0,"MQTT connect is OK.\r\n");
//							mqtt.connected = 1;
//						}else{
//							mqtt.connected = 0;
//						}
//					}
//				}
//			}
//		}
//	}	
	ret = gsm_write(mqtt_buf,idx_w);
	HAL_Delay(2000);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			
			if(data_gsm.value[0] == (uint8_t)0x90){
				if(data_gsm.value[1] == (uint8_t)3){
					//if((data_gsm.value[2] == (uint8_t)0x00) && (data_gsm.value[3] == (uint8_t)0x00))
					if(data_gsm.value[2] == (uint8_t)0x00){
						print_debug(0,"MQTT Subscribe is OK.\r\n");
						mqtt.connected = 1;
						ret = 1;
					}else{
						mqtt.connected = 0;
						ret = 0;
					}
				}
				else{
						mqtt.connected = 0;
						ret = 0;
				}
			}
			else{
					mqtt.connected = 0;
					ret = 0;
			}
		}
		else{
				mqtt.connected = 0;
				ret = 0;
		}
	}
	
	return ret;
}


int8_t mqtt_callback(){
	int8_t ret = 0;
	
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_debug(0,"[callback]len(%d).\r\n",data_gsm.length);
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)&data_gsm.value[4],"em001/data/led",14)==0){
//				print_debug(0,"[callback]em001/data/led.\r\n");
//				
//			}else if(strncmp((char*)data_gsm.value,"\r\n> ",4)==0){
//				print_debug(0,"[callback]\r\n> .\r\n");
//				if(data_gsm.value[4] && (uint8_t)0xF0 == (uint8_t)0x30){
//					if(strncmp((char*)&data_gsm.value[4],"em001/data/led",14)==0){
//						print_debug(0,"[callback]em001/data/led.\r\n");
//					}
//				}
//			}
//		}
//		
//		
//	}
	
//	print_debug(0,"(Get data manual)AT+CIPRXGET=1\r\n");
//	HAL_Delay(1000);
//	ret = gsm_write((uint8_t*)"AT+CIPRXGET=1,256\r\n",19);
//	HAL_Delay(1000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_debug(0,"[callback]len(%d).\r\n",data_gsm.length);
//			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nSEND OK\r\n",11)==0){
//				mqtt_parser(&data_gsm.value[11], (data_gsm.length - 11));
//			}
//			else if(strncmp((char*)data_gsm.value,"\r\nERROR\r\n",9)==0){
//				mqtt_parser(&data_gsm.value[9], (data_gsm.length - 9));
//			}else{
//				mqtt_parser(&data_gsm.value[0], (data_gsm.length - 0));
//			}
//		}
//	}
	
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_debug(0,"[callback]len(%d).\r\n",data_gsm.length);
			print_payload(0,data_gsm.value,data_gsm.length);
			mqtt_parser(data_gsm.value,data_gsm.length);
		}
	}

	return ret;
}

int8_t mqtt_parser(uint8_t * val, uint16_t len){
	int8_t ret = 0;
	uint8_t header_control = 0;
	uint8_t header_qos = 0;
	uint8_t len_remain = 0;
	uint8_t ret_code1 = 0;
	uint8_t ret_code2 = 0;
	uint16_t ret_pack_id = 0;
	uint8_t buf_ack[5];
	uint16_t cal_all_len = 0;
	uint16_t indexread = 0;
	
	
	
	print_debug(0,"[MQTT Parser]len(%d)\n",len);
	if(len <= (uint16_t)0){
		print_debug(0,"[MQTT_CALLBACK]error len(%d)\n",len);
		return ret;
	}
	
	do
	{
			header_control = (val[indexread] & 0xF0); 
		print_debug(0,"[MQTT_CALLBACK]header conrtol(%02X)\n",header_control);
		if(header_control == (uint8_t)0x20){
			len_remain = val[indexread+1];
			print_debug(0,"[MQTT_CALLBACK]CONACK Len remaining(%d)\n",len_remain);
			if(len_remain==2){
				ret_code1 = val[indexread+2];
				ret_code2 = val[indexread+3];
				if((ret_code1==0x00) && (ret_code2==0x00) ){
					print_debug(0,"[MQTT_CALLBACK]MQTT Connect OK.\n");
					ret = 1;
					indexread += len_remain+2;
				}else{
					print_debug(0,"[MQTT_CALLBACK]MQTT Connect Fail.\n");
					ret = 0;
				}
			}
		}else if(header_control == (uint8_t)0x30){
			header_qos = (val[indexread] & 0x06); 
//			if(header_qos == 0x00){//QOS0
//			
//			}else if(header_qos == 0x02){//QOS1
//			
//			}else if(header_qos == 0x04){//QOS2
//			
//			}
			len_remain = val[indexread+1];
			print_debug(0,"[MQTT_CALLBACK]PUBLISH Len remaining(%d)\n",len_remain);
			mqtt_data_rx.topic.length = (uint16_t)(val[indexread+2] << 8) | val[indexread+3];
			print_debug(0,"[MQTT_CALLBACK]PUBLISH Topic Len(%d)\n",mqtt_data_rx.topic.length);
			memcpy(mqtt_data_rx.topic.value,&val[indexread+4],mqtt_data_rx.topic.length);
			print_payload(0,mqtt_data_rx.topic.value,mqtt_data_rx.topic.length);
			
	//		cal_all_len += mqtt_data_rx.topic.length;
	//		cal_all_len += (len_remain - mqtt_data_rx.topic.length);//Len of Payload
			
			mqtt_data_rx.payload.length = (len_remain - mqtt_data_rx.topic.length - 2);//2=len position of topic
			print_debug(0,"[MQTT_CALLBACK]PUBLISH Payload Len(%d)\n",mqtt_data_rx.payload.length);
			memcpy(mqtt_data_rx.payload.value,&val[indexread+4+mqtt_data_rx.topic.length],mqtt_data_rx.topic.length);
			print_payload(0,mqtt_data_rx.payload.value,mqtt_data_rx.payload.length);
			
			//Check QoS > 0 Only
			if((val[indexread+0] & 0x06) == (uint8_t)MQTT_QOS0){
				print_debug(0,"[MQTT_CALLBACK]PUBLISH QoS(%d)\n",0);
				net_callback(mqtt_data_rx.topic.value, mqtt_data_rx.topic.length, mqtt_data_rx.payload.value, mqtt_data_rx.payload.length, (uint16_t)0);
				ret = 1;
				indexread += len_remain+2;
			}else if((val[indexread+0] & 0x06) == (uint8_t)MQTT_QOS1){
				print_debug(0,"[MQTT_CALLBACK]PUBLISH QoS(%d)\n",1);
				memcpy(&ret_pack_id,mqtt_data_rx.payload.value,sizeof(uint16_t));
				print_debug(0,"[MQTT_CALLBACK]PUBLISH MID(%d DCE)\n",ret_pack_id);
				print_payload(0,(uint8_t*)&ret_pack_id,sizeof(uint16_t));
				net_callback(mqtt_data_rx.topic.value, mqtt_data_rx.topic.length, &mqtt_data_rx.payload.value[2], mqtt_data_rx.payload.length-2, (uint16_t)ret_pack_id);
				ret = 1;
				indexread += len_remain+2;
			}else if((val[indexread+0] & 0x06) == (uint8_t)MQTT_QOS2){
				print_debug(0,"[MQTT_CALLBACK]PUBLISH QoS(%d)\n",2);
				//ret_pack_id = 
			}
			
		}else if(header_control == (uint8_t)0x40){
			len_remain = val[indexread+1];
			if(len_remain == 2){
				ret_pack_id = (uint16_t)(val[indexread+2] << 8) | val[indexread+3];
				print_debug(0,"[MQTT_CALLBACK]PUBACK Packet ID(%d)\n",ret_pack_id);
				ret = 1;
				mqtt.connected = 1;
				indexread += len_remain+2;
			}else{
				ret = 0;
			}
		}else if(header_control == (uint8_t)0x90){
			print_debug(0,"[MQTT_CALLBACK]SUBACK(0x90) Packet ID(%d)\n",ret_pack_id);
			len_remain = val[indexread+1];
			if(len_remain == 3){
				ret_pack_id = (uint16_t)(val[indexread+2] << 8) | val[indexread+3];
				print_debug(0,"[MQTT_CALLBACK]SUBACK Packet ID(%d)\n",ret_pack_id);
				ret_code1 = val[indexread+4];
				if((ret_code1 >= 0) || (ret_code1 < 3)){
					ret = 1;
					indexread += len_remain+2;
				}else{
					ret = 0;
				}
			}else{
				ret = 0;
			}
		}else if(header_control == (uint8_t)0xC0){
			buf_ack[0] = (uint8_t)0xD0;
			buf_ack[1] = (uint8_t)0x00;
			print_debug(0,"[MQTT_CALLBACK]PINGREQ(0xC0) Packet ID(%d)\n",ret_pack_id);
			//wifi_write(buf_ack,2);
			mqtt_ack_ping(buf_ack,2);
			ret = 1;
		}else if(header_control == (uint8_t)0xD0){
			len_remain = val[indexread+1];
			if(len_remain == 0){
				print_debug(0,"[MQTT_CALLBACK]PINGRESP(0xD0).\n");
				mqtt.connected = 1;
				ret = 1;
				indexread += len_remain+2;
			}else{
				ret = 0;
			}
			if(ret == 1){
				mqtt.connected = 1;
				print_debug(0,"[MQTT]PING is OK\n");
			}else{
				mqtt.connected = 0;
				print_debug(0,"[MQTT]PING is fail\n");
			}
			//indexread++;
		}
		else
		{
			indexread++;
		}
		
	}while(indexread < len);
	
	return ret;
}


int8_t mqtt_ack_ping(uint8_t *val, uint16_t len){
	int8_t ret = 0;
	char at_buf[30];
	
	
	
	memset(at_buf,0,sizeof(at_buf));
	sprintf(at_buf,"AT+CIPSEND=%d\r\n",len);
	print_debug(0,"(Status)%s\r\n",at_buf);
	ret = gsm_write((uint8_t*)at_buf,strlen(at_buf));
	HAL_Delay(1000);
	
	
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\n> ",4)==0){
				ret = 1;
			}else{
				ret = 0;
			}
		}else{
			ret = 0;
		}
	}
	
	if(ret){
		print_debug(0,"Data Init Transmit is OK.\r\n");
		print_debug(0,"[INFO]MQTT Packet Len(%d):\r\n",len);
		print_payload(0,(const unsigned char*)val, len);
		print_debug(0,"\n");
		ret = gsm_write(val,len);
		ret = gsm_write(&ctrl_z,1);
		HAL_Delay(1000);
	}
	return ret;
}

int8_t mqtt_ping(){
	int8_t ret;
	uint16_t idx_w = 0;
	uint16_t receiv_len = 0;
	uint16_t i = 0;
	
	mqtt_buf[idx_w++] = (uint8_t)MQTT_PINGREQ;
	mqtt_buf[idx_w++] = (uint8_t)0x00;
	
	print_debug(0,"[MQTT]PING Packet (Len=%d):\r\n",idx_w);
	print_payload(0,(const unsigned char*)mqtt_buf, idx_w);
	print_debug(0,"\n");
	
	char at_buf[30];
	

	ret = gsm_write(mqtt_buf,idx_w);
	for(i=0;i<4;i++)
	{
		//GetGPS();
		HAL_Delay(500);
	}
	
	//HAL_Delay(2000);
//	ret = gsm_read(data_gsm.value,&data_gsm.length);
//	if(ret){
//		if(data_gsm.length > 0){
//			print_payload(0,data_gsm.value,data_gsm.length);
//			
//			if(data_gsm.value[0] == (uint8_t)0xd0){
//				print_debug(0,"MQTT Ping is OK.\r\n");
//				mqtt.connected = 1;
//				ret = 1;	
//			}
//			else{
//					mqtt.connected = 0;
//					ret = 0;
//			}
//		}
//		else{
//				mqtt.connected = 0;
//				ret = 0;
//		}
//	}
	
	
	return ret;
}

