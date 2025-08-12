#include "key.h"
#include "tim.h"

key_type Key_Val;

void key_init(void)
{
   HAL_TIM_Base_Start_IT(&htim6);
}  

void key_scan(void)
{
  static uint16_t WakeUp_cnt,Key1_cnt,Key2_cnt;
  if(HAL_GPIO_ReadPin(WakeUp_GPIO_Port, WakeUp_Pin))
  {
    if(++WakeUp_cnt == 1)
      Key_Val = Key_Enter;
  }
  else WakeUp_cnt = 0;
  if(!(HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin)))
  {
    if(++Key1_cnt == 1)
      Key_Val = Key_Up;
  }
  else Key1_cnt = 0;
  if(!(HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin)))
  {
    if(++Key2_cnt == 1)
      Key_Val = Key_Down;
  }
  else Key2_cnt = 0;
}

