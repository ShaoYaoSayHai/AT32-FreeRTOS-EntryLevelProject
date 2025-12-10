#ifndef _KEY_H
#define _KEY_H
#include "user_source.h"

//#define KEY_UP    0
//#define KEY_DOWN  1

//#define KEY_PB2_CHANGE_VALVE_VALUE    0x01
//#define KEY_PB2_SECURITY_CHECK_VALUE  0x02
//#define KEY_PB2_ALL_CLEAR_VALUE       0x03


//#define KEY_NO         0xFF

//#define TIME_100MS     10
//#define TIME_2S        200
//#define TIME_5S        500
//#define DEBOUNCE_TIME  3

//#define READ_PB2_STATUS  gpio_input_data_bit_read(GPIOB,GPIO_PINS_2)

extern TaskHandle_t key_task_handle;

//uint8_t key_scan(void);

// 按键事件类型
typedef enum {
    BUTTON_EVENT_NONE = 0,      // 无事件
    BUTTON_EVENT_CLICK,         // 单击
    BUTTON_EVENT_DOUBLE_CLICK,  // 双击
    BUTTON_EVENT_LONG_PRESS,    // 长按
    BUTTON_EVENT_LONG_HOLD,     // 长按保持
    BUTTON_EVENT_RELEASE,       // 释放
} button_event_t;

// 按键状态
typedef enum {
    BUTTON_STATE_RELEASED = 0,  // 释放状态
    BUTTON_STATE_PRESSED,       // 按下状态
    BUTTON_STATE_DEBOUNCE,      // 消抖状态
} button_state_t;

// 按键配置结构体
typedef struct {
    gpio_type *port;           // GPIO端口
    uint32_t pin;              // GPIO引脚
    uint8_t active_level;      // 有效电平 (0:低电平有效, 1:高电平有效)
    uint16_t debounce_time;    // 消抖时间(ms)
    uint16_t click_timeout;    // 单击超时时间(ms)
    uint16_t long_press_time;  // 长按判定时间(ms)
    uint16_t double_click_gap; // 双击间隔时间(ms)
} button_config_t;

// 按键句柄结构体
typedef struct button_handle_t {
    button_config_t config;     // 配置参数
    button_state_t state;       // 当前状态
    button_event_t last_event;  // 上次事件
    
    // 时间记录
    TickType_t press_start_time;    // 按下开始时间
    TickType_t release_start_time;  // 释放开始时间
    uint8_t click_count;            // 点击计数
    
    // 回调函数
    void (*event_callback)(struct button_handle_t *btn, button_event_t event);
    void *user_data;                // 用户数据
    
    // FreeRTOS相关
    TaskHandle_t task_handle;       // 任务句柄
    QueueHandle_t event_queue;      // 事件队列
} button_handle_t;

// API函数
uint8_t button_read_pin(button_handle_t *btn);
button_handle_t *button_create(const button_config_t *config);
void button_delete(button_handle_t *btn);
void button_set_callback(button_handle_t *btn, 
                         void (*callback)(button_handle_t *btn, button_event_t event),
                         void *user_data);
button_event_t button_get_event(button_handle_t *btn);
uint8_t button_is_pressed(button_handle_t *btn);
												 
void button_event_handler(button_handle_t *btn, button_event_t event);

												 
void creat_button_task(void);
// 默认配置
#define BUTTON_DEFAULT_CONFIG(port, pin) { \
    .port = port, \
    .pin = pin, \
    .active_level = 0, \
    .debounce_time = 20, \
    .click_timeout = 300, \
    .long_press_time = 1000, \
    .double_click_gap = 300, \
}												 
#endif