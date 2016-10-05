/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */
#include "main.h"
#include "print_debug.h"
#include "gsm.h"
#include "mqtt_ptc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "network.h"
#include "rtc_internal.h"
#include "flash_rw.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
data_t data_gsm, data_debug,data_gps;
recv_uart_t recv_uart_1,recv_uart_2;
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);

extern int FlagSendAlarm;
extern mqtt_t mqtt;
extern int FlagPing;
extern int FlagSendGPS;
extern int num_sendGPS;
extern int newADC_int;

int8_t ret = 0;
uint8_t mqtt_user[32] = "nx_device";
uint8_t mqtt_aio_key[32] = "25353f7437f14595a69bee0f970cf62d";	//new gen
uint8_t mqtt_client_id[32] = "nx-smart-device";
char Longitude[20];
char Latitude[20];
char conLongitude[20];
char conLatitude[20];
char Date[20];
char CSQ[20];
uint32_t Csleep = 0;
int randomSGSM;
int GSM_int;
char*inputRan;
char ADC_char[20];
int OldADC_int = 0;

char Time[20];
char TimeOld[20];
char datagps[100];
int gps_index;


uint8_t CSendDataPull = 0;
uint8_t CSendDataCut = 0;

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	int8_t ret = 0;
	uint16_t idx_r = 0;
	char *tok = NULL;
	char *tok_buf = NULL;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  //MX_RTC_Init();

  /* USER CODE BEGIN 2 */
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	memset(&recv_uart_1,0,sizeof(recv_uart_t));
	memset(&recv_uart_2,0,sizeof(recv_uart_t));
	
	
	memset(&data_gsm,0,sizeof(data_t));
	memset(&data_debug,0,sizeof(data_t));
	
	uart1_flush();
	uart2_flush();
	print_init(&huart2);
	
//	print_debug(0,"[INFO]MCU Wake Up ...\r\n");
//	
//	print_debug(0,"data:%08X\n",*((uint32_t*)FLASH_USER_START_ADDR));
//	HAL_Delay(5000);

//	memcpy(&mqtt.connected,((uint32_t*)FLASH_USER_START_ADDR),sizeof(uint32_t));
//	print_debug(0,"[INFO] mqtt.connected = %d ...\r\n",mqtt.connected);
//	if(mqtt.connected==0)
//	{
//		
//	}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		Power_On();

		Test_Con();

		HAL_Delay(2000);
		print_debug(0,"[GSM] Net Connect Complete...\r\n");
		while(mqtt_connect(mqtt_user, 9, mqtt_aio_key, 32, mqtt_client_id, 15)==0)
		{
			HAL_Delay(1000);
		}
		//mqtt_publish((uint8_t*)"m001/data/gsm_sig", (uint16_t)17, (uint8_t*)CSQ, (uint16_t)strlen(CSQ), (uint8_t)MQTT_QOS1, 0);
		
	HAL_TIM_Base_Init(&htim3);
	HAL_TIM_Base_Start_IT(&htim3);

  while (1)
  {
		
  /* USER CODE END WHILE */
		GetGPS();
		//print_debug(0,"[GSM]Test..\r\n");
		if(mqtt.connected==1)
		{
			//GetGPS();
			HAL_Delay(50);
			mqtt_callback();
			
			SensorAll();
			
			// Ping every 10 s
			if(FlagPing)
			{
				HAL_TIM_Base_Stop_IT(&htim3);
				mqtt_ping();
				FlagPing = 0;
				num_sendGPS++;
				if(num_sendGPS == 2)
				{
					FlagSendGPS = 1;
				}
				HAL_TIM_Base_Start_IT(&htim3);
			}
			// Send GPS every 20 s
			if(FlagSendGPS)
			{
				HAL_TIM_Base_Stop_IT(&htim3);
				FlagSendGPS = 0;
				num_sendGPS=0;
				GetGPS();
				
				
				randomGMS(CSQ);
				convertTime(Time);
				memcpy(conLatitude,Latitude,strlen(Latitude));
				memcpy(conLongitude,Longitude,strlen(Longitude));
				convert2Degrees(conLatitude);
				convert2Degrees(conLongitude);
				SensorAll();
				mqtt_publish((uint8_t*)"m001/data/gsm_signal", (uint16_t)20, (uint8_t*)CSQ, (uint16_t)strlen(CSQ), (uint8_t)MQTT_QOS1, 0);
				HAL_Delay(50);
				mqtt_callback();
//				mqtt_publish((uint8_t*)"m001/data/Time", (uint16_t)14, (uint8_t*)Time, (uint16_t)strlen(Time), (uint8_t)MQTT_QOS1, 0);
//				HAL_Delay(1000);
//				mqtt_callback();
//				mqtt_publish((uint8_t*)"m001/data/Date", (uint16_t)14, (uint8_t*)Date, (uint16_t)strlen(Date), (uint8_t)MQTT_QOS1, 0);
//				HAL_Delay(1000);
//				mqtt_callback();
//				mqtt_publish((uint8_t*)"m001/data/gsm_lat", (uint16_t)17, (uint8_t*)conLatitude, (uint16_t)strlen(conLatitude), (uint8_t)MQTT_QOS1, 0);
//				HAL_Delay(1000);
//				mqtt_callback();
//				mqtt_publish((uint8_t*)"m001/data/gsm_long", (uint16_t)18, (uint8_t*)conLongitude, (uint16_t)strlen(conLongitude), (uint8_t)MQTT_QOS1, 0);
//				HAL_Delay(1000);
//				mqtt_callback();

//				gps_index = 0;
//				memset(datagps,0,sizeof(datagps));
//				memcpy(datagps,Date,strlen(Date));
//				gps_index = strlen(Date)+1;
//				datagps[gps_index] = ',';
//				memcpy(&datagps[gps_index++],Time,strlen(Time));
//				datagps[gps_index++] = ',';
				SensorAll();
				if(Date[0]== 0 && Date[1]== 0){
					Date[0]='0';
				}
				
				sprintf(datagps,"%s,%s,%s,%s",Date,Time,conLatitude,conLongitude);
				print_debug(0,"[GSM]%s\r\n",datagps);
				mqtt_publish((uint8_t*)"m001/data/gps", (uint16_t)13, (uint8_t*)datagps, (uint16_t)strlen(datagps), (uint8_t)MQTT_QOS1, 0);
				HAL_Delay(50);
				mqtt_callback();
			
				
				HAL_TIM_Base_Start_IT(&htim3);
				
//				// Close GSM
////				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
////				HAL_Delay(3000);
////				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
//				
//				
//				//Csleep = 2;
//				flash_write((uint32_t)0, mqtt.connected);
//				print_debug(0,"data:%08X\n",*((uint32_t*)FLASH_USER_START_ADDR));
//				system_init();
//				HAL_Delay(1000);				
//				while(1)
//				{
//					print_debug(0,"[INFO]MCU Sleep Mode...\r\n");
//					rtc_time_show();
//					HAL_Delay(10);
//				}

			}
		}
		else
		{
			Power_On();

			Test_Con();

			HAL_Delay(2000);
			print_debug(0,"[GSM] Net Connect Complete...\r\n");
			while(mqtt_connect(mqtt_user, 9, mqtt_aio_key, 32, mqtt_client_id, 15)==0)
			{
				HAL_Delay(1000);
			}
			//mqtt_publish((uint8_t*)"m001/data/gsm_sig", (uint16_t)17, (uint8_t*)CSQ, (uint16_t)strlen(CSQ), (uint8_t)MQTT_QOS1, 0);
			HAL_TIM_Base_Init(&htim3);
			HAL_TIM_Base_Start_IT(&htim3);
		}
		
  }
	

}

void Power_On(void)
{
	uint8_t ret =0;
	
//	if(Csleep==0)
//	{
//		
//	}
	print_debug(0,"program startup...\n");
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_SET);
	

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
	
	HAL_Delay(5000);
	
	print_debug(0,"[GSM] AT \n");
	SsendATcommand((uint8_t*)"ATE0\r\n",4,(uint8_t*)"\r\nOK\r\n",6,100);
	while(ret==0)
	{
		ret = SsendATcommand((uint8_t*)"AT\r\n",4,(uint8_t*)"\r\nOK\r\n",6,100);
	}
	print_debug(0,"[INFO] GSM ON.\r\n");

	print_debug(0,"(Powers GPS)AT+CGPSPWR=1\r\n");
	SsendATcommand((uint8_t*)"AT+CGPSPWR=1\r\n",14,(uint8_t*)"\r\nOK\r\n",6,1000);
	//HAL_Delay(500);
//	
	print_debug(0,"(Set Reset mode)AT+CGPSRST=0\r\n");
	SsendATcommand((uint8_t*)"AT+CGPSRST=0\r\n",14,(uint8_t*)"\r\nOK\r\n",6,500);
	
	ReadBett();
	
	GetsignalGSM(CSQ);

	//ret = 0;
//	while(1)
//	{
//		print_debug(0,"(STATUS)AT+CGPSSTATUS?\r\n");
//		if(SsendATcommand((uint8_t*)"AT+CGPSSTATUS?\r\n",16,(uint8_t*)"\r\n+CGPSSTATUS: Location 2D Fix\r\n",32,5000))
//		{
//			break;
//		}
//		if(SsendATcommand((uint8_t*)"AT+CGPSSTATUS?\r\n",16,(uint8_t*)"\r\n+CGPSSTATUS: Location 3D Fix\r\n",32,5000))
//		{
//			break;
//		}
//	}
//	
//	while (SsendATcommand((uint8_t*)"AT+CREG?\r\n",14,(uint8_t*)"\r\n+CREG: 0,1",12,2000) == 0);
//	
//	print_debug(0,"[GSM] SET GPRS.\r\n");
//	SsendATcommand((uint8_t*)"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n",31,(uint8_t*)"\r\nOK\r\n",6,500);
//	print_debug(0,"[GSM] SET APN.\r\n");
//	SsendATcommand((uint8_t*)"AT+SAPBR=3,1,\"APN\",\"www.dtac.co.th\"\r\n",37,(uint8_t*)"\r\nOK\r\n",6,500);
//	
//	
//	 // gets the GPRS bearer
//	print_debug(0,"[GSM] GET GPRS.\r\n");
//	while (SsendATcommand((uint8_t*)"AT+SAPBR=1,1\r\n",14,(uint8_t*)"\r\nOK\r\n",6,3000) == 0)
//	{
//			//print_debug(0,"[GSM] ERROR\r\n");
//			HAL_Delay(5000);
//	}
	
}

void Test_Con(void)
{
	
	print_debug(0,"[GSM] AT+CGPSOUT=2\r\n");
	SsendATcommand((uint8_t*)"AT+CGPSOUT=32\r\n",14,(uint8_t*)"\r\nOK\r\n",6,100);
	
	//HAL_Delay(100);
	
	print_debug(0,"[GSM] AT+CIPMODE\r\n");
	SsendATcommand((uint8_t*)"AT+CIPMODE=1\r\n",14,(uint8_t*)"\r\nOK\r\n",6,1000);
	
	//HAL_Delay(100);
	
	print_debug(0,"[GSM] AT+CGATT?\r\n");
	SsendATcommand((uint8_t*)"AT+CGATT?\r\n",11,(uint8_t*)"\r\nCGATT: 1\r\nOK\r\n",16,1000);
	
	//HAL_Delay(100);
	
	print_debug(0,"[GSM] AT+CSTT=\"CMNET\"\r\n");
	SsendATcommand((uint8_t*)"AT+CSTT=\"CMNET\"\r\n",17,(uint8_t*)"\r\nOK\r\n",6,1000);
	
	//HAL_Delay(100);
	
	print_debug(0,"[GSM] AT+CIICR\r\n");
	SsendATcommand((uint8_t*)"AT+CIICR\r\n",10,(uint8_t*)"\r\nOK\r\n",6,1500);
	
	//HAL_Delay(100);
	print_debug(0,"[GSM] AT+CIFSR\r\n");
	SsendATcommand((uint8_t*)"AT+CIFSR\r\n",10,(uint8_t*)"\r\nOK\r\n",6,1500);
	
	//HAL_Delay(100);
	print_debug(0,"[GSM] AT+CIFSR\r\n");
	SsendATcommand((uint8_t*)"AT+CIFSR\r\n",10,(uint8_t*)"\r\nOK\r\n",6,1500);
	
	
	//HAL_Delay(100);
	print_debug(0,"[GSM]AT+CIPSTART=\"TCP\",\"io.codeunbug.com\",\"5555\"\r\n");
	//while(SsendATcommand((uint8_t*)"AT+CIPSTART=\"TCP\",\"io.codeunbug.com\",\"5555\"\r\n",45,(uint8_t*)"\r\nOK\r\n\r\nCONNECT\r\n",15,10000)==0);
	while(SsendATcommand((uint8_t*)"AT+CIPSTART=\"TCP\",\"mqtt.codeunbug.com\",\"1883\"\r\n",47,(uint8_t*)"\r\nOK\r\n\r\nCONNECT\r\n",15,8000)==0)
	{
		HAL_Delay(500);
	}
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
	
}

int8_t GetsignalGSM(char*input)
{
	int8_t ret = 0;
	uint8_t gsm_sig = 0;
	char *token = NULL;
	char str_sig_buff[4];
	int i = 0;

	float deg;
  uint8_t neg = 0;
	float minutes;
	float numberFloat;
	int intPart[10];
	int digit; 	
	long newNumber;
	int size;
    //auxiiar variable
  char aux[10];

//	
//	HAL_Delay(1000);
//	//while(SsendATcommand((uint8_t*)"+++\r\n",5,(uint8_t*)"\r\nCLOSED\r\n",10,800)==0);
//	HAL_Delay(1000);
	
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
					memset(input,0,strlen(input));
					print_debug(0,"GSM Signal(%s) is OK.\r\n",str_sig_buff);
					memcpy(input,str_sig_buff,strlen(str_sig_buff));
					ret = 1;
				}
				
			}
		}
	}
	
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
	GSM_int = (int)deg*(100/31);
	
	
	
	
	//print_debug(0,"GSM Signal int %d.\r\n",GSM_int);
	return ret;
}

uint8_t randomGMS(char*input)
{
	float deg;
  uint8_t neg = 0;
	float minutes;
	float numberFloat;
	int intPart[10];
	int digit; 	
	long newNumber;
	int size;
    //auxiiar variable
  char aux[10];
	
	
	randomSGSM =  GSM_int + rand() % 5;
	newNumber = (long)randomSGSM;  
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
}

uint8_t GetGPS(void)
{
	uint8_t c;
	uint8_t ret;
	char TimeCP[20];
	char *token;
	c =	ReadGPSExt(data_gps.value,&data_gps.length);
	HAL_Delay(500);
	
	memset(Time,0,sizeof(Time));
	if(c == 1)
	{
		if(data_gps.length > 0){
				//Mode
			token = strtok((char*)data_gps.value,",");
			if(token != NULL){
				//print_debug(0,"\r\n(Get GPS) Mode => (%s).\r\n",token);
			}else{
				return 0;
			}
			//TIME
			token = strtok(NULL, ",");
			if(token != NULL){
				//print_debug(0,"(Get GPS) TIME=> (%s)(len:%d).\r\n",token,strlen(token));
				memset(TimeCP,0,sizeof(TimeCP));
				memcpy(TimeCP,token,strlen(token));
				//print_debug(0,"(Get GPS) TIME=> (%s)(len:%d).\r\n",Time,strlen(Time));
			}else{
				return 0;
			}
			
			token = strtok(NULL, ",");
			if(token != NULL)
			{
				if(token[0]=='A')
				{
					memset(Time,0,sizeof(Time));
					memcpy(Time,TimeCP,strlen(TimeCP));
					
					memcpy(TimeOld,TimeCP,strlen(TimeCP));
							//Latitude
					token = strtok(NULL, ",");
					if(token != NULL){
						//print_debug(0,"(Get GPS) Latitude=> (%s)(len:%d).\r\n",token,strlen(token));
						memset(Latitude,0,sizeof(Latitude));
						memcpy(Latitude,token,strlen(token));
						//print_debug(0,"(Get GPS) Latitude=> (%s)(len:%d).\r\n",Latitude,strlen(Latitude));
					}else{
						return 0;
					}
					
					token = strtok(NULL, ",");
					//Longitude
					token = strtok(NULL, ",");
					if(token != NULL){
						//print_debug(0,"(Get GPS) Longitude=> (%s)(len:%d).\r\n",token,strlen(token));
						memset(Longitude,0,sizeof(Longitude));
						memcpy(Longitude,token,strlen(token));
						
						//print_debug(0,"(Get GPS) Longitude=> (%s)(len:%d).\r\n",Longitude,strlen(Longitude));
					}else{
						return 0;
					}
					token = strtok(NULL, ",");
					token = strtok(NULL, ",");
					token = strtok(NULL, ",");
					
					//Deta
					token = strtok(NULL, ",");
					if(token != NULL){
						//print_debug(0,"(Get GPS) Longitude=> (%s)(len:%d).\r\n",token,strlen(token));
						memset(Date,0,sizeof(Date));
						memcpy(Date,token,strlen(token));
						
						//print_debug(0,"(Get GPS) Longitude=> (%s)(len:%d).\r\n",Date,strlen(Date));
					}else{
						return 0;
					}
					
				}
				
			}
			
			

		}
	}
}

void SensorAll(void)
{
	//Read Senser ADC Pull Line
		if(Get_ADC(ADC_char))
		{
			if(newADC_int!=OldADC_int)
			{
				if(newADC_int>OldADC_int)
				{
						//Get_ADC(ADC_char);
					if((newADC_int-OldADC_int)>5)
					{
						mqtt_publish((uint8_t*)"m001/data/strain", (uint16_t)16, (uint8_t*)ADC_char, (uint16_t)strlen(ADC_char), (uint8_t)MQTT_QOS1, 0);
						HAL_Delay(50);
						mqtt_callback();
					}
					
				}
				else if(newADC_int<OldADC_int)
				{
						if((OldADC_int-newADC_int)>5)
						{
							mqtt_publish((uint8_t*)"m001/data/strain", (uint16_t)16, (uint8_t*)ADC_char, (uint16_t)strlen(ADC_char), (uint8_t)MQTT_QOS1, 0);
							HAL_Delay(50);
							mqtt_callback();
						}
				}
				FlagSendAlarm = 0;
		
				CSendDataPull = 1;
				OldADC_int = newADC_int;
			}
//			if(CSendDataPull == 0)
//			{
////					mqtt_publish((uint8_t*)"m001/Event/Alarm_Pull", (uint16_t)21, (uint8_t*)"Alarm Pulling Line", (uint16_t)18, (uint8_t)MQTT_QOS1, 0);
////					HAL_Delay(1000);
//				Get_ADC(ADC_char);
//				mqtt_publish((uint8_t*)"m001/data/strain", (uint16_t)16, (uint8_t*)ADC_char, (uint16_t)strlen(ADC_char), (uint8_t)MQTT_QOS1, 0);
//				HAL_Delay(50);
//				mqtt_callback();
//				CSendDataPull = 1;
//			}
			
		}
		else
		{
			if(CSendDataPull == 1)
			{
					mqtt_publish((uint8_t*)"m001/data/strain", (uint16_t)16, (uint8_t*)ADC_char, (uint16_t)strlen(ADC_char), (uint8_t)MQTT_QOS1, 0);
					HAL_Delay(50);
					mqtt_callback();
					CSendDataPull = 0;
			}
	
		}
		//Read Senser Cut libe
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==1)
			{
				if(CSendDataCut == 0)
				{
					FlagSendAlarm = 0;
					mqtt_publish((uint8_t*)"m001/event/alarm_cut", (uint16_t)20, (uint8_t*)"Alarm Cuting Line", (uint16_t)17, (uint8_t)MQTT_QOS1, 0);
					HAL_Delay(50);
					mqtt_callback();
					CSendDataCut = 1;
				}
				
			}
			else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)==0)
			{
				if(CSendDataCut == 1)
				{
					mqtt_publish((uint8_t*)"m001/event/alarm_cut", (uint16_t)20, (uint8_t*)"Normal", (uint16_t)6, (uint8_t)MQTT_QOS1, 0);
					HAL_Delay(50);
					mqtt_callback();
					CSendDataCut = 0;
				}
				
			}
		
}

int SsendATcommand(uint8_t* ATcommand,uint16_t lenAT,uint8_t* Ret,uint16_t lenRet,int Time)
{
	int ret =0;
	int C = 0;
	ret = gsm_write((uint8_t*)ATcommand,lenAT);
	HAL_Delay(Time);
	ret = gsm_read(data_gsm.value,&data_gsm.length);
	if(ret == 1)
	{
		if(data_gsm.length > 0){
			print_payload(0,data_gsm.value,data_gsm.length);
			//print_debug(0,"(RETURN GSM) %s  :",data_gsm.value);
			if(strncmp((char*)data_gsm.value,(char*)Ret,lenRet)==0){
				C = 1;
				//print_debug(0,"[INFO] GSM ON.\r\n");
				//print_debug(0,"[INFO] GSM ON.\r\n");
				//break;
			}
		}
	}
	
	return C;
}



/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC init function */
void MX_ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  HAL_ADC_Init(&hadc);

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  HAL_ADC_ConfigChannel(&hadc, &sConfig);

}

/* RTC init function */
void MX_RTC_Init(void)
{

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  RTC_AlarmTypeDef sAlarm;
  RTC_TamperTypeDef sTamper;

    /**Initialize RTC and set the Time and Date 
    */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  HAL_RTC_Init(&hrtc);

  sTime.Hours = 0x15;
  sTime.Minutes = 0x20;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

//  sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
//  sDate.Month = RTC_MONTH_SEPTEMBER;
//  sDate.Date = 0x18;
//  sDate.Year = 0x16;

//  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

    /**Enable the Alarm A 
    */
  sAlarm.AlarmTime.Hours = 0x15;
  sAlarm.AlarmTime.Minutes = 0x20;
  sAlarm.AlarmTime.Seconds = 0x10;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD);

    /**Enable the RTC Tamper 2 
    */
//  sTamper.Tamper = RTC_TAMPER_2;
//  sTamper.Trigger = RTC_TAMPERTRIGGER_FALLINGEDGE;
//  sTamper.Filter = RTC_TAMPERFILTER_DISABLE;
//  sTamper.SamplingFrequency = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768;
//  sTamper.PrechargeDuration = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
//  sTamper.TamperPullUp = RTC_TAMPER_PULLUP_ENABLE;
//  sTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
//  HAL_RTCEx_SetTamper(&hrtc, &sTamper);

}

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48000;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart1);

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart2);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(latch_pin_GPIO_Port, latch_pin_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, clock_pin_Pin|data_pin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_SET);

  /*Configure GPIO pins : latch_pin_Pin clock_pin_Pin data_pin_Pin */
  GPIO_InitStruct.Pin = latch_pin_Pin|clock_pin_Pin|data_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 sw_1_Pin sw_2_Pin sw_3_Pin 
                           sw_4_Pin sw_5_Pin sw_6_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_10|sw_1_Pin|sw_2_Pin|sw_3_Pin 
                          |sw_4_Pin|sw_5_Pin|sw_6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_15|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	print_debug(0,"Wrong parameters value: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
