/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  *
  *
  *    
  ******************************************************************************
  */
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "ADC/adc.h"
#include "DAC/dac.h"

uint8_t text[20];
__IO uint16_t ADCConvertedValue;

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  ADC_Configuration();
  DAC_Configuration();
  OLED_Init();

  OLED_ShowString(20, 0, "DAC demo", OLED_8X16);
  
  while(1)
  {
    ADCConvertedValue = ADC_GetValue();
    sprintf(text, "adcValue: %d ", ADCConvertedValue);
    OLED_ShowString(0, 20, text, OLED_8X16);

    memset(text, 0, 20);
    sprintf(text, "Volt: %.2f", ADCConvertedValue*3.3/4096);
    OLED_ShowString(0, 40, text, OLED_8X16);    
    
    OLED_Update();
    delay_ms(100);         
  }  
}
