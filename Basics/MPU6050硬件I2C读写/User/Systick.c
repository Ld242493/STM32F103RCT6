#include "Systick.h"

static __IO uint32_t TimingDelay;

void Systick_Init(void)
{
  while(SysTick_Config(SystemCoreClock / 1000));   
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

// /**
//   * @brief  微秒级延时
//   * @param  xus 延时时长，范围：0~233015
//   * @retval 无
//   */
// void delay_us(uint32_t xus)
// {
// 	SysTick->LOAD = 72 * xus;				//设置定时器重装值
// 	SysTick->VAL = 0x00;					//清空当前计数值
// 	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
// 	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
// 	SysTick->CTRL = 0x00000004;				//关闭定时器
// }

// /**
//   * @brief  毫秒级延时
//   * @param  xms 延时时长，范围：0~4294967295
//   * @retval 无
//   */
// void delay_ms(uint32_t xms)
// {
// 	while(xms--)
// 	{
// 		delay_us(1000);
// 	}
// }