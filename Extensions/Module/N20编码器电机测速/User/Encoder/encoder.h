#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

void Motor_TIM2_Init(void);
void Encoder_TIM3_Init(void);
int Motor_ReadSpeed(void);

#endif
