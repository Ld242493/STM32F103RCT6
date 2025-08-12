#include "Ultrasonic.h"

float distance;

void Ultrasonic_TIM7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* TIM7 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  /* GPIOC clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(HC_SR04_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(HC_SR04_PORT, &GPIO_InitStructure);

  /* Connect EXTI1 Line to PA.01 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

  EXTI_InitTypeDef   EXTI_InitStructure;
  /* Configure EXTI1 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);    

  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable the EXTI1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);  
}


/**
 * @brief 发出10us以上的高电平触发信号，然后模块内部发出8个40khz的脉冲信号
 * 
 */
void Ultrasonic_Start(void)
{
  GPIO_SetBits(HC_SR04_PORT, TRIG_PIN);
  delay_us(20);
  GPIO_ResetBits(HC_SR04_PORT, TRIG_PIN);
}
