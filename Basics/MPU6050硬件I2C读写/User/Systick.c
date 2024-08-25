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
//   * @brief  ΢�뼶��ʱ
//   * @param  xus ��ʱʱ������Χ��0~233015
//   * @retval ��
//   */
// void delay_us(uint32_t xus)
// {
// 	SysTick->LOAD = 72 * xus;				//���ö�ʱ����װֵ
// 	SysTick->VAL = 0x00;					//��յ�ǰ����ֵ
// 	SysTick->CTRL = 0x00000005;				//����ʱ��ԴΪHCLK��������ʱ��
// 	while(!(SysTick->CTRL & 0x00010000));	//�ȴ�������0
// 	SysTick->CTRL = 0x00000004;				//�رն�ʱ��
// }

// /**
//   * @brief  ���뼶��ʱ
//   * @param  xms ��ʱʱ������Χ��0~4294967295
//   * @retval ��
//   */
// void delay_ms(uint32_t xms)
// {
// 	while(xms--)
// 	{
// 		delay_us(1000);
// 	}
// }