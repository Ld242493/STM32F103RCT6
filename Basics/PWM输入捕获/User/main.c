/**
  ******************************************************************************
  * @file    main.c 
  * @brief   ͨ��ʹ�ö�ʱ�����벶��������PWM���ڣ�Ƶ�ʣ�ռ�ձ�
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
   //���������ؽ���һ���ж� 
   if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
   {  
     /* Clear TIM3 Capture compare interrupt pending bit */
     TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    
     /* Get the Input Capture value */
     IC2Value = TIM_GetCapture2(TIM3) + 1;

     /* ռ�ձ� = �ߵ�ƽ����ֵ/�����ڼ���ֵ X 100%  */
     DutyCycle =  IC1Value*100 / IC2Value;   
     /* �����PWM���� = �ܵļ���ֵ X ������������Ҫ��ʱ��  */
     Period = IC2Value; // 72/72 = 1Mhz = 1us����ʾ��һ������Ҫ��ʱ��
     /* ����usת��Ϊs����PWM����Ƶ�� */
     Frequency = 1000000/Period;      
   }
   //�����½��ؽ���һ���ж�
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
