/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *          1.���Ȱ���Ƶͨ��opencv��Ƭ��ѹ����128*64�ߴ��ͼƬ(����ߴ�̫���ˣ�
  *          ramһ֡ͼƬ���Ų���)
  *          2.ͨ��Image2LCDȡģ����������ɶ������ļ�,ȡģ���������ļ�ͷ����ѡ
  *          �ɨ��ģʽ(����ˮƽ���ֽڴ�ֱ),����Ҷ�(��ɫ),��ѡ(�ֽ�����������
  *          ����),ѡ����ɫ��ת��
  *          3.��Linuxϵͳ��ʹ������������ж������ļ��ϲ������ɵĶ������ļ���ֱ
  *          ��ճ��������
  *          for file in `ls -v *.ebm`; do  cat $file >> ../BadApple.bin; done
  *          ls -v �鿴�������ļ�����˳���Ƿ���ͼƬ˳�����е�(��һ������Ҫ��
  *          �㲻�������룬���˳��Ĭ���Ѿ����кã�ֱ�Ӻϲ�����
  *          cat *.ebm > ../BadApple.bin ����)
  *          4.�����¼����㶨 
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
/*  128 x 64 /8 = 1024, ÿ�����ص�ռ1���ֽ�8λ����ɫͼƬռ2���ֽ�16λ  */
#define ImageSize   1024
uint8_t ReadBuffer[ImageSize];   

/**
 * @brief ͨ��ʹ��Fatfs����ȡSD�д�ŵ���Ƶ����ʾ��OLED��
 * 
 */
void SD_Card_OledShowVedio(void)
{
  int offset = 0, file_size=0;
  f_mount(&fs, "0:", 1); 					//����SD�� 
  /*  1.���ļ�  */
	res = f_open(&fnew, "0:BadApple.bin", FA_OPEN_EXISTING | FA_READ); 	 
  if(res!=FR_OK)  led_ctrl(led_on);
  /*  2.��������ļ���С  */
  file_size = f_size(&fnew);
  /*  3.��ʼ��ƫ����Ϊ0  */
  offset = 0 ;
  /*  4.����bin�ļ����ܹ���������֡ͼƬ��Ȼ�󲻶ϸ�OLED��ʾ  */
  for(int i = 0 ; i < file_size / ImageSize ; i++)
  {
      /*  5.��ȡһ��ͼƬ,һ��ͼƬ�Ĵ�С��ImageSize��
          ����ȡ������ͼƬ��ŵ�������ReadBuffer����   */
      res = f_read(&fnew, ReadBuffer, ImageSize, &fnum);
      if(res!=FR_OK)
      {
          led_ctrl(led_on);
          f_close(&fnew);
      }  
      /*  6.���������ݸ�ֵ����ʾ�ӿڣ���ʾͼƬ  */ 
      OLED_ShowImage(0, 0, 128, 64, (uint8_t *)ReadBuffer);
      OLED_Update();
      /*  ������ʾ���������ˢ���ٶ�  */
      delay_ms(100);
      /*  7.��ƫ�������ImageSisze,�͵�����һ��ͼƬ  */
      offset += ImageSize;
      res = f_lseek(&fnew, offset);
      if(FR_OK != res)  led_ctrl(led_on);
  }
	/* ���ٶ�д���ر��ļ� */
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
