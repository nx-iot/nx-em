#include "standby.h"

void standby_init(){
	HAL_EnableDBGStandbyMode();
//	HAL_EnableDBGStopMode();
//	HAL_PWR_EnableBkUpAccess();

//	HAL_PWR_DisableWakeUpPin( PWR_WAKEUP_PIN1 );
//														
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//											
//	/* Enable WKUP pin */
//	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	//HAL_PWR_EnableWakeUp
	
	
}