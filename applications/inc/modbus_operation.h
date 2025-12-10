#ifndef _MODBUS_OPERATION_H
#define _MODBUS_OPERATION_H

#include "stdbool.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#include "hpw6862i.h"
#include "hp203b.h"
#include "user_task.h"
#include "gt_uart.h"
#include "gt_flash.h"
#include "soft_i2c.h"
#include "bt_device.h"
#include "utils.h"
#include "delay.h"

#include "gt_adc.h"
#include "i2c_application.h"


#define AIR_PRESSURE_REQUEST 41
#define WRITE_SN_REQUEST 27
#define AT_COMMAND_REQUEST 44
#define SEND_FIRMWARE_REQUEST 31
#define SEND_DATA_REQUEST 01
#define START_UPGRADE_REQUEST 32

#define BUFFER_SIZE 256

// 定义错误码
#define OPERATION_SUCCESS 0
#define OPERATION_FAILURE 1
#define OPERATION_SUCCESS_AND_REBOOT 2
#define OPERATION_SUCCESS_HAS_NEXT 3

// 宏定义三色LED的端口和引脚
#define LED_RED_PORT (GPIOB)
#define LED_RED_PIN (GPIO_PINS_14)

#define LED_GREEN_PORT (GPIOB)
#define LED_GREEN_PIN (GPIO_PINS_13)

#define LED_BLUE_PORT (GPIOB)
#define LED_BLUE_PIN (GPIO_PINS_3)

#define BUTTON_PORT (GPIOB)
#define BUTTON_PIN (GPIO_PINS_2)

#define VALVE_CONTROL_PORT_A_B (GPIOB)
#define VALVE_CONTROL_PORT_A_B_PIN (GPIO_PINS_12)

#define VALVE_CONTROL_PORT_B_A (GPIOB)
#define VALVE_CONTROL_PORT_B_A_PIN (GPIO_PINS_5)

#define REED_SWITCH_PORT (GPIOB)
#define REED_SWITCH_PIN (GPIO_PINS_15)


// 定义处理函数类型
typedef uint8_t (*HandlerFunc)(uint16_t register_address, uint8_t * param, uint8_t param_len, uint8_t * result, uint8_t *result_len);

typedef struct {
    uint8_t function_code;
    uint16_t register_address;
    HandlerFunc handler;  // 处理函数指针
} RegisterOperation;

typedef struct {
    RegisterOperation *operations;
    size_t capacity;
    size_t count;
} RegisterManager;


HandlerFunc modbus_get_handle_func(uint8_t function_code, uint16_t register_address);


int32_t register_manager_init(size_t initial_capacity);
int32_t register_manager_add(uint8_t function_code,
                             uint16_t register_address,
                             HandlerFunc handler);
#endif

