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
  led_init();
  key_init();
  Timer6_Init();
  USART_Configuration();
  OLED_Init();
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
