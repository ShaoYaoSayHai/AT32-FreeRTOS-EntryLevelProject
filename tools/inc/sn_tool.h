
#ifndef _SN_TOOL_H_
#define _SN_TOOL_H_

#include "stdio.h"
#include "string.h"
#include "gt_flash.h"

#define READ_SN_TIMEOUT_CNT 20

typedef int32_t (*FuncReadSN)(char *sn , uint32_t len);

/**
 * @brief SN组成函数，将header填入其中拼接出新的SN
 *
 * @param header 1220000668
 * @param sn     CE02_251126_10001
 * @param result
 */
void concate_new_array(char *header, char *sn, char *result);

/**
 * @brief 生成新的SN号
 *
 * @param result 输出结果
 * @param func  读取FLASH的SN的函数
 * @return int
 */
int generate_server_sn(char *result, FuncReadSN func);

#endif
