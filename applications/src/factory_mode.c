#include "factory_mode.h"
#include "user_source.h"
#include "user_task.h"
#include "led_colour.h"
#include "button_task.h"

FACTORY_MODE_t xFactoryModeCode = IN_APP_MODE;

int32_t user_enter_factory_mode(uint8_t enter)
{
	// TODO factory mode code, enter = 1 enter factory mode, enter = 0 exit factory mode
	if (enter)
	{
		wdt_counter_reload();
		vTaskResume(xFactoryModeWatchDogTaskHandler); // 恢�?�产测模式看门狗任务
		vTaskSuspend(xWatchdogTaskHandle);			  // 挂起APP模式看门狗任�?
		vTaskSuspend(main_process_task_handle);
		vTaskSuspend(hpw6862i_task_handle);
		vTaskSuspend(xTaskMultiButtonHandler);
		vTaskSuspend(hp203b_task_handle);
		vTaskSuspend(valva_control_task_handle);
		led_show_white();
		wdt_counter_reload();
		// 产测模式改变变量 xFactoryModeCode / xFactoryModeCode
		if (xGetFactoryModeCode() != IN_FACTORY_MODE) // 不�?�于产测模式即进入产测模�?
		{
			if (xSemaphoreTake(xMutexFactoryMode, 100) == pdTRUE)
			{
				xFactoryModeCode = IN_FACTORY_MODE;
				xSemaphoreGive(xMutexFactoryMode);
			}
		}
	}
	else
	{
		wdt_counter_reload();
		vTaskResume(xWatchdogTaskHandle);			   // 恢�?�app模式看门狗任�?
		vTaskSuspend(xFactoryModeWatchDogTaskHandler); // 挂起产测模式看门狗任�?
		vTaskResume(main_process_task_handle);
		vTaskResume(hpw6862i_task_handle);
		vTaskResume(xTaskMultiButtonHandler);
		vTaskResume(hp203b_task_handle);
		vTaskResume(valva_control_task_handle);
		if (xGetFactoryModeCode() != IN_APP_MODE)
		{
			if (xSemaphoreTake(xMutexFactoryMode, 100) == pdTRUE) // 不�?�于APP模式进入APP模式
			{
				xFactoryModeCode = IN_APP_MODE;
				xSemaphoreGive(xMutexFactoryMode);
			}
		}
	}
	return 0;
}

FACTORY_MODE_t xGetFactoryModeCode(void)
{
	FACTORY_MODE_t status = IN_UNKOWN_MODE;
	if (xSemaphoreTake(xMutexFactoryMode, 100))
	{
		status = xFactoryModeCode;
		xSemaphoreGive(xMutexFactoryMode);
	}
	return status;
}