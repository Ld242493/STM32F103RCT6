/**
  ******************************************************************************
  * @file    main.c 
  * @brief   通过使用定时器输入捕获来测量PWM周期，频率，占空比
  *
  *
  *    
  ******************************************************************************
  */
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "pwm_capture.h"

__IO uint16_t DutyCycle = 0;
__IO uint32_t Frequency = 0;
__IO uint32_t Period = 0;

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
 void TIM3_IRQHandler(void)
 { 
   static __IO uint16_t IC1Value, IC2Value;
   //捕获到上升沿进入一次中断 
   if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
   {  
     /* Clear TIM3 Capture compare interrupt pending bit */
     TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    
     /* Get the Input Capture value */
     IC2Value = TIM_GetCapture2(TIM3) + 1;

     /* 占空比 = 高电平计数值/总周期计数值 X 100%  */
     DutyCycle =  IC1Value*100 / IC2Value;   
     /* 捕获的PWM周期 = 总的计数值 X 单个计数所需要的时间  */
     Period = IC2Value; // 72/72 = 1Mhz = 1us，表示计一个数需要的时间
     /* 周期us转换为s计算PWM捕获频率 */
     Frequency = 1000000/Period;      
   }
   //捕获到下降沿进入一次中断
   if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
   {  
     /* Clear TIM3 Capture compare interrupt pending bit */
     TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

     /* Get the Input Capture value */
     IC1Value = TIM_GetCapture1(TIM3) + 1;
     if(IC1Value > IC2Value) {IC1Value = 0xFFFF - IC2Value + IC1Value;}
   }
 }

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();
  PWM_Capture_Init();
  PWM_Output_Init();
  OLED_ShowString(20,  0, "PWM Capture", OLED_8X16);
  OLED_ShowString(0, 20, "Freq:       HZ", OLED_8X16);
  OLED_ShowString(0, 40, "Duty:   %", OLED_8X16);
  OLED_Update();  
  while(1)
  {
    OLED_ShowNum(45, 20, Frequency, 6, OLED_8X16);
    OLED_ShowNum(45, 40, DutyCycle, 2, OLED_8X16);
    OLED_Update();
    delay_ms(100);
  }  
}
