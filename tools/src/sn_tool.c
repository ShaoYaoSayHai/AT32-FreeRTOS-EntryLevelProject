
#include "FreeRTOS.h"
#include "task.h"
#include "sn_tool.h"

char scReadSNBuffer[50];

/**
 * @brief SN组成函数，将header填入其中拼接出新的SN
 *
 * @param header 1220000668
 * @param sn     CE02_251126_10001
 * @param result
 */
void concate_new_array(char *header, char *sn, char *result)
{
    strcpy(result, header);
    strcat(result, sn);
}

/**
 * @brief 生成新的SN号
 *
 * @param result 输出结果
 * @param func  读取FLASH的SN的函数
 * @return int
 */
int generate_server_sn(char *result, FuncReadSN func)
{
    // 读取设备序列号
    int timeout = 0;
    int status = gt_flash_read_sn2(scReadSNBuffer, 17);
    while (!status && (timeout < READ_SN_TIMEOUT_CNT))
    {
        timeout++;
        status = gt_flash_read_sn2(scReadSNBuffer, 17);
        vTaskDelay(100);
    }
    if (timeout >= READ_SN_TIMEOUT_CNT)
    {
        return FALSE;
    }
    // 拼接新的SN
    concate_new_array("1220000668", scReadSNBuffer, result);
    return TRUE;
}
