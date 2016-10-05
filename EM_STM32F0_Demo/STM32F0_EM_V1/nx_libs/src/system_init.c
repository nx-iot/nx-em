#include "system_init.h"
#include "rtc_internal.h"

void system_init(){
	rtc_sleep_init();
	standby_init();
}