//=== INCLUDES =============================================================================
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"   
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "timers.h"                     // ARM.FreeRTOS::RTOS:Timers
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include <stdio.h>
#include "Led.h"
#include "SysClock.h"
#include "Ultrasuoni.h"


EventGroupHandle_t xEventGroupRullo;



int main(void){ 
	
	pllInit(0, 10, 0);	
	LedInit();
	ultrsInit();
  xEventGroupRullo=xEventGroupCreate();
	vTaskStartScheduler();	
	
}

