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
__IO float volt, temp, verf;


int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  ADC_DMA_Init();
  OLED_Init();
  OLED_ShowString(20, 0, "ADC demo", OLED_8X16);
  while(1)
  {
    // ADC_GetValue();  //adc单次转换配合dma非循环模式需要不断启动转换

    memset(text, 0, 20);
    volt = adcBuffer[0]*3.3f/4096;
    sprintf(text, "Volt: %.2f", volt);
    OLED_ShowString(0, 25, text, OLED_6X8);

    memset(text, 0, 20);
    temp = (1.43f - adcBuffer[1] * 3.3f/4096) * 1000 / 4.3f + 25;
    sprintf(text, "Temp: %.2f", temp);
    OLED_ShowString(0, 40, text, OLED_6X8);    
    
    memset(text, 0, 20);
    verf = adcBuffer[2]*3.3f/4096;
    sprintf(text, "Verf: %.2f", verf);
    OLED_ShowString(0, 55, text, OLED_6X8);         

    delay_ms(100);        
    OLED_Update();
  }  
}
