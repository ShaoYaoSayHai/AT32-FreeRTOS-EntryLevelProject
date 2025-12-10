#ifndef __BOOT_H__
#define __BOOT_H__
#include "gt_flash.h"

#define BOOT_FLAG_NEWAPP        1
#define BOOT_FLAG_SECONDOK      2
#define BOOT_FLAG_SECONDTRY     3
#define BOOT_FLAG_FIRST         4
#define BOOT_FLAG_BACKFIRST     5

typedef struct
{
  struct
  {	
    uint8_t   BootFlag;         //启动标志
    uint8_t   BootNum;         //启动次数
    uint8_t   isOTA;           //是否是OTA升级标志
    uint16_t  backup_fw_crc;
    uint32_t  backup_fw_size;
  }BootMsg;
  uint8_t crc;
}BOOT_DATA;

void gt_inner_flash_write(uint32_t addr, uint8_t *pbuffer, uint32_t num);
void gt_inner_flash_read(uint32_t addr, uint8_t *pbuffer, uint32_t num);
void gt_inner_flash_sector_erase(uint32_t addr);
void gt_inner_flash_blank2_erase();
void ModifyBootStatus( uint8_t flag );
void setOTA(uint8_t isOTA);
uint8_t isOTA(void);
void setStartOk(void);
int32_t backupApp(void);

#endif /* __BOOT_H__ */
