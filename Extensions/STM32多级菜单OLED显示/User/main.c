/**
  ******************************************************************************
  * @file    main.c 
  * @brief   多级菜单实现
  * @attention  建议使用 MDK V5 编译, V6可能会出bug
  *
  *
  *    
  ******************************************************************************
  */

#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "Menu.h"

int main(void)
{
  Systick_Init();
  /*  待机模式使能  */
  RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN, ENABLE);
  PWR_WakeUpPinCmd(ENABLE);

  key_init();
  PWM_LED_Init(); 
  Timer6_Init();
  OLED_Init();   
  TIM_Cmd(TIM6, DISABLE);

  while(1)  
  {
    /*  定时器开启延时，防止按键唤醒信号为  key_enter  */
    delay_ms(500);
    TIM_Cmd(TIM6, ENABLE);
    
    main_menu();
    /*  清屏待机  */
    OLED_Clear();
    OLED_Update();
    PWR_EnterSTANDBYMode();
  }  
}
