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
#include "LCD/Image.h"

uint8_t num;

int main(void)
{
  Systick_Init();
  LED_Init();
  KEY_TIM6_Init();
  LCD_Init();

  LCD_Clear(BLACK);
  
  lcddev.fontColor = RED;
  lcddev.fontSize = FontSize16;
  lcddev.bgColor  = BLACK;
  LCD_ShowString(0, 0, "TEST");
  LCD_ShowNum(40, 0, 16, 2);
  LCD_ShowChinese(60, 0, "中文测试");
  LCD_DrawRectangle(0, 0, 125, 16);

  lcddev.fontColor = MAGENTA;
  lcddev.fontSize = FontSize24;
  lcddev.bgColor  = LIGHTBlUE;
  LCD_ShowString(0, 30, "TEST");
  LCD_ShowNum(60, 30, 24, 2);
  LCD_ShowChinese(90, 30, "中文测试");

  lcddev.fontColor = GREEN;
  lcddev.fontSize = FontSize12;
  lcddev.bgColor  = BLACK;
  LCD_ShowString(0, 60, "TEST");
  LCD_ShowNum(30, 60, 12, 2);
  LCD_ShowChinese(50, 60, "中文测试");   

  // LCD_ShowImage(0, 0, 128, 160, gImage);

  while(1)
  {
    if(keyVal == Key_Up) LED_Toggle();
    else if(keyVal == Key_Enter)  num++;
    else if(keyVal == Key_Down)   num--;
    keyVal = Key_None;
    LCD_ShowNum(0, 80, num, 3);
    delay_ms(50);
  }  
}


