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
#include "RTC/rtc.h"

uint8_t buf[30];

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  RTC_Configuration();
  OLED_Init();

  OLED_ShowString(10, 0, "RTC Show demo", OLED_8X16);
  Time_Adjust(11, 1, 55);
  Time_Display(RTC_GetCounter(), buf);
  OLED_ShowString(0, 30, buf, OLED_8X16);  
  OLED_Update();  

  while(1)
  {
    if (TimeDisplay)
    {
      TimeDisplay = 0;
      Time_Display(RTC_GetCounter(), buf);
      OLED_ShowString(0, 30, buf, OLED_8X16);
      OLED_Update();
    }
  }  
}
