#include "encoder.h"

void Motor_TIM2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOA, GPIO_Pin_4);
  GPIO_SetBits(GPIOA, GPIO_Pin_5);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  /* TIM IT enable */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_OCInitTypeDef  TIM_OCInitStructure;
  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 600;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);

  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM2, ENABLE);   
}

void Encoder_TIM3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PD2 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICInitTypeDef  TIM_ICInitStructure;
  TIM_ICStructInit(&TIM_ICInitStructure); 
  TIM_ICInitStructure.TIM_ICFilter = 0x0A;
  TIM_ICInit(TIM3, &TIM_ICInitStructure); 

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  TIM_SetCounter(TIM3, 0);
  TIM_Cmd(TIM3, ENABLE);
}


int Motor_ReadSpeed(void)
{
  int val = 0;
  val = (short)TIM_GetCounter(TIM3);
  TIM_SetCounter(TIM3, 0);
  return val;
}

