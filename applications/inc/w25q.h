#ifndef _W25Q_H
#define _W25Q_H

#include "user_source.h"

// W25Q指令集
#define W25Q_WRITE_ENABLE      0x06
#define W25Q_WRITE_DISABLE     0x04
#define W25Q_READ_JEDEC_ID     0x9F
#define W25Q_READ_DATA         0x03
#define W25Q_PAGE_PROGRAM      0x02
#define W25Q_SECTOR_ERASE      0x20
#define W25Q_READ_STATUS_REG1  0x05

// W25Q参数
#define W25Q_PAGE_SIZE         256
#define W25Q_SECTOR_SIZE       4096
#define W25Q_MANUFACTURER_ID   0xEF


// 函数声明
void W25Q_Init(void);
uint8_t W25Q_ReadSR(void);
void W25Q_WriteEnable(void);
void W25Q_WaitBusy(void);
void W25Q_EraseSector(uint32_t addr);
void W25Q_ReadData(uint32_t addr, uint8_t *buf, uint16_t len);
void W25Q_PageWrite(uint32_t addr, uint8_t *buf, uint16_t len);
//void Store_data(uint16_t Year, uint8_t Month, uint8_t Day, uint8_t Hour, 
//                uint8_t Minute, uint8_t Second, char *state, float err_num);
//uint16_t Read_W25Q_data(void (*process_record)(char *));

uint32_t W25Q_ReadID(void);
//void process_log(char *record);

bool Read_clientID(char *userid_buf);

void Read_calValue(float *cal1,float *cal2);


#endif

