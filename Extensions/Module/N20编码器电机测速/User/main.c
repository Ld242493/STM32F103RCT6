/**
  ******************************************************************************
  * @file    main.c 
  * @brief   电机速度计算流程
  *           1、计算电机每秒脉冲数
  *           2、计算电机每转脉冲数
  *           3、计算电机转速
  *           4、根据转速与选择的轮子计算速度
  *    
  ******************************************************************************
  */
 
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "LCD/lcd.h"
#include "Encoder/encoder.h"


/*  编码器检测电机每转一圈产生的脉冲数  */
#define   PPR   28    //7（电机线数）x 4（编码器模式3）

int ppr1s, rpm;
float speed;
uint8_t buf[50];

int main(void)
{
  LED_Init();
  PWMLED_TIM1_Init();
  KEY_TIM6_Init();
  Motor_TIM2_Init();
  Encoder_TIM3_Init();
  LCD_Init();

  LCD_Clear(BLACK);
  LCD_ShowString(20, 0, "Encoder Test");

  while(1)
  {
    
    sprintf(buf, "Speed: %.2fm/s    ", speed);    
    LCD_ShowString(0, 40, buf);

    delay_ms(200);
  }  
}


void TIM2_IRQHandler(void)
{
  static uint16_t Cnt1 = 0;
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);     
    if(++Cnt1 == 1000)
    {
      Cnt1 = 0;
      ppr1s = Motor_ReadSpeed();   //每秒脉冲数
      rpm = ppr1s * 60 / PPR / 50; //电机转速(RPM) = 每秒总脉冲数 x 60 / 每转脉冲数 / 减速比（50）
      /*  选用D轴车轮，直径43mm  */
      speed = rpm / 60  * 3.14 * 0.043;   //周长(3.14 x 0.043m) x 转每秒（RPM / 60) = 车速
    }
  }
}