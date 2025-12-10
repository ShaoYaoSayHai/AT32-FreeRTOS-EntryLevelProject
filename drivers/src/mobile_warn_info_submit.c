#include "queue.h"
#include "mobile_warn_info_submit.h"

#define BUFFER_SIZE 256
#define POOL_SIZE 10

typedef struct BaseWarnMessage_t
{
    char cBuffer[BUFFER_SIZE];
    uint8_t ucUsed; // 是否被使用
} BaseWarnMessage_t;

typedef struct WarnMsgPtr_t
{
    BaseType_t xPoolIndex; // 存储池数组下标
} WarnMsgPtr_t;

BaseWarnMessage_t PoolWarnInffo[POOL_SIZE]; // 在全局区初始化，默认全0（即FALSE）

SemaphoreHandle_t xPoolMutex = NULL;
QueueHandle_t xQueue = NULL;

// 初始化函数，在任务创建前调用
void vInitQueueAndMutex(void)
{
    // 创建队列和互斥信号量
    xQueue = xQueueCreate(POOL_SIZE, sizeof(WarnMsgPtr_t));
    xPoolMutex = xSemaphoreCreateMutex();

    for (int i = 0; i < POOL_SIZE; i++)
    {
        PoolWarnInffo[i].ucUsed = pdFALSE;
        memset(PoolWarnInffo[i].cBuffer, 0, BUFFER_SIZE);
    }
}

void vSendTaskDemo(void *pxParameter)
{
    WarnMsgPtr_t xWarnMsgPtr;

    while (1)
    {
        // 查找可用的缓冲区
        BaseType_t xIndex;
        BaseType_t xFound = pdFALSE;

        // 查找空闲缓冲区，需要互斥保护
        if (xSemaphoreTake(xPoolMutex, portMAX_DELAY) == pdTRUE)
        {
            for (xIndex = 0; xIndex < POOL_SIZE; xIndex++)
            {
                if (PoolWarnInffo[xIndex].ucUsed == pdFALSE)
                {
                    PoolWarnInffo[xIndex].ucUsed = pdTRUE; // 标记为已使用
                    xFound = pdTRUE;
                    break;
                }
            }
            xSemaphoreGive(xPoolMutex);
        }

        if (xFound)
        {
            // 获取缓冲区后，先填充需要发送的数据
            // 在消息中直接传递索引，而灭有用指针，我担心地址不连续导致错误
            xWarnMsgPtr.xPoolIndex = xIndex;

            if (xQueueSend(xQueue, &xWarnMsgPtr, pdMS_TO_TICKS(100)) != pdPASS)
            {
                // 发送失败，释放缓冲区
                if (xSemaphoreTake(xPoolMutex, portMAX_DELAY) == pdTRUE)
                {
                    PoolWarnInffo[xIndex].ucUsed = pdFALSE;
                    xSemaphoreGive(xPoolMutex);
                }
            }
            // 如果发送成功，缓冲区由接收任务释放
        }
        else
        {
            // 内存池已满
        }
    }
}

void vRecvTaskDemo(void *pxParameter)
{
    WarnMsgPtr_t xReceiveMsg;

    while (1)
    {
        if (xQueueReceive(xQueue, &xReceiveMsg, portMAX_DELAY) == pdPASS)
        {
            // 使用传递过来的索引
            BaseType_t xIndex = xReceiveMsg.xPoolIndex;
            // 处理接收到的数据，数据在 PoolWarnInffo[xIndex].cBuffer 中
            // 处理完毕后，归还缓冲区
            if (xSemaphoreTake(xPoolMutex, portMAX_DELAY) == pdTRUE)
            {
                PoolWarnInffo[xIndex].ucUsed = pdFALSE;
                xSemaphoreGive(xPoolMutex);
            }
        }
    }
}