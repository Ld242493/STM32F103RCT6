#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"


void Systick_Init(void);
void TimingDelay_Decrement(void);
void delay_ms(__IO uint32_t nTime);

//void delay_init(void);
//void delay_ms(unsigned int nms);
//void delay_us(unsigned long nus);

#endif
