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
#include "LCD/lcd.h"
#include "HX711/hx711.h"

float weight;
uint8_t buf[50];

int main(void)
{
  LED_Init();
  PWMLED_TIM1_Init();
  KEY_TIM6_Init();
  HX711_Init();
  LCD_Init();

  LCD_Clear(BLACK);
  LCD_ShowString(30, 0, "HX711 Test");

  while(1)
  {
    weight = HX711_GetWeight();
    sprintf(buf, "Weight: %.2fg    ", weight);    
    LCD_ShowString(0, 40, buf);

    delay_ms(200);
  }  
}


