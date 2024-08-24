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

uint8_t text[20];
__IO uint16_t ADCConvertedValue;
__IO uint8_t ADC_Flag;

void ADC1_2_IRQHandler(void)
{
  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    ADC_Flag = 1;
  }
}

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  ADC_Configuration();
  OLED_Init();
  OLED_ShowString(20, 0, "ADC demo", OLED_8X16);
  while(1)
  {
    if(ADC_Flag)
    {
      ADC_Flag = 0;
      ADCConvertedValue = ADC_GetValue();
      sprintf(text, "adcValue: %d", ADCConvertedValue);
      OLED_ShowString(0, 20, text, OLED_8X16);
      memset(text, 0, 20);
      sprintf(text, "Volt: %.2f", ADCConvertedValue*3.3f/4096);
      OLED_ShowString(0, 40, text, OLED_8X16);        
    }
    delay_ms(100);
    OLED_Update();
  }  
}
