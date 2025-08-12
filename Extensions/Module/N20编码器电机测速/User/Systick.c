#include "Systick.h"

// static __IO uint32_t TimingDelay;

// void Systick_Init(void)
// {
//   while(SysTick_Config(SystemCoreClock / 1000));   
// }

// /**
//   * @brief  Decrements the TimingDelay variable.
//   * @param  None
//   * @retval None
//   */
// void TimingDelay_Decrement(void)
// {
//   if (TimingDelay != 0x00)
//   { 
//     TimingDelay--;
//   }
// }

// /**
//   * @brief  Inserts a delay time.
//   * @param  nTime: specifies the delay time length, in milliseconds.
//   * @retval None
//   */
// void delay_ms(__IO uint32_t nTime)
// { 
//   TimingDelay = nTime;

//   while(TimingDelay != 0);
// }



/**
 * @brief 微秒延时，大致范围：0 ~ 233015 us
 * 
 * @param us 
 */
void delay_us(uint32_t us)
{
  SysTick->LOAD = 72 * us;  //  LOAD寄存器只有24位
  SysTick->VAL = 0x00;      //  计数寄存器同样只有24位，清空当前值
  SysTick->CTRL = 0x00000005;   //  设置AHB时钟，不分频
  while(!(SysTick->CTRL & 0x00010000));   //  等待计数为0
  SysTick->CTRL = 0x00000004;   //  关闭计数器
}

void delay_ms(uint32_t ms)
{
  while (ms--)
  {
    delay_us(1000);
  }
}