#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define Key2_Pin           GPIO_Pin_13
#define Key2_GPIO_Port     GPIOC
#define WakeUp_Pin         GPIO_Pin_0
#define WakeUp_GPIO_Port   GPIOA
#define Key1_Pin           GPIO_Pin_1
#define Key1_GPIO_Port     GPIOA

#define ReadKey1            (GPIO_ReadInputDataBit(Key1_GPIO_Port, Key1_Pin))
#define ReadKeyWakeUp       (GPIO_ReadInputDataBit(WakeUp_GPIO_Port, WakeUp_Pin))
#define ReadKey2            (GPIO_ReadInputDataBit(Key2_GPIO_Port, Key2_Pin))

#define KeyTimeVal    30  //按键消抖延时时间

typedef enum {
  Key_None  = 0,
  Key_Enter,
  Key_Up,
  Key_Down,
}key_type;

extern key_type keyVal;

void KEY_TIM6_Init(void);
#endif
