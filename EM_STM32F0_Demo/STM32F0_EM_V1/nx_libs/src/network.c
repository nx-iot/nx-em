#include "network.h"
#include "mqtt_ptc.h"
#include "gsm.h"
#include "print_debug.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


extern data_t data_gsm;
extern mqtt_t mqtt;
extern char TimeOld[20];
extern gps_t gps;
extern gps_str_t gps_str;
extern int FlagSendAlarm;
extern ADC_HandleTypeDef hadc;
net_t net;

int newADC_int;


int8_t net_run(){
	int8_t ret = 0;
	uint8_t mqtt_user[32] = "nx_device";
	uint8_t mqtt_aio_key[32] = "25353f7437f14595a69bee0f970cf62d";	//new gen
	uint8_t mqtt_client_id[32] = "nx-smart-device";
	uint16_t remain_sub_time = 10;
	uint8_t gsm_sig = 0; 
	uint8_t gsm_Bett = 0;
	char Ddata[20];
	char str_buff[10];
	char str_gps_buff[50];
	uint8_t CSendDataPull = 0;
	uint8_t CSendDataCut = 0;
	
	net.connected = 0;
	
	net.ptc = NET_MQTT_PTC;
	
	if(net.ptc == NET_MQTT_PTC){
		while(1){
			
			//if(net.connected){
				if(mqtt.connected){
					
					//mqtt_callback();
					
//					mqtt_publish((uint8_t*)"em001/data/gps", (uint16_t)14, (uint8_t*)"12345", (uint16_t)5, (uint8_t)MQTT_QOS1, 0);
//					HAL_Delay(1000);
					
					while(remain_sub_time > (uint16_t)0){
						
						if(Get_ADC(Ddata))
						{
							FlagSendAlarm = 0;
							if(CSendDataPull == 0)
							{
								mqtt_publish((uint8_t*)"m001/Event/Alarm_Pull", (uint16_t)21, (uint8_t*)"Alarm Pulling Line", (uint16_t)18, (uint8_t)MQTT_QOS1, 0);
								HAL_Delay(1000);
								mqtt_callback();
								CSendDataPull = 1;
							}
							
						}
						else
						{
							if(CSendDataPull == 1)
							{
									mqtt_publish((uint8_t*)"m001/Event/Alarm_Pull", (uint16_t)21, (uint8_t*)"Normal", (uint16_t)6, (uint8_t)MQTT_QOS1, 0);
									HAL_Delay(1000);
									mqtt_callback();
									CSendDataPull = 0;
							}
					
						}
						if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0)
						{
							if(CSendDataCut == 0)
							{
								FlagSendAlarm = 0;
								mqtt_publish((uint8_t*)"m001/Event/Alarm_Cut", (uint16_t)20, (uint8_t*)"Alarm Cuting Line", (uint16_t)17, (uint8_t)MQTT_QOS1, 0);
								HAL_Delay(1000);
								mqtt_callback();
								CSendDataCut = 1;
							}
							
						}
						else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==1)
						{
							if(CSendDataCut == 1)
							{
								mqtt_publish((uint8_t*)"m001/Event/Alarm_Cut", (uint16_t)20, (uint8_t*)"Normal", (uint16_t)6, (uint8_t)MQTT_QOS1, 0);
								HAL_Delay(1000);
								mqtt_callback();
								CSendDataCut = 0;
							}
							
						}
						mqtt_callback();
						//HAL_Delay(100);
						remain_sub_time--;
						//print_debug(0,"remain_sub_time(%d)\r\n",remain_sub_time);
					}
					
					if(remain_sub_time <= 0){
						mqtt_ping();
						mqtt_callback();
						remain_sub_time = 10;
						ret = gsm_read_signal(&gsm_sig);
						if(ret){
							if((gsm_sig >= 0) || (gsm_sig <= 100)){
								
								sprintf(str_buff,"%d",gsm_sig);
								mqtt_publish((uint8_t*)"m001/data/gsm_sig", (uint16_t)17, (uint8_t*)str_buff, (uint16_t)strlen(str_buff), (uint8_t)MQTT_QOS1, 0);
								HAL_Delay(1000);
								mqtt_callback();
							}
						}
						
						//Read Batt
						ReadBett();
						
						//End Read Batt
						ret = gsm_read_gps(&gps, &gps_str);
						convert2Degrees(gps_str.longitude);
						convert2Degrees(gps_str.latitude);
						HAL_Delay(1000);
						if(ret){
							memset(str_gps_buff,0,sizeof(str_gps_buff));
							mqtt_publish((uint8_t*)"m001/data/gsm_long", (uint16_t)18, (uint8_t*)gps_str.longitude, (uint16_t)strlen(gps_str.longitude), (uint8_t)MQTT_QOS1, 0);
							HAL_Delay(1000);
							mqtt_callback();
							HAL_Delay(200);
							
							mqtt_publish((uint8_t*)"m001/data/gsm_lat", (uint16_t)17, (uint8_t*)gps_str.latitude, (uint16_t)strlen(gps_str.latitude), (uint8_t)MQTT_QOS1, 0);
							HAL_Delay(1000);
							mqtt_callback();
							HAL_Delay(200);
//							
//							mqtt_publish((uint8_t*)"em001/data/gsm_utctime", (uint16_t)22, (uint8_t*)gps_str.utc_time, (uint16_t)strlen(gps_str.utc_time), (uint8_t)MQTT_QOS1, 0);
//							HAL_Delay(1000);
//							mqtt_callback();
//							HAL_Delay(200);
							
							sprintf(str_gps_buff,"%s,%s,%s",gps_str.longitude,gps_str.latitude,gps_str.utc_time);
							mqtt_publish((uint8_t*)"m001/data/gps", (uint16_t)13, (uint8_t*)str_gps_buff, (uint16_t)strlen(str_gps_buff), (uint8_t)MQTT_QOS1, 0);
							HAL_Delay(1000);
							mqtt_callback();
							HAL_Delay(200);
						}
						
					}
				}else{
					ret = mqtt_connect(mqtt_user, 9, mqtt_aio_key, 32, mqtt_client_id, 15);
					
					ret = mqtt_subscribe((uint8_t*)"m001/data/led", (uint16_t)13, (uint8_t)MQTT_QOS1);
					//HAL_Delay(1000);
					mqtt_callback();
//					HAL_Delay(1000);
//					ret = mqtt_subscribe((uint8_t*)"m001/data/Clear_Alarm", (uint16_t)21, (uint8_t)MQTT_QOS1);
//					HAL_Delay(1000);
//					mqtt_callback();
				}
				
//			}else{
//				ret = net_connect();
//			}
		}
		
	}
	
	return ret;
}

/* convert2Degrees ( input ) - performs the conversion from input 
 * parameters in  DD°MM.mmm’ notation to DD.dddddd° notation. 
 * 
 * Sign '+' is set for positive latitudes/longitudes (North, East)
 * Sign '-' is set for negative latitudes/longitudes (South, West)
 *  
 */
int8_t convert2Degrees(char* input){

    float deg;
    float minutes;
    uint8_t neg = 0;
		float numberFloat;
		int intPart[10];
    int digit; 	
		long newNumber;
		int size;

    //auxiliar variable
    char aux[10];

    if (input[0] == '-')
    {
        neg = 1;
        strcpy(aux, strtok(input+1, "."));

    }
    else
    {
        strcpy(aux, strtok(input, "."));
    }

    // convert string to integer and add it to final float variable
    deg = atof(aux);

    strcpy(aux, strtok(NULL, "\0"));
    minutes=atof(aux);
    minutes/=1000000;
    if (deg < 100)
    {
        minutes += deg;
        deg = 0;
    }
    else
    {
        minutes += (int)deg % 100;
        deg = (int)deg / 100;    
    }

    // add minutes to degrees 
    deg=deg+minutes/60;


    if (neg == 1)
    {
        deg*=-1.0;
    }

    neg = 0;

    if( deg < 0 ){
        neg = 1;
        deg*=-1;
    }
    
		
    numberFloat=deg; 
//		if(numberFloat!=0)
//			numberFloat =  numberFloat + ((rand()%5)*0.000001);
//		
		
    newNumber = (long)numberFloat;  
    size = 0;
    
    while(1){
        size=size+1;
        digit=newNumber%10;
        newNumber=newNumber/10;
        intPart[size-1]=digit; 
        if (newNumber==0){
            break;
        }
    }
   
    int index=0;
    if( neg ){
        index++;
        input[0]='-';
    }
    for (int i=size-1; i >= 0; i--)
    {
        input[index]=intPart[i]+'0'; 
        index++;
    }

    input[index]='.';
    index++;

    numberFloat=(numberFloat-(int)numberFloat);
    for (int i=1; i<=6 ; i++)
    {
        numberFloat=numberFloat*10;
        digit= (long)numberFloat;          
        numberFloat=numberFloat-digit;
        input[index]=(char)digit+48;
        index++;
    }
    input[index]='\0';


}
int8_t convertTime(char* input)
{
	  float deg;
    float minutes;
    uint8_t neg = 0;
		float numberFloat;
		int intPart[10];
    int digit; 	
		long newNumber;
		int size;

    //auxiliar variable
    char aux[10];

    if (input[0] == '-')
    {
        neg = 1;
        strcpy(aux, strtok(input+1, "."));

    }
    else
    {
        strcpy(aux, strtok(input, "."));
    }
		deg = atof(aux);
		
    numberFloat=deg;
		
		if(numberFloat!=0.00000)
		{
			numberFloat=deg+70000.000;
		}
		else
		{
			//TimeOld
			memset(aux,0,sizeof(aux));
			if (TimeOld[0] == '-')
			{
					neg = 1;
					strcpy(aux, strtok(TimeOld+1, "."));

			}
			else
			{
					strcpy(aux, strtok(TimeOld, "."));
			}
			deg = atof(aux);
			numberFloat=deg;
		}
    
		print_debug(0,"(Time Value %f)",numberFloat);
		
		newNumber = (long)numberFloat;  
    size = 0;
    
    while(1){
        size=size+1;
        digit=newNumber%10;
        newNumber=newNumber/10;
        intPart[size-1]=digit; 
        if (newNumber==0){
            break;
        }
    }
   
    int index=0;
    if( neg ){
        index++;
        input[0]='-';
    }
    for (int i=size-1; i >= 0; i--)
    {
        input[index]=intPart[i]+'0'; 
        index++;
    }

//    input[index]='.';
//    index++;

//    numberFloat=(numberFloat-(int)numberFloat);
//    for (int i=1; i<=6 ; i++)
//    {
//        numberFloat=numberFloat*10;
//        digit= (long)numberFloat;          
//        numberFloat=numberFloat-digit;
//        input[index]=(char)digit+48;
//        index++;
//    }
    input[index]='\0';

}

uint8_t ReadBett(void)
{
	uint8_t ret =0;
	//SsendATcommand((uint8_t*)"AT+CBTE?\r\n",10,(uint8_t*)"\r\n+CBTE: \r\n",10,500);
	print_debug(0,"(Batt Value)");
	ret = gsm_write((uint8_t*)"AT+CBTE?\r\n",10);
	HAL_Delay(1000);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){

		print_debug(0,"(Batt Value)-------------------------->%s.\r\n",data_gsm.value);
	}

}

int8_t net_connect(){
	int8_t ret = 0;
	uint8_t gsm_sig = 0; 
	
	HAL_Delay(2000);
	print_debug(0,"(Check module)AT\r\n");
	ret = gsm_write((uint8_t*)"AT\r\n",4);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret == 1){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
				print_debug(0,"AT is OK.\r\n");
			}
		}
	}else if(ret == 0){
//		print_debug(0,"(Power up)AT+CGPSPWR=1\r\n");
//		ret = gsm_write((uint8_t*)"AT+CGPSPWR=1\r\n",14);
//		HAL_Delay(500);
//		ret = gsm_read(data_gsm.value,&data_gsm.length);
//		if(ret){
//			if(data_gsm.length > 0){
//				print_payload(0,data_gsm.value,data_gsm.length);
//				if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//					print_debug(0,"AT is OK.\r\n");
//				}
//			}
//		}
	}
	
	
	
	print_debug(0,"(Status)AT+CIPSTATUS\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPSTATUS\r\n",14);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n\r\nSTATE: IP INITIAL\r\n",27)==0){
				print_debug(0,"IP INITIAL is OK.\r\n");
			}
		}
	}
	
	print_debug(0,"(Set Context)AT+CGDCONT=0,\"IP\",\r\n");
	ret = gsm_write((uint8_t*)"AT+CGDCONT=0,\"IP\",\r\n",20);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
			}
		}
	}
	
	print_debug(0,"(Active Context)AT+CGACT=1,1.\r\n");
	ret = gsm_write((uint8_t*)"AT+CGACT=1,1\r\n",14);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(Single)AT+CIPMUX=0\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPMUX=0\r\n",13);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	
	print_debug(0,"(Speed Close Remote)AT+CIPQRCLOSE=1\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPQRCLOSE=1\r\n",17);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(MODE)AT+CIPMODE=1\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPMODE=0\r\n",14);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(GATT)AT+CGATT=1\r\n");
	ret = gsm_write((uint8_t*)"AT+CGATT=1\r\n",12);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(Close Connect)AT+CIPCLOSE\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPCLOSE\r\n",13);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n\r\nSTATE: IP INITIAL\r\n",27)==0){
//				print_debug(0,"IP INITIAL is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(Status)AT+CIPSTATUS\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPSTATUS\r\n",14);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n\r\nSTATE: IP INITIAL\r\n",27)==0){
				print_debug(0,"IP INITIAL is OK.\r\n");
			}
		}
	}
	
	print_debug(0,"(CSTT)AT+CSTT\r\n");
	ret = gsm_write((uint8_t*)"AT+CSTT\r\n",9);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
//			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
//				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
//			}
		}
	}
	
	print_debug(0,"(Status)AT+CIPSTATUS\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPSTATUS\r\n",14);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n\r\nSTATE: IP INITIAL\r\n",27)==0){
				print_debug(0,"IP INITIAL is OK.\r\n");
			}
		}
	}
	
	print_debug(0,"(CIICR)AT+CIICR\r\n");
	ret = gsm_write((uint8_t*)"AT+CIICR\r\n",10);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
				print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
			}
		}
	}
	
	print_debug(0,"(Read IP)AT+CIFSR\r\n");
	ret = gsm_write((uint8_t*)"AT+CIFSR\r\n",10);
	HAL_Delay(500);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n",6)==0){
				//print_debug(0,"(Context)AT+CGDCONT is OK.\r\n");
			}
		}
	}
	
	//ret = gsm_read_signal(&gsm_sig);
	
	print_debug(0,"(Start Connect)AT+CIPSTART=\"TCP\",\"io.codeunbug.com\",\"5555\"\r\n");
	ret = gsm_write((uint8_t*)"AT+CIPSTART=\"TCP\",\"io.codeunbug.com\",\"5555\"\r\n",45);
	HAL_Delay(2000);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret){
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			if(strncmp((char*)data_gsm.value,"\r\nOK\r\n\r\nCONNECT OK\r\n",20)==0){
				print_debug(0,"NETWORK CONNECT is OK.\r\n");
				net.connected = 1;
				ret = 1;
			}
		}
	}
	
	
//	HAL_Delay(5000);
//	HAL_UART_Transmit(&huart1,(uint8_t*)"TEST",4,500);
	
	//print_debug(0,"TEST.\r\n");
	
	
	
	return ret;
}

void net_ptc(uint8_t ptc){
	net.ptc = ptc;
}

void net_callback(uint8_t *topic_val, uint16_t topic_len, uint8_t *payload_val, uint16_t payload_len, uint16_t packet_id){

	uint8_t q_ret = 0;
	uint8_t i = 0;
	int8_t ret = 0;
	char *token = NULL;
	char buff;
	
	print_debug(0,"[INFO]Network Topic Len(%d)\n",topic_len);
	print_payload(0,topic_val,topic_len);
	
	print_debug(0,"[INFO]Network Payload Len(%d)\n",payload_len);
	print_payload(0,payload_val,payload_len);
	
	if(strncmp((const char*)topic_val,"m001/data/buzzer",16)==0){
		//Rrmote Receive
		if(strncmp((char*)payload_val,"1",1) == 0){//0:the contents of both string are equal
			print_debug(0,"Remote Status: ON\n");		
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
			HAL_Delay(100);
		}else if(strncmp((char*)payload_val,"0",1) == 0){//0:the contents of both string are equal
			print_debug(0,"Remote Status: OFF\n");
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
			HAL_Delay(100);
		}
	}
	
//	if(strncmp((const char*)topic_val,"m001/data/Clear_Alarm",21)==0)
//	{
//		mqtt_publish((uint8_t*)"m001/Event/Alarm_Pull", (uint16_t)21, (uint8_t*)"Normal", (uint16_t)6, (uint8_t)MQTT_QOS1, 0);
//		HAL_Delay(1000);
//		mqtt_callback();
//		
//		mqtt_publish((uint8_t*)"m001/Event/Alarm_Cut", (uint16_t)20, (uint8_t*)"Normal", (uint16_t)6, (uint8_t)MQTT_QOS1, 0);
//		HAL_Delay(1000);
//		mqtt_callback();
//	}
	
	
	
}


uint8_t Get_ADC(char*input)
{	
	int ret = 0;
	int ADC_Value =0;
	float deg;
	float minutes;
	uint8_t neg = 0;
	float numberFloat;
	int intPart[10];
	int digit; 	
	long newNumber;
	int size;
	uint8_t CADC =0;

    //auxiliar variable
  char aux[10];
	HAL_ADC_Start(&hadc);
	ADC_Value = HAL_ADC_GetValue(&hadc);
	
	newNumber = (long)ADC_Value; 
	newNumber = newNumber*(100.00/4095.00);
	
	newADC_int = newNumber;
	
	if(newNumber > 50)
	{
		ret =1;
	}
	size = 0;
    
	while(1){
			size=size+1;
			digit=newNumber%10;
			newNumber=newNumber/10;
			intPart[size-1]=digit; 
			if (newNumber==0){
					break;
			}
	}
 
	int index=0;
	if( neg ){
			index++;
			input[0]='-';
	}
	for (int i=size-1; i >= 0; i--)
	{
			input[index]=intPart[i]+'0'; 
			index++;
	}
	input[index]='\0';
	

	return ret;
}