/**
  ******************************************************************************
  * @file    main.c 
  * @brief   Project Template
  * @warning  该工程中的core_cm3.h文件与其他工程中的core_cm3.h文件不同，移植DSP
  *           必须使用该工程的core_cm3.h文件编译才不会报错，此外在宏定义中添加
  *           ARM_MATH_CM3
  * @attention  移植DSP时可以直接在MDK中添加DSP库，该工程目录中的DSP文件内容可以
  *             不用添加到工程中，放在这里方便查看
  *    
  ******************************************************************************
  */
#include "main.h"
#include "LED/led.h"
#include "KEY/key.h"
#include "OLED/OLED.h"
#include "math.h"
#include "arm_math.h"

float data;

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();
  OLED_ShowString(10, 0, "DSP demo", OLED_8X16);
  OLED_Update();

  while(1)
  {
    data = arm_sin_f32(3.1415926/6);  //sin（30°），理论值为0.5
    OLED_ShowString(0, 30, "Result: ", OLED_8X16);
    OLED_ShowFloatNum(65, 30, data, 1, 3, OLED_8X16);
    OLED_Update();
    delay_ms(100);
  }  
}
