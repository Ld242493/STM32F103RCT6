#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "main.h"

#define   HC_SR04_PORT    GPIOB
#define   ECHO_PIN        GPIO_Pin_1
#define   TRIG_PIN        GPIO_Pin_0

extern float distance;


void Ultrasonic_TIM7_Init(void);
void Ultrasonic_Start(void);

#endif
