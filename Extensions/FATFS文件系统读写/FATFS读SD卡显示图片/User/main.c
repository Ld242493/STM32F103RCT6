/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *          1.������һ��128*64�ߴ��ͼƬ(����ߴ�̫���ˣ�ramһ��ͼƬ���ݶ��Ų���)
  *          2.ͨ��Image2LCDȡģ������ɶ������ļ�,ȡģ���������ļ�ͷ����ѡ�
  *          ɨ��ģʽ(����ˮƽ���ֽڴ�ֱ),����Ҷ�(��ɫ),��ѡ(�ֽ����������ݷ���),
  *          ѡ����ɫ��ת�� 
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
uint8_t ReadBuffer[1024];   

/**
 * @brief ͨ��ʹ��Fatfs����ȡSD�д�ŵ�ͼƬ����ʾ��OLED��
 * 
 */
void SD_Card_OledShowImgae(void)
{
  f_mount(&fs,"0:",1); 					//����SD�� 

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
  
  SD_Card_OledShowImgae();

  while(1)
  {    
    delay_ms(100);
  }  
}
