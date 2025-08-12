#ifndef __HX711_H
#define __HX711_H


#include "main.h"

#define   HX711_PORT      GPIOB
#define   HX711_DT_PIN    GPIO_Pin_9
#define   HX711_SCK_PIN   GPIO_Pin_8

#define   HX711_DT_IN()         GPIO_ReadInputDataBit(HX711_PORT, HX711_DT_PIN)
#define   HX711_DT_OUT(x)       GPIO_WriteBit(HX711_PORT, HX711_DT_PIN, x)   
#define   HX711_SCK_OUT(x)      GPIO_WriteBit(HX711_PORT, HX711_SCK_PIN, x)

void HX711_Init(void);
void HX711_GetMaoPi(void);
float HX711_GetWeight(void);

#endif
