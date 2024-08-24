#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define led_on  Bit_RESET
#define led_off Bit_SET


void led_init(void);
void led_toggle(void);
void led_ctrl(BitAction Bit_Val);
void PWM_LED_Init(void);
void PWM_LED_Ctrl(BitAction Bit_Val);
void PWM_LED_Breathe(void);
#endif 
