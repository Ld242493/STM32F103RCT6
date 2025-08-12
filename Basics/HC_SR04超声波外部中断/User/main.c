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
#include "HC_SR04/Ultrasonic.h"

uint8_t buf[50];

int main(void)
{
  LED_Init();
  KEY_Init();
  TIM6_Init();
  OLED_Init();

  Ultrasonic_TIM7_Init();
  
  while(1)
  {

    Ultrasonic_Start();
    sprintf(buf, "dis: %.2fcm   ", distance);
    OLED_ShowString(10, 0, buf, OLED_8X16);
    OLED_Update(); 

    delay_ms(200);
  }  
}


/**
  * @brief  This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)
  { 
    /* Clear the  EXTI line 1 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line1);
    /*  检测到高电平，从0开始计数  */
    TIM_SetCounter(TIM7, 0);
    TIM_Cmd(TIM7, ENABLE);
    /*  等待高电平时间  */
    while(GPIO_ReadInputDataBit(HC_SR04_PORT, ECHO_PIN));
    TIM_Cmd(TIM7, DISABLE);

    /*  定时器配置100us计数一次，6.5左右秒溢出  */
    distance = TIM_GetCounter(TIM7) * 3.43f / 2;  //  总计数时间 x 100us x 光速0.0343cm/us / 来回次数2
  }
}