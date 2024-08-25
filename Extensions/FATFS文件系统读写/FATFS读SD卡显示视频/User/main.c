/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *          1.首先把视频通过opencv切片并压缩成128*64尺寸的图片(如果尺寸太大了，
  *          ram一帧图片都放不下)
  *          2.通过Image2LCD取模软件批量生成二进制文件,取模不勾包含文件头数据选
  *          项，扫描模式(数据水平，字节垂直),输出灰度(单色),勾选(字节内像素数据
  *          反序),选择颜色反转。
  *          3.在Linux系统中使用以下命令进行二进制文件合并，生成的二进制文件下直
  *          接粘贴该命令
  *          for file in `ls -v *.ebm`; do  cat $file >> ../BadApple.bin; done
  *          ls -v 查看二进制文件排列顺序是否按照图片顺序排列的(这一步很重要，
  *          搞不定就乱码，如果顺序默认已经排列好，直接合并即可
  *          cat *.ebm > ../BadApple.bin 即可)
  *          4.最后烧录代码搞定 
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
#define ImageSize   1024
uint8_t ReadBuffer[ImageSize];   

/**
 * @brief 通过使用Fatfs，读取SD中存放的视频并显示在OLED上
 * 
 */
void SD_Card_OledShowVedio(void)
{
  int offset = 0, file_size=0;
  f_mount(&fs, "0:", 1); 					//挂载SD卡 
  /*  1.打开文件  */
	res = f_open(&fnew, "0:BadApple.bin", FA_OPEN_EXISTING | FA_READ); 	 
  if(res!=FR_OK)  led_ctrl(led_on);
  /*  2.计算这个文件大小  */
  file_size = f_size(&fnew);
  /*  3.初始化偏移量为0  */
  offset = 0 ;
  /*  4.计算bin文件里总共包含多少帧图片，然后不断给OLED显示  */
  for(int i = 0 ; i < file_size / ImageSize ; i++)
  {
      /*  5.读取一张图片,一张图片的大小是ImageSize，
          将读取出来的图片存放到缓存区ReadBuffer数组   */
      res = f_read(&fnew, ReadBuffer, ImageSize, &fnum);
      if(res!=FR_OK)
      {
          led_ctrl(led_on);
          f_close(&fnew);
      }  
      /*  6.将整个数据赋值给显示接口，显示图片  */ 
      OLED_ShowImage(0, 0, 128, 64, (uint8_t *)ReadBuffer);
      OLED_Update();
      /*  根据显示情况，调整刷新速度  */
      delay_ms(100);
      /*  7.将偏移往后加ImageSisze,就到了下一张图片  */
      offset += ImageSize;
      res = f_lseek(&fnew, offset);
      if(FR_OK != res)  led_ctrl(led_on);
  }
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
  
  SD_Card_OledShowVedio();

  while(1)
  {    
    delay_ms(100);
  }  
}
