#ifndef __GT_FLASH_H__
#define __GT_FLASH_H__

#include <stdint.h>
#include <time.h>
#include "rtc.h"

#define WARNING_MAGIC_NUM 0x4754574E
#define LOG_MAGIC_NUM     0x47544C47

#define FW_TYPE_MCU       0x47544150
#define FW_TYPE_MOBILE    0x47544D44
#define FW_TYPE_BT        0x47544254

#define LOG_ONE_LINE_SIZE 128

typedef struct {
    char sn1[32]; // 大板序列号
    char sn2[32]; // 整机序列号
} DeviceInfo;

typedef struct {
    DeviceInfo devInfo;
    uint8_t crc;
} DeviceInfoWithCrc;

typedef struct {
    char mqtt_url[128];
    uint32_t mqtt_port;
    char mqtt_password[64];
} MqttServerParam;

typedef struct {
    int32_t precal1;
    int32_t precal2;
} CalibrationParam;

typedef struct {
    MqttServerParam mqttParam;
    CalibrationParam calParam;
} ConfigParam;

typedef struct {
    ConfigParam configParam;
    uint8_t crc;
} ConfigParamWithCrc;



typedef enum{
    status_no_error = 0 ,  // 无异常
    status_leak ,          // 泄漏
    status_over_flow ,     // 过流
    status_over_press ,    // 超压
    status_insufficient_press , // 欠压 
    status_over_temp ,     // 超温
    status_unkown_error ,  // 未知
}DeviceCollectionStatus_t ; 

typedef struct {
    calendar_type time;
    DeviceCollectionStatus_t status;
} WarningInfo;

typedef struct {
    uint32_t magicNum;
    uint32_t warningIndex;
    WarningInfo warningInfo;
    uint8_t crc;
} WarningInfoWithCrc;

#define WARNING_INFO_WITH_CRC_SIZE 32



typedef struct {
    uint32_t fwType;
    uint32_t fwSize;
    uint8_t  fwMd5[16];
    char fwVersion[12];
}OTA_FW_INFO;

typedef struct {
    OTA_FW_INFO info;
    uint8_t crc;
}OTA_FW_INFO_DATA;

#define OTA_FW_SEND_BUFF_SIZE 2048


typedef struct {
    OTA_FW_INFO fwInfo;
    uint32_t fwPackageIndex;
    uint32_t fwRcvSize;
    uint16_t cache_len;
    uint8_t cache_buff[OTA_FW_SEND_BUFF_SIZE];
} OTA_FW_DLOAD_INFO;


extern CalibrationParam cal_param;
extern MqttServerParam  mqtt_param;
extern DeviceInfo  Device_serial_number;
extern WarningInfo warning;


//初始化SPI FLASH，在使用SPI FLASH之前，需要先调用这个接口；
void gt_flash_init(void);

//读取SPI FLASH数据
void spiflash_read_with_lock(uint8_t *pbuffer, uint32_t read_addr, uint32_t length);

//写入SPI FLASH数据
void spiflash_write_with_lock(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);

//擦除指定区域flash
void spiflash_sector_erase_with_lock(uint32_t erase_addr, uint32_t erase_size);

//获取SPI LFASH ID
uint16_t spiflash_read_flash_id_with_lock();

//获取SN1，这是单板SN号
int32_t gt_flash_read_sn1(char *sn1, uint32_t len);

//获取SN2， 这是整机SN号
int32_t gt_flash_read_sn2(char *sn2, uint32_t len);

//写SN1，这是单板SN号
int32_t gt_flash_write_sn1(const char *sn1);

//写SN2，这是整机SN号
int32_t gt_flash_write_sn2(const char *sn2);

//读取MQTT配置参数
int32_t gt_flash_read_mqtt_server_param(MqttServerParam *param);

//读取校准参数
int32_t gt_flash_read_calibration_param(CalibrationParam *param);

//写MQTT配置参数
int32_t gt_flash_write_mqtt_server_url(const char *mqtt_url);

int32_t gt_flash_write_mqtt_server_port(uint32_t port);

int32_t gt_flash_write_mqtt_password(const char *password);

//写校准参数
int32_t gt_flash_write_calc_param(const CalibrationParam *param);

int32_t gt_flash_write_precal1(int32_t precal1);

int32_t gt_flash_write_precal2(int32_t precal2);

//写警告信息
int32_t gt_flash_write_warning_info(WarningInfo *info);

//获取当前警告信息
int32_t gt_flash_get_current_warning_info(WarningInfo *info);

//清除当前警告信息
void gt_flash_clear_current_warning_info(void);

//清除所有警告信息
void gt_flash_clear_all_warning_info(void);

//通过索引获取警告信息
int32_t gt_flash_read_warning_info_by_index(WarningInfo *info, uint32_t index);

//写日志信息，参照printf格式
void gt_flash_write_log(const char *__format, ...);

//清除所有日志信息
void gt_flash_clear_all_log(void);

//获取当前日志组总个数，第一个日志组索引，最后一个日志组索引
void gt_flash_log_system_get_info(uint32_t *total_groups, uint32_t *first_group, uint32_t *last_group);

//通过日志组索引以及组内偏移量获取日志信息，group_id类似文件描述符，offset类似文件偏移量
int32_t gt_flash_read_log(uint32_t group_id, uint32_t offset, char *buffer, uint32_t buffer_size);

int32_t gt_flash_write_ota_fw_info(OTA_FW_INFO *info);

int32_t gt_flash_read_ota_fw_info(OTA_FW_INFO *info);

int32_t gt_flash_write_fw(uint32_t pos, uint8_t *fw, uint32_t size);

int32_t gt_flash_read_fw(uint32_t pos, uint8_t *fw, uint32_t size);

int32_t gt_flash_write_fw_with_cache(uint16_t package_index, uint8_t *fw, uint32_t size);

int32_t gt_flash_check_fw_md5();

#endif