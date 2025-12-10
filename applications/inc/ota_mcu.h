#ifndef _OTA_MCU_H
#define _OTA_MCU_H

#include "user_source.h"

typedef struct
{
	int ota_type;
	char host[50];
	int port;
	char path[50];
	char version[20];
	int file_size;
	char md5[40];
}ota_param;


void ota_task(void *pvParameters);
extern TaskHandle_t ota_task_handle;
extern QueueHandle_t ota_cmd_queue;

bool hex_string_to_bytes(const char *hex_str, unsigned char *byte_array, size_t byte_len);
bool string_to_port(const char *port_str, uint16_t *rcport);
void check_ota_success(void);

bool convert_https_to_http(char *url, size_t url_size);
#endif