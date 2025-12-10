#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__
#include "stdint.h"

#define TX_BUFFER_SIZE 512

typedef enum {
    MQTT_CONNECT_SUCCESS = 0, //连接成功
    MQTT_CONNECT_RETRY = 1, //正在重连
    MQTT_CONNECT_CLIENT_DISC = 2, //客户端断开
    MQTT_CONNECT_SERVE_REJECT = 3, //服务器拒绝连接
    MQTT_CONNECT_SERVER_DISC = 4, //服务器断开连接
    MQTT_CONNECT_NETWORK_ABNOARMAL = 5, //网络异常
    MQTT_CONNECT_CONNECTING = 10, //正在连接
    MQTT_CONNECT_UNKNOWN = 255, //未知错误
}MqttConnectStatus;

//MQTT的参数
typedef struct {
  char client_id[32]; //client id，和user name一样    
  char password[50]; //mqtt的password  
  char mqtt_server[32]; //mqtt服务器的地址    
  uint16_t server_port; //mqtt服务器的端口
}MqttInfo;

extern MqttConnectStatus mqttstatus;
extern MqttInfo mqttinfo;
/*@brief 回调函数，MQTT收到对方publish的数据；
 *@param 
 * topic:收到的消息的topic
 * msg: 收到的消息主体
 * len: 收到的消息主体的长度
 */
typedef void (*MqttMsgRcvcCb)(char *topic, uint8_t *msg, uint8_t len);

//初始化mqtt client
//@param mqtt_info: mqtt的参数
//@param msg_rcv_cb: 收到消息的回调函数
int32_t mqtt_client_init(MqttInfo *mqtt_info, MqttMsgRcvcCb msg_rcv_cb);

//连接mqtt服务器，
//@return 0: 成功，其他值: 失败，这里只是返回模组收到了连接的消息，并不代表已经连接成功了。
int32_t mqtt_client_connect(void);

int32_t mqtt_client_disconnect(void);

//订阅topic
//@param topic: 要订阅的topic
//@return 0: 成功，其他值: 失败，这里只是返回了模组收到了订阅的消息，并不代表已经订阅成功了。
int32_t mqtt_client_subscribe(char *topic);

//发布消息
//@param topic: 要发布的topic
//@param msg: 要发布的消息主体
//@param timeout_ms: 超时时间，单位毫秒
//@return 0: 成功，其他值: 失败，这里的结果是服务器返回的结果，是最终的结果。
int32_t mqtt_client_publish(char *topic, char *msg, uint32_t timeout_ms);

//获取mqtt连接状态
//@return mqtt连接状态
MqttConnectStatus mqtt_client_get_connect_status(void);

//获取订阅状态
//@return 0: 未订阅，1: 已订阅
uint8_t mqtt_client_get_sub_status(void);




#endif // __MQTT_CLIENT_H__