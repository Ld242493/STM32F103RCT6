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

__IO uint16_t ADCConvertedValue;
CurvePara oledPara = {60, 20, 10, 110, 0};

/**
 * @brief OLED绘制折线，曲线图
 * 
 * @param para 
 * @param val 最大为60-20对应波峰，最小为0对应波谷
 */
void OLED_DrawCurve(CurvePara para, uint8_t val)
{
  static uint8_t firstpoint = 1;
  static uint8_t lastX, lastY;  //上个点的坐标
  uint8_t x = 0, y = 0; //当前点的坐标

  if(val >= para.yoffset - para.ymax || val < 0)  
  {
    val =  para.yoffset - para.ymax;      
  }

  /*  显示数据对应的y值  */
  y = para.yoffset - val;

  /*  描第一个点，无需画线  */
  if(firstpoint)
  {
    OLED_DrawPoint(para.xStart, y);
    lastX = para.xStart;
    lastY = y;
    firstpoint = 0;
  }
  else
  {
    x = lastX + 1;
    if(x < para.xEnd)
    {
      OLED_DrawLine(lastX, lastY, x, y);
      /*  清除下一列数据，防止绘制后面波形干扰*/
      OLED_ClearArea(x + 1, para.ymax, 1, (para.yoffset - para.ymax + 1));
      lastX = x;
      lastY = y;
    }
    else
    {
      /*  重新画第一列，清空第一列与第二列  */
      OLED_ClearArea(para.xStart, para.ymax, 2, (para.yoffset - para.ymax + 1));
      OLED_DrawPoint(para.xStart, y);
      lastX = para.xStart;
      lastY = y;
    }
  }
}


int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  USART_Configuration();
  ADC_Configuration();
  DAC_Configuration();
  OLED_Init();

  OLED_ShowString(10, 0, "DAC Triangle", OLED_8X16);
  OLED_Update();

  while(1)
  {
    ADCConvertedValue = ADC_GetValue();
    OLED_DrawCurve(oledPara, ADCConvertedValue*40/4095);
    OLED_Update();
    // printf("%d\r\n", ADCConvertedValue);
    // delay_ms(5);    
  }  
}
