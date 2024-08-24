#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define Key2_Pin           GPIO_Pin_13
#define Key2_GPIO_Port     GPIOC
#define WakeUp_Pin         GPIO_Pin_0
#define WakeUp_GPIO_Port   GPIOA
#define Key1_Pin           GPIO_Pin_1
#define Key1_GPIO_Port     GPIOA

typedef enum {
  Key_None  = 0,
  Key_Enter = 1,
  Key_Up,
  Key_Down,
}key_type;

extern key_type Key_Val;

void key_init(void);
key_type key_GetVal(void);
void Timer6_Init(void);
#endif
