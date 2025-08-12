#ifndef __ADC_H
#define __ADC_H

#include "main.h"

#define SampleNber  256
extern uint16_t adcBuffer[SampleNber];

void ADC_DMA_Init(void);
#endif
