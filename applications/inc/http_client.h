#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__
#include "stdint.h"

#define HTTP_RSP_RCV 0x10

#define HTTP_DL_START 0x01
#define HTTP_DL_OK    0x02
#define HTTP_DL_ERR   0x04

typedef int32_t (*HttpFileRcvCb)(uint32_t package_index, uint8_t *data, uint16_t len);

int32_t http_client_get_file(const char *host, uint16_t port, const char *path, uint32_t file_size,  HttpFileRcvCb cb);

#endif // __HTTP_CLIENT_H__