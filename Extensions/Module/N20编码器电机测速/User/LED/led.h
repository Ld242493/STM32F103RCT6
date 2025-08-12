#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LEDON     Bit_RESET
#define LEDOFF    Bit_SET


void LED_Init(void);
void LED_Toggle(void);
void LED_Ctrl(BitAction BitVal);
void PWMLED_TIM1_Init(void);
void PWMLED_Ctrl(BitAction BitVal);
void PWMLED_Breathe(void);
#endif 
