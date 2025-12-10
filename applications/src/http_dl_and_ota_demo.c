#include "boot.h"
#include "gt_flash.h"
#include "http_client.h"
#include "version_info.h"
#include "flash_partition.h"

int32_t http_out_rcv_cb(uint32_t package_index, uint8_t *data, uint16_t len) {
    return gt_flash_write_fw_with_cache(package_index, data, len);
}


void http_dl_and_ota_demo() {
   OTA_FW_INFO info = {FW_TYPE_MCU, 78256, {0x06,0x8f,0x96,0xbc,0x41,0x15,0x27,0xcd,0xc0,0xe9,0x73,0x82,0x12,0x06,0xcb,0xa3}, "V101SP01"};
   gt_flash_write_ota_fw_info(&info);//写入OTA信息到flash中
   spiflash_sector_erase_with_lock(DOWNLOAD_ADDR, 0x20000); //擦除下载区，此处没有全部擦除，可以更具固件大小来擦除
   int32_t res = http_client_get_file("223.166.186.230", 12348, "/wm200_1.0.3.bin", 78256, http_out_rcv_cb); //下载固件文件
   if(res != 0) {
       return;
   }
   res = gt_flash_check_fw_md5(); //校验固件MD5是否正确
   if(res != 0) {
       return;
   }
   res = backupApp(); //备份当前固件到备份区
   if(res != 0) {
       return;
   }
   setOTA(1); //设置OTA标志位，下次启动时bootloader会更新新固件
   nvic_system_reset(); //重启设备，进入bootloader更新固件
}

void check_ota_success_demo(void) {
    if(isOTA()) {//说明是经过OTA的，需要检查下版本号是否对
        OTA_FW_INFO info = {0};
        gt_flash_read_ota_fw_info(&info);
        if(!strcmp(info.fwVersion, "V101SP01")) {
             //上报OTA成功
        }else {
             //上报OTA失败
        }
        setOTA(0); //清除OTA标志位，下次启动就不要检查这个标志了
    }
}