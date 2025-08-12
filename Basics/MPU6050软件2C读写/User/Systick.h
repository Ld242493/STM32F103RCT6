#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"


void Systick_Init(void);
void TimingDelay_Decrement(void);
void delay_ms(__IO uint32_t nTime);

// void delay_us(uint32_t us);
// void delay_ms(uint32_t ms);

#endif
