#include "led.h"
#include "Systick.h"

void led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOD, GPIO_Pin_2);
}

void led_toggle(void)
{
  GPIO_WriteBit(GPIOD, GPIO_Pin_2, !(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)));
}

void led_ctrl(BitAction Bit_Val)
{
  GPIO_WriteBit(GPIOD, GPIO_Pin_2, Bit_Val);
}

void PWM_LED_Init(void)
{
  GPIO_InitTypeDef         GPIO_InitStructure;  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef        TIM_OCInitStructure;  

  /* TIM1, GPIOA and AFIO clocks enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | 
                         RCC_APB2Periph_AFIO, ENABLE);
  
  /* GPIOF Configuration: TIM1 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 1000;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);   
}

void PWM_LED_Ctrl(BitAction Bit_Val)
{
  if(Bit_Val)
    TIM_SetCompare1(TIM1, 1000);
  else 
    TIM_SetCompare1(TIM1, 0);
}

/**
 * @brief pwm 呼吸灯
 * @param None
 * @retval None
 */ 
void PWM_LED_Breathe(void)
{
  for(uint16_t j = 500; j > 0; j--)
  {
    TIM_SetCompare1(TIM1, j*2);
    delay_ms(1);
  }
  for(uint16_t i = 0; i <= 500; i++)
  {
    TIM_SetCompare1(TIM1, i*2);
    delay_ms(1);
  }  
}