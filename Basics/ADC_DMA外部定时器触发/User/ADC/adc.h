#ifndef __ADC_H
#define __ADC_H

#include "main.h"

extern uint16_t adcBuffer[3];

void ADC_DMA_Init(void);
uint16_t ADC_GetValue(void);

#endif
