#include "rtc_internal.h"
#include "print_debug.h"
#include "stm32f0xx_hal_rtc.h"
extern RTC_HandleTypeDef hrtc;

uint8_t seconds_old = 0;

void rtc_time_show(){
	int8_t ret = 0;
	ret = rtc_time_update();
}

int8_t rtc_time_update(){
	RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  //sprintf((char*)showtime,"%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	
	if(stimestructureget.Seconds != seconds_old){
		seconds_old = stimestructureget.Seconds;
		print_debug(0,"[INFO]{rtc_time_update}%02d:%02d:%02d , %02d:%02d:%02d\n",sdatestructureget.Date,sdatestructureget.Month,sdatestructureget.Year,stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
		
		/* Request to enter STANDBY mode (Wake Up flag is cleared in HAL_PWR_EnterSTANDBYMode function) */
		HAL_PWR_EnterSTANDBYMode();
		//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	}
	
}

void rtc_sleep_init(){
	
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
  sAlarm.AlarmTime.Minutes = 0x21;
  sAlarm.AlarmTime.Seconds = 0x00;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	//sAlarm.AlarmMask = RTC_ALARMMASK_SECONDS;
	//sAlarm.AlarmMask = RTC_ALARMMASK_MINUTES;
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


	//Config Stanby and RTC wakeup
	
	
	
	 

	__HAL_RTC_ALARM_ENABLE_IT(&hrtc,RTC_IT_ALRA);
}

void rtc_set_date(RTC_DateTypeDef sDate){
//	sDate.WeekDay = RTC_WEEKDAY_SUNDAY;
//  sDate.Month = RTC_MONTH_SEPTEMBER;
//  sDate.Date = 0x18;
//  sDate.Year = 0x16;

  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void rtc_set_time(RTC_TimeTypeDef sTime){
//	sTime.Hours = 0x15;
//  sTime.Minutes = 0x20;
//  sTime.Seconds = 0x0;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

//void sleep(void) {
//    TimMasterHandle.Instance = TIM5;
// 
//    // Disable HAL tick interrupt
//    __HAL_TIM_DISABLE_IT(&TimMasterHandle, TIM_IT_CC2);
// 
//    // Request to enter SLEEP mode
//    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
// 
//    // Enable HAL tick interrupt
//    __HAL_TIM_ENABLE_IT(&TimMasterHandle, TIM_IT_CC2);
//}
// 
//void deepsleep(void) {
//    // Request to enter STOP mode with regulator in low power mode
//    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
// 
//    // After wake-up from STOP reconfigure the PLL
//    SetSysClock();
//}