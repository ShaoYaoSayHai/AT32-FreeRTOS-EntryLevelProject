#include "mqtt_business_task.h"
#include "mobile_monitor.h"
#include "http_client.h"
#include "gt_flash.h"
#include "flash_partition.h"
#include "mobile_device.h"
#include "sn_tool.h"
#include "drv_led.h"

#include "user_task.h"
#include "led_colour.h"
#include "watchdog_task.h"

void mqtt_write_msg_type(SemaphoreHandle_t *signal, char *buffer);

TaskHandle_t mqtt_business_task_handle;
MqttMsgRcvc_t g_mqtt_msg_rcv = {0};
MQTT_BUSINESS_STATUS_t g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
uint8_t retry_time = 0;
// TOPIC
char subtopic[25] = "device_control", pubtopic[25] = "oako";

volatile bool snedto4g_flag = false;

void mqtt_demo_msg_rcv_cb(char *topic, uint8_t *msg, uint8_t len)
{
    g_mqtt_msg_rcv.new_msg = 1;
    strcpy(g_mqtt_msg_rcv.topic, topic);
    //    memcpy(g_mqtt_msg_rcv.msg, msg, len);

    for (int i = 0; i < 256; i++)
    {
        g_mqtt_msg_rcv.msg[i] = 0;
    }
    for (int i = 0; i < len; i++)
    {
        g_mqtt_msg_rcv.msg[i] = msg[i];
    }

    g_mqtt_msg_rcv.len = len;
    g_mqtt_business_status = MQTT_BUSINESS_MSG_RCVED;
}

void mqtt_business_check_connect_status(void)
{
    MqttConnectStatus connect_status = mqtt_client_get_connect_status();
    if (connect_status == MQTT_CONNECT_SUCCESS)
    {
        g_mqtt_business_status = MQTT_BUSINESS_CONNECTED;
        retry_time = 0;
    }
    else if (connect_status == MQTT_CONNECT_RETRY || connect_status == MQTT_CONNECT_CONNECTING)
    {
        g_mqtt_business_status = MQTT_BUSINESS_CONNECTING;
        retry_time++;
        if (retry_time >= MQTT_BUSSINESS_RETRY_TIME_MAX || !get_network_ready())
        {
            g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
        }
    }
    else
    {
        g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
    }
}

void mqtt_business_connect(void)
{
    retry_time = 0;
    int32_t ret;
    if (!get_network_ready())
    {
        g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
        return;
    }
    ret = mqtt_client_connect();
    if (ret == 0)
    {
        g_mqtt_business_status = MQTT_BUSINESS_CONNECTING;
    }
    else
    {
        g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
    }
}

void mqtt_business_subscribe(void)
{
    //   int32_t ret = mqtt_client_subscribe("device_control");
    int32_t ret = mqtt_client_subscribe(subtopic);
    if (ret == 0)
    {
        g_mqtt_business_status = MQTT_BUSINESS_SUBING;
        retry_time = 0;
    }
    else
    {
        retry_time++;
        if (retry_time >= MQTT_BUSSINESS_RETRY_TIME_MAX || !get_network_ready())
        {
            g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
            retry_time = 0;
        }
    }
}

void mqtt_business_check_subscribe_status(void)
{
    uint8_t subscribe_status = mqtt_client_get_sub_status();
    if (subscribe_status)
    {
        g_mqtt_business_status = MQTT_BUSINESS_READY;
        retry_time = 0;
    }
    else
    {
        retry_time++;
        if (retry_time >= MQTT_BUSSINESS_RETRY_TIME_MAX)
        {
            g_mqtt_business_status = MQTT_BUSINESS_CONNECTED;
            retry_time = 0;
        }
        if (!get_network_ready() || mqtt_client_get_connect_status() != MQTT_CONNECT_SUCCESS)
        {
            g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
            retry_time = 0;
            return;
        }
    }
}

// char px_pub_buffer[] = "{\"type\":1,\"serialNumber\":\"CE02_251126_10100\",\"collectStatus\":1,\"press\":3.1415926,\"tof\":0.0,\"temperature\":0.0,\"flow\":0.0}";
bool first_on = true;

#if 1
void mqtt_business_run(void)
{
    static uint8_t errorCnts = 0;
    int32_t publishStatus = 0x00;

    if (first_on)
    {      
        if (hpw6862i_task_handle)
        {
            vTaskResume(hpw6862i_task_handle);
        }
        if (main_process_task_handle)
        {
            vTaskResume(main_process_task_handle);
        }
        if (xWatchdogTaskHandle)
        {
            vTaskResume(xWatchdogTaskHandle);
        }
        TaskHandle_t *pxTaskHandler = pxTakeStartWdtTaskHandler();
        if (*pxTaskHandler)
        {
            vTaskDelete(*pxTaskHandler);
        }
        first_on = false;
    }

    if (!(get_network_ready()) || mqtt_client_get_connect_status() != MQTT_CONNECT_SUCCESS)
    {
        g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
        retry_time = 0;
        return;
    }
    else
    {
        mqtt_write_msg_type(&mqtt_type1_send_signal, pcTakeTypeBuffer1());
        mqtt_write_msg_type(&mqtt_type2_send_signal, pcTakeTypeBuffer2());
        mqtt_write_msg_type(&mqtt_type3_send_signal, pcTakeTypeBuffer3());
    }
}
#else

void mqtt_business_run(void)
{
    static uint8_t send_time = 3;
    if (!(get_network_ready()) || mqtt_client_get_connect_status() != MQTT_CONNECT_SUCCESS)
    {
        g_mqtt_business_status = MQTT_BUSINESS_UNKNOWN;
        retry_time = 0;
        return;
    }
    else
    {
        if (send_time > 0)
        {
            int32_t ret = mqtt_client_publish("oako", "{\"data\":123456 , \"func\":\"123\"}", 10000);
            led_show_null();
            if (ret == 0)
            {
                // do something you need
            }
            else
            {
                // do something you need
                led_show_yellow();
            }
            send_time--;
        }
    }
}
// register

#endif

void mqtt_business_process_rcv_msg(void)
{
    if (g_mqtt_msg_rcv.new_msg)
    {
        // TODO do something you need
        process_mqtt_message(g_mqtt_msg_rcv.msg, g_mqtt_msg_rcv.len);
        g_mqtt_business_status = MQTT_BUSINESS_READY;
        g_mqtt_msg_rcv.new_msg = 0;
    }
}
#if 1

void mqtt_business_task(void *pvParameters)
{
    // led_show_white_blink();
    led_show_purple_blink();

    /*************读取序列�?************/
    gt_flash_read_sn2(Device_serial_number.sn2, 32);
    /*************读取序列�?************/

    /***********读取mqtt参数*********/
    gt_flash_read_mqtt_server_param(&mqtt_param);
    /***********读取mqtt参数*********/

    /***************拼接前缀给mqttinfo*****************/
    snprintf(mqttinfo.client_id, 32, "1220000668%s", Device_serial_number.sn2);
    /***************拼接前缀给mqttinfo*****************/

    for (int i = 0; i < 50; i++)
    {
        mqttinfo.password[i] = mqtt_param.mqtt_password[i];
    }

    mqttinfo.server_port = mqtt_param.mqtt_port;

    for (int j = 0; j < 32; j++)
    {
        mqttinfo.mqtt_server[j] = mqtt_param.mqtt_url[j];
    }

    //    //TODO 参数从flash里面获取
    //    MqttInfo mqtt_client_connect_info = {
    //        .client_id = "wxw1",
    //        .password = "wxw1",
    //        .mqtt_server = "223.166.186.227",
    //        .server_port = 12351
    //    };

    /***************用于调试*********************/
    //    MqttInfo mqtt_client_connect_info = {
    //        .client_id = "1220000668CE02_251126_10300",
    //        .password = "aujdS-Vk3oiMO0JaOWf4VbVAc3homdgD1FIVpV0AtNo",
    //        .mqtt_server = "chongqing-mqtt.ctwing.cn",
    //        .server_port = 1883};
    //    strcpy(Device_serial_number.sn2, "CE02_251126_10100");
    /***************用于调试*********************/
    while (!get_network_ready())
    {
        vTaskDelay(1000);
    }
    // generate_server_sn(mqtt_client_connect_info.client_id, gt_flash_read_sn2);

    mqtt_client_init(&mqttinfo, mqtt_demo_msg_rcv_cb);
    // mqtt_client_init(&mqtt_client_connect_info, mqtt_demo_msg_rcv_cb);
    while (1)
    {
        switch (g_mqtt_business_status)
        {
        case MQTT_BUSINESS_UNKNOWN: // MQTT�?知错�?
            mqtt_business_connect();
            break;
        case MQTT_BUSINESS_CONNECTING: // MQTT连接�?
            mqtt_business_check_connect_status();
            break;
        case MQTT_BUSINESS_CONNECTED: // MQTT连接
            mqtt_business_subscribe();
            break;
        case MQTT_BUSINESS_SUBING: // MQTT订阅�?
            mqtt_business_check_subscribe_status();
            break;
        case MQTT_BUSINESS_READY: // 任务准�?�就�?
            mqtt_business_run();
            break;
        case MQTT_BUSINESS_MSG_RCVED: // 收到服务器消�?
            mqtt_business_process_rcv_msg();
            break;
        default:
            break;
        }
        vTaskDelay(1000);
    }
}

#else
char http_dl[] = "AT+HTTPFOTA=223.166.186.227,12348,/GDC100_V105_update_V106.fwpkg.bin,0\r\n";
char tmp_rsp[128] = {0};
void mqtt_business_task(void *pvParameters)
{
    MqttInfo mqtt_client_connect_info = {
        .client_id = "1220000668CE02_250929_20244",
        .password = "aujdS-Vk3oiMO0JaOWf4VbVAc3homdgD1FIVpV0AtNo",
        .mqtt_server = "chongqing-mqtt.ctwing.cn",
        .server_port = 1883};
    while (!get_network_ready())
    {
        vTaskDelay(1000);
    }
    mobile_dev_send_cmd(http_dl, tmp_rsp, sizeof(tmp_rsp));
    while (!get_network_ready())
    {
        vTaskDelay(1000);
    }
    while (1)
    {
        vTaskDelay(1000);
    }
}

#endif

/**

char http_dl[]= "AT+HTTPFOTA=oss.oakocq.com,80,/GDC100_V105_update_V106.fwpkg.bin,0\r\n";
char tmp_rsp[128]= {0};
mobile_dev_send_cmd(http_dl, tmp_rsp, sizeof(tmp_rsp));

 */

// uint32_t rcv_count = 0;
// uint32_t total_rcv_size = 0;
// uint32_t rcv_package_index[100] = {0};
// int32_t http_out_rcv_cb(uint32_t package_index, uint8_t *data, uint16_t len) {
//     rcv_package_index[rcv_count] = package_index;
//     rcv_count ++;
//     total_rcv_size += len;
//     return gt_flash_write_fw_with_cache(package_index, data, len);
// }

// void http_dl_and_ota_demo() {
//    rcv_count = 0;
//    total_rcv_size = 0;
//    memset(rcv_package_index, 0, sizeof(rcv_package_index));
//    OTA_FW_INFO info = {FW_TYPE_MCU, 44840, {0x9f,0xde,0x33,0xb6,0x42,0x3c,0x02,0xcc,0x04,0x42,0xdf,0xb2,0x41,0x92,0x12,0xba}, "V101SP01"};
//    gt_flash_write_ota_fw_info(&info);//д��OTA��Ϣ��flash��
//    spiflash_sector_erase_with_lock(DOWNLOAD_ADDR, DOWNLOAD_SIZE); //�������������˴�û��ȫ�����������Ը��߹̼���С������
//    int32_t res = http_client_get_file("oss.oakocq.com", 80, "/updates/GDC100_V105_update.fwpkg.bin", 44840, http_out_rcv_cb); //���ع̼��ļ�
//    if(res != 0) {
//        return;
//    }
//    res = gt_flash_check_fw_md5(); //У��̼�MD5�Ƿ���ȷ
//    if(res != 0) {
//        return;
//    }
// }

void mqtt_write_msg_type(SemaphoreHandle_t *signal, char *buffer)
{
    static uint8_t errorCnts = 0;
    static int32_t publishStatus = 0x00;
    // �ź�������
    if (xSemaphoreTake(*signal, 30) != pdFALSE)
    {
        if (xSemaphoreTake(xMutexMqttWriteBufferProtectHandler, 100) != pdFALSE)
        {
            publishStatus = mqtt_client_publish(pubtopic, buffer, 1500);
            xSemaphoreGive(xMutexMqttWriteBufferProtectHandler);
        }
        // publishStatus = mqtt_client_publish(pubtopic, buffer, 1500);
        if (publishStatus == 0)
        {
            errorCnts = 0;
        }
        else
        {
            errorCnts++;
            if (errorCnts >= 3)
            {
                // led_show_yellow();
            }
            return;
        }
    }
}