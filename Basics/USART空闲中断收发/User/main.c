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


int main(void)
{
  Systick_Init();
  LED_Init();
  KEY_Init();
  TIM6_Init();
  USART_Configuration();
  OLED_Init();
  
  printf("你好，世界\r\n");
  OLED_ShowString(8, 0, "USART IDLE demo", OLED_8X16);
  OLED_Update(); 

  while(1)
  {
    if (RxFlag)
    {
      RxFlag = 0;
      printf("%s\n", RxBuffer);
      memset(RxBuffer, 0, sizeof(RxBuffer));
    }
    delay_ms(100);
  }  
}
