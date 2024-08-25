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
#include "W25Q16/w25q16.h"
#include "FATFS/ff.h"


FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res;

uint8_t WriteBuffer[20] = "test data";
uint8_t ReadBuffer[20];

void FatFs_WriteTest(void)
{
  res = f_mount(&fs, "1:", 1);
  if(res == FR_NO_FILESYSTEM)   //没有文件系统，格式化
  {
    res = f_mkfs("1:", 1, 0);
    if(FR_OK == res)
    {
      f_mount(NULL, "1:", 1);   //取消挂载
      f_mount(&fs, "1:", 1);    //重新挂载
    }
    else  led_ctrl(led_on);
  }
  /*  写数据  */
  if(FR_OK == f_open(&fnew, "1:FatFs.txt", FA_OPEN_ALWAYS | FA_WRITE))
  {
    f_write(&fnew, WriteBuffer, 20, &fnum);
    OLED_ShowString(0, 30, "Write: ", OLED_8X16);
    OLED_ShowString(55, 30, WriteBuffer, OLED_8X16);
    f_close(&fnew);
  }
  else  led_ctrl(led_on);
    /*  取消挂载  */
  f_mount(NULL, "1:", 1);
}

void FatFs_ReadTest(void)
{
  res = f_mount(&fs, "1:", 1);
  /*  读数据  */
  if(FR_OK == f_open(&fnew, "1:FatFs.txt", FA_OPEN_EXISTING | FA_READ))
  {
    f_read(&fnew, ReadBuffer, 20, &fnum);
    OLED_ShowString(0, 50, "Read: ", OLED_8X16);
    OLED_ShowString(55, 50, ReadBuffer, OLED_8X16);
    f_close(&fnew);
  }
  else  led_ctrl(led_on);
  f_mount(NULL, "1:", 1);
}

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();

  OLED_ShowString(20, 0, "FATFS Demo", OLED_8X16);
  FatFs_WriteTest();
  FatFs_ReadTest();
  OLED_Update();  

  while(1)
  {    
    delay_ms(100);
  }  
}
