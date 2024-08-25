#include "key.h"

key_type Key_Val;

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