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
#include "MPU6050/mpu6050.h"

uint8_t text[20];
int16_t ax, ay, az;
int16_t gx, gy, gz;
SystemInit();
int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  MPU6050_Init();
  OLED_Init();
  
  OLED_ShowString(10, 0, "MPU6050 demo", OLED_8X16);
  sprintf(text, "ID: 0X%X", MPU6050_GetID());
  OLED_ShowString(10, 20, text, OLED_8X16);
  OLED_Update();  
  while(1)
  {
    memset(text, 0, 20);
    MPU6050_GetAccel(&ax, &ay, &az);
    sprintf(text, "%d   %d   %d   ", ax, ay, az);
    OLED_ShowString(10, 40, text, OLED_6X8);

    memset(text, 0, 20);
    MPU6050_GetGyro(&gx, &gy, &gz);
    sprintf(text, "%d   %d   %d   ", gx, gy, gz);
    OLED_ShowString(10, 55, text, OLED_6X8);

    OLED_Update();
    delay_ms(100);
  }  
}
