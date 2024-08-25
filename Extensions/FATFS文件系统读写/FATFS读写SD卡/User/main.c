/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *
  *
  *    
  ******************************************************************************
  */
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "SD_CARD/sd_card.h"
#include "FATFS/ff.h"

FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res;

uint32_t SD_CardTotalSize, SD_CardFreeSize;

uint8_t WriteBuffer[20] = "test data";
uint8_t ReadBuffer[20];


void FatFs_WriteTest(void)
{
  res = f_mount(&fs, "0:", 1);
  if(res == FR_NO_FILESYSTEM)   //没有文件系统，格式化
  {
    res = f_mkfs("0:", 1, 0);
    if(FR_OK == res)
    {
      f_mount(NULL, "0:", 1);   //取消挂载
      f_mount(&fs, "0:", 1);    //重新挂载
    }
    else  led_ctrl(led_on);
  }
  /*  写数据  */
  if(FR_OK == f_open(&fnew, "0:FatFs.txt", FA_OPEN_ALWAYS | FA_WRITE))
  {
    f_write(&fnew, WriteBuffer, 20, &fnum);
    OLED_ShowString(0, 35, "Write: ", OLED_8X16);
    OLED_ShowString(55, 35, WriteBuffer, OLED_8X16);
    f_close(&fnew);
  }
  else  led_ctrl(led_on);
    /*  取消挂载  */
  f_mount(NULL, "0:", 1);
}

void FatFs_ReadTest(void)
{
  res = f_mount(&fs, "0:", 1);
  /*  读数据  */
  if(FR_OK == f_open(&fnew, "0:FatFs.txt", FA_OPEN_EXISTING | FA_READ))
  {
    f_read(&fnew, ReadBuffer, 20, &fnum);
    OLED_ShowString(0, 50, "Read: ", OLED_8X16);
    OLED_ShowString(55, 50, ReadBuffer, OLED_8X16);
    f_close(&fnew);
  }
  else  led_ctrl(led_on); 

  f_mount(NULL, "0:", 1);  
}

/**
 * @brief 获取SD_Card内存
 * 
 * @param path 磁盘编号
 * @param total 总内存
 * @param free 剩余内存
 * @return uint8_t 
 */
uint8_t Fatfs_GetStorage(FATFS *fs, uint8_t *path, uint32_t *total, uint32_t *free)
{
  FRESULT res;
  /*  空闲簇，空闲扇区，总扇区  */
  uint32_t free_clust, free_sector, total_sector;
  /*  获取内存前需要先挂载  */
  f_mount(fs, (const TCHAR*)path, 1);

  res = (uint32_t)f_getfree((const TCHAR*)path, (DWORD*)&free_clust, &fs);
  if (!res)
  {
    total_sector = (fs->n_fatent - 2) * fs->csize;  //计算总扇区
    free_sector = free_clust * fs->csize;   //计算空闲扇区

    #if __MAX_SS != 512
    total_sector *= fs->ssize / 512;
    free_sector  *= fs->ssize / 512;
    #endif

    *total = total_sector >> 1;   //单位为KB
    *free  = free_sector  >> 1;
  }

  f_mount(NULL, (const TCHAR*)path, 1);
  return res;
}

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();
  
  OLED_ShowString(20, 0, "SD Card Demo", OLED_8X16);
  Fatfs_GetStorage(&fs, "0:", &SD_CardTotalSize, &SD_CardFreeSize);
  sprintf(ReadBuffer, "T: %d MB F: %d MB", SD_CardTotalSize/1024, SD_CardFreeSize/1024);
  OLED_ShowString(0, 20, ReadBuffer, OLED_6X8);
  FatFs_WriteTest();
  FatFs_ReadTest(); 
  OLED_Update();   

  while(1)
  {    
    delay_ms(100);
  }  
}
