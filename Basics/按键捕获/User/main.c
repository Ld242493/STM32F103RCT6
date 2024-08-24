/**
  ******************************************************************************
  * @file    main.c 
  * @brief   通过使用定时器捕获来测量按键高电平时间，实现长按或短按识别
  *
  *
  *    
  ******************************************************************************
  */
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"

void TIM5_IRQHandler(void)
{
  /*  CaptureNber 捕获次数，第一次捕获表示按键按下，第二次表示按键松开
  *               0表示第一次，1表示第二次  
  *   CaptureOK   表示捕获完成一次，即按键按下松开一次
  *   Cnt 定时器溢出计数，65.535ms 一次
  *   CaptureVal 当前捕获值
  *   total_val  高电平时间总的计数值，也是高电平总的时间单位为us
  */
  static uint8_t CaptureNber, CaptureOK, Cnt; 
  static uint16_t CaptureVal;
  static uint32_t total_val;
  if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    if(CaptureOK == 0 && CaptureNber == 1)//从第一次捕获开始
    {
      Cnt ++; //定时器溢出次数加1, 1次 = 65535us
    }
    else if(CaptureOK == 1)
    {
      CaptureOK = 0;
      total_val = CaptureVal+Cnt*65535;
      if((total_val/1000) <= 200) //超过0.2s 为长按，否则为短按
        Key_Val = Key_Enter;
      else
        Key_Val = Key_Enter_long;
      OLED_ShowNum(75,20, total_val, 6, OLED_8X16);
      OLED_Update();
    }
  }
  if(TIM_GetITStatus(TIM5, TIM_IT_CC1) == SET)
  {
    TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
    if(CaptureOK == 0)
    {
      if(CaptureNber == 1) //第二次捕获(release)
      { 
        CaptureOK = 1;  //标记捕获完成，停止计数
        CaptureNber = 0;
        CaptureVal = TIM_GetCapture1(TIM5);
        TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Rising);//设置再次按下检测
      }
      else    //第一次捕获(press)
      {
        Cnt = 0;
        CaptureVal = 0;
        CaptureNber = 1;  //开始捕获   
        TIM_SetCounter(TIM5, 0);//计数从0开始
        TIM_OC1PolarityConfig(TIM5, TIM_ICPolarity_Falling);//设置松手检测
      }
    }
  }    
}

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  key_capture_init();
  OLED_Init();
  OLED_ShowString(0, 0, "Key Capture demo", OLED_8X16);
  OLED_ShowString(0, 20, "Key_time:", OLED_8X16);
  OLED_ShowString(0, 50, "Time: (1 ~ 999999)us", OLED_6X8);
  OLED_Update();  
  while(1)
  {
    switch (key_GetVal())
    {
    case Key_Enter_long:
      led_ctrl(led_on);
      break;
    case Key_Enter:
      led_ctrl(led_off);
      break;
    }
    delay_ms(100);
  }  
}
