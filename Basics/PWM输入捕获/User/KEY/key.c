#include "key.h"

key_type Key_Val = Key_None;

void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = Key2_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(Key2_GPIO_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = WakeUp_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(WakeUp_GPIO_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = Key1_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(Key1_GPIO_Port, &GPIO_InitStructure);     
}

/**
 * @brief  按键扫描
 * @param  void
 * @retval void
 */
void key_scan(void)
{
  static uint16_t WakeUP_cnt,Key1_cnt,Key2_cnt;
  if(GPIO_ReadInputDataBit(WakeUp_GPIO_Port, WakeUp_Pin))
  {
    if(++WakeUP_cnt == 1)
      Key_Val = Key_Enter;
  }
  else WakeUP_cnt = 0;
  if(!(GPIO_ReadInputDataBit(Key1_GPIO_Port, Key1_Pin)))
  {
    if(++Key1_cnt == 1)
      Key_Val = Key_Up;
  }
  else Key1_cnt = 0;
  if(!(GPIO_ReadInputDataBit(Key2_GPIO_Port, Key2_Pin)))
  {
    if(++Key2_cnt == 1)
      Key_Val = Key_Down;
  }
  else Key2_cnt = 0;
}
/**
 * @brief  获取按键值
 * @param  void
 * @retval key_type 按键值类型
 */
key_type key_GetVal(void)
{
  key_type key_temp = Key_None;
  key_temp = Key_Val;
  Key_Val = Key_None;
  return key_temp;
}

void Timer6_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM6, 71, TIM_PSCReloadMode_Immediate);  
  
  /* TIM IT enable */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

  /* TIM2 enable counter */
  TIM_Cmd(TIM6, ENABLE);

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void TIM6_IRQHandler(void)
{
  static uint16_t Cnt1;
  if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
     TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
     if(++Cnt1 == 20)
     {
       Cnt1 = 0;
       key_scan();
     }
   }
}
