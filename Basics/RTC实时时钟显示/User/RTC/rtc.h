#ifndef __RTC_H
#define __RTC_H

#include "main.h"

extern __IO uint8_t TimeDisplay;

void RTC_Configuration(void);
void Time_Display(uint32_t TimeVar, uint8_t *text);
void Time_Adjust(uint8_t hour, uint8_t min, uint8_t sec);
#endif
