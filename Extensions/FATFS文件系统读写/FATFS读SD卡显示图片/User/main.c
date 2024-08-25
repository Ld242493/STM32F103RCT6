/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *          1.首先找一张128*64尺寸的图片(如果尺寸太大了，ram一张图片数据都放不下)
  *          2.通过Image2LCD取模软件生成二进制文件,取模不勾包含文件头数据选项，
  *          扫描模式(数据水平，字节垂直),输出灰度(单色),勾选(字节内像素数据反序),
  *          选择颜色反转。 
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
/*  128 x 64 /8 = 1024, 每个像素点占1个字节8位，彩色图片占2个字节16位  */
uint8_t ReadBuffer[1024];   

/**
 * @brief 通过使用Fatfs，读取SD中存放的图片并显示在OLED上
 * 
 */
void SD_Card_OledShowImgae(void)
{
  f_mount(&fs,"0:",1); 					//挂载SD卡 

	res = f_open(&fnew, "0:Pic.ebm", FA_OPEN_EXISTING | FA_READ); 	 
  if(res!=FR_OK)  led_ctrl(led_on);
  res = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
  if(res!=FR_OK)
  {
      led_ctrl(led_on);
      f_close(&fnew);
      return;
  }
  OLED_ShowImage(0, 0, 128, 64, ReadBuffer);
  OLED_Update();
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
}


int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();
  
  SD_Card_OledShowImgae();

  while(1)
  {    
    delay_ms(100);
  }  
}
