#include "key.h"

#include "user_task.h"


//uint8_t key_scan(void)
//{
//	static bool keyPA1_prev        = true;
//	static bool keyPA1_stable      = true;
//	static uint8_t  keyPA1_debounce = 0;
//	static uint16_t keyPA1_duration = 0;
//	
//	bool keyPA1_current = READ_PB2_STATUS;
//	uint8_t ret         = KEY_NO;
//	
//	if(keyPA1_current != keyPA1_stable)
//	{
//		keyPA1_debounce++;
//		if(keyPA1_debounce >= DEBOUNCE_TIME)
//		{
//			keyPA1_stable   = keyPA1_current;
//			keyPA1_debounce = 0;
//		}		
//	}
//	else
//	{
//		keyPA1_debounce = 0;
//	}
//	
//	if(!keyPA1_stable)
//	{
//		keyPA1_duration++;
//	}
//	else if(!keyPA1_prev)
//	{
//		if((keyPA1_duration >= TIME_100MS)&&(keyPA1_duration <TIME_2S))
//		{
//			ret = KEY_PB2_CHANGE_VALVE_VALUE;
//		}
//		else if((keyPA1_duration >= TIME_2S)&&(keyPA1_duration <TIME_5S))
//		{
//			ret = KEY_PB2_SECURITY_CHECK_VALUE;
//		}
//		else if(keyPA1_duration >= TIME_5S)
//		{
//			ret = KEY_PB2_ALL_CLEAR_VALUE;
//		}
//		keyPA1_duration=0;
//	}
//	
//	keyPA1_prev = keyPA1_stable;
//	
//	return ret;	
//}

TaskHandle_t key_task_handle;

// 读取按键物理状态
 uint8_t button_read_pin(button_handle_t *btn)
{
    // 读取GPIO引脚状态
    uint8_t pin_state = gpio_input_data_bit_read(btn->config.port, btn->config.pin);
    
    // 根据有效电平返回逻辑状态
    // 如果配置为低电平有效，则实际低电平时返回1（按下）
    if (btn->config.active_level == 0) {
        return (pin_state == RESET) ? 1 : 0;
    } else {
        return (pin_state == SET) ? 1 : 0;
    }
}

// 按键扫描任务
static void button_scan_task(void *param)
{
    button_handle_t *btn = (button_handle_t *)param;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(5);  // 5ms扫描周期
    
    for (;;) {
        uint8_t current_pin_state = button_read_pin(btn);
        TickType_t current_time = xTaskGetTickCount();
        TickType_t elapsed_time;
        
        switch (btn->state) {
            case BUTTON_STATE_RELEASED:
                if (current_pin_state) {  // 检测到按下
                    btn->state = BUTTON_STATE_DEBOUNCE;
                    btn->press_start_time = current_time;
                }
                break;
                
            case BUTTON_STATE_DEBOUNCE:
                elapsed_time = (current_time - btn->press_start_time) * portTICK_PERIOD_MS;
                
                if (elapsed_time >= btn->config.debounce_time) {
                    if (current_pin_state) {  // 确认按下
                        btn->state = BUTTON_STATE_PRESSED;
                        btn->press_start_time = current_time;
                        btn->click_count = 0;
                        
                        // 发送按下事件
                        if (btn->event_queue) {
                            button_event_t event = BUTTON_EVENT_NONE;
                            xQueueSend(btn->event_queue, &event, 0);
                        }
                    } else {
                        btn->state = BUTTON_STATE_RELEASED;
                    }
                }
                break;
                
            case BUTTON_STATE_PRESSED:
                if (!current_pin_state) {  // 检测到释放
                    elapsed_time = (current_time - btn->press_start_time) * portTICK_PERIOD_MS;
                    
                    if (elapsed_time < btn->config.long_press_time) {
                        // 短按释放
                        btn->click_count++;
                        btn->release_start_time = current_time;
                        btn->state = BUTTON_STATE_RELEASED;
                        
                        // 发送释放事件
                        if (btn->event_queue) {
                            button_event_t event = BUTTON_EVENT_RELEASE;
                            xQueueSend(btn->event_queue, &event, 0);
                        }
                    }
                } else {
                    // 检查长按
                    elapsed_time = (current_time - btn->press_start_time) * portTICK_PERIOD_MS;
                    
                    if (elapsed_time >= btn->config.long_press_time) {
                        // 发送长按事件
                        if (btn->event_queue) {
                            button_event_t event = BUTTON_EVENT_LONG_PRESS;
                            xQueueSend(btn->event_queue, &event, 0);
                            btn->last_event = BUTTON_EVENT_LONG_PRESS;
                            
                            // 回调函数
                            if (btn->event_callback) {
                                btn->event_callback(btn, BUTTON_EVENT_LONG_PRESS);
                            }
                        }
                        
                        // 进入长按保持状态
                        btn->state = BUTTON_STATE_RELEASED;
                    }
                }
                break;
        }
        
        // 检查双击
        if (btn->state == BUTTON_STATE_RELEASED && btn->click_count > 0) {
            elapsed_time = (current_time - btn->release_start_time) * portTICK_PERIOD_MS;
            
            if (elapsed_time >= btn->config.click_timeout) {
                // 单击超时
                if (btn->click_count == 1) {
                    // 发送单击事件
                    if (btn->event_queue) {
                        button_event_t event = BUTTON_EVENT_CLICK;
                        xQueueSend(btn->event_queue, &event, 0);
                        btn->last_event = BUTTON_EVENT_CLICK;
                        
                        // 回调函数
                        if (btn->event_callback) {
                            btn->event_callback(btn, BUTTON_EVENT_CLICK);
                        }
                    }
                } else if (btn->click_count == 2) {
                    // 发送双击事件
                    if (btn->event_queue) {
                        button_event_t event = BUTTON_EVENT_DOUBLE_CLICK;
                        xQueueSend(btn->event_queue, &event, 0);
                        btn->last_event = BUTTON_EVENT_DOUBLE_CLICK;
                        
                        // 回调函数
                        if (btn->event_callback) {
                            btn->event_callback(btn, BUTTON_EVENT_DOUBLE_CLICK);
                        }
                    }
                }
                
                btn->click_count = 0;
            } else if (btn->click_count >= 2) {
                // 已经检测到两次点击，且在双击时间间隔内
                // 发送双击事件
                if (btn->event_queue) {
                    button_event_t event = BUTTON_EVENT_DOUBLE_CLICK;
                    xQueueSend(btn->event_queue, &event, 0);
                    btn->last_event = BUTTON_EVENT_DOUBLE_CLICK;
                    
                    // 回调函数
                    if (btn->event_callback) {
                        btn->event_callback(btn, BUTTON_EVENT_DOUBLE_CLICK);
                    }
                }
                
                btn->click_count = 0;
            }
        }
        
        // 精确延时
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

// 创建按键
button_handle_t *button_create(const button_config_t *config)
{
    button_handle_t *btn = pvPortMalloc(sizeof(button_handle_t));
    if (!btn) return NULL;
    
    // 复制配置
    memcpy(&btn->config, config, sizeof(button_config_t));
    
    // 初始化状态
    btn->state = BUTTON_STATE_RELEASED;
    btn->last_event = BUTTON_EVENT_NONE;
    btn->press_start_time = 0;
    btn->release_start_time = 0;
    btn->click_count = 0;
    btn->event_callback = NULL;
    btn->user_data = NULL;
    
    // 创建事件队列
    btn->event_queue = xQueueCreate(10, sizeof(button_event_t));
    
    // 创建扫描任务
	  
//	  taskENTER_CRITICAL();
    xTaskCreate(button_scan_task, 
                "ButtonScan", 
                128,
                btn, 
                5,  // 中等优先级
                &key_task_handle);
//    taskEXIT_CRITICAL();
    return btn;
}

// 删除按键
void button_delete(button_handle_t *btn)
{
    if (!btn) return;
    
    // 删除任务
    if (key_task_handle) {
        vTaskDelete(key_task_handle);
    }
    
    // 删除队列
    if (btn->event_queue) {
        vQueueDelete(btn->event_queue);
    }
    
    // 释放内存
    vPortFree(btn);
}

// 设置回调函数
void button_set_callback(button_handle_t *btn, 
                         void (*callback)(button_handle_t *btn, button_event_t event),
                         void *user_data)
{
    if (!btn) return;
    
    btn->event_callback = callback;
    btn->user_data = user_data;
}

// 获取事件
button_event_t button_get_event(button_handle_t *btn)
{
    if (!btn || !btn->event_queue) return BUTTON_EVENT_NONE;
    
    button_event_t event;
    if (xQueueReceive(btn->event_queue, &event, 0) == pdPASS) {
        btn->last_event = event;
        return event;
    }
    
    return BUTTON_EVENT_NONE;
}

// 检查按键是否按下
uint8_t button_is_pressed(button_handle_t *btn)
{
    if (!btn) return 0;
    return (btn->state == BUTTON_STATE_PRESSED);
}


void creat_button_task(void)
{
	button_config_t btn_config;
	btn_config.port=GPIOB;
	btn_config.pin=GPIO_PINS_2;
	btn_config.active_level = 0;      // 低电平有效（按下为低）
	btn_config.debounce_time = 20;    // 20ms消抖
	btn_config.click_timeout = 200;   // 单击超时300ms
	btn_config.long_press_time = 3000;// 长按1秒
	btn_config.double_click_gap = 200;// 双击间隔300ms
	
	button_handle_t *button_pb2 = button_create(&btn_config);
	button_set_callback(button_pb2, button_event_handler, NULL);
}

//按键事件回调函数
void button_event_handler(button_handle_t *btn, button_event_t event)
{
    switch (event) {
        case BUTTON_EVENT_CLICK:
//            printf("单击事件\r\n");
//            led_toggle();  // 单击切换LED
				
				if (xSemaphoreGive(bt_change_valve_sem) == pdTRUE)
				{
					
				}
            break;
            
        case BUTTON_EVENT_DOUBLE_CLICK:
//            printf("双击事件\r\n");
//            // 双击可以做其他事情
				if (xSemaphoreGive(bt_security_check_sem) == pdTRUE)
				{
					
				}
            break;
            
        case BUTTON_EVENT_LONG_PRESS:
//            printf("长按事件\r\n");
//            // 长按复位或其他功能
				
				xSemaphoreTake(bt_change_valve_sem, 0);
				xSemaphoreTake(bt_security_check_sem, 0);
				happen_warn = false;
            break;
            
        case BUTTON_EVENT_LONG_HOLD:
//            printf("长按保持\r\n");
            break;
            
        case BUTTON_EVENT_RELEASE:
//            printf("释放事件\r\n");
            break;
            
        default:
            break;
    }
}