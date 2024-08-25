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
  OLED_Init();
  OLED_ShowString(10, 0, "OLED Show demo", OLED_8X16);
  OLED_ShowChinese(0, 20, "你好，世界。");
  OLED_Update();  
  while(1)
  {
    switch (KeyVal)
    {
      case Key_Enter:
        led_toggle();
        break;  
    }

    KeyVal = Key_None;

    delay_ms(100);
  }  
}
