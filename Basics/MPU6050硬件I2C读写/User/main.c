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
#include "MPU6050/inv_mpu_dmp_motion_driver.h"
#include "MPU6050/inv_mpu.h"

uint8_t text[20];
int16_t ax, ay, az;
int16_t gx, gy, gz;
float pitch, roll, yaw, temp;

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  USART_Configuration();
  MPU6050_Init();
  mpu_dmp_init();
  OLED_Init();
  
  OLED_ShowString(10, 0, "MPU6050 demo", OLED_8X16);
  sprintf(text, "ID: 0X%X", MPU6050_GetID());
  OLED_ShowString(10, 20, text, OLED_8X16);
  OLED_Update();

  while(1)
  {
    mpu_dmp_get_data(&pitch, &roll, &yaw);
    printf("X:%.1f°  Y:%.1f°  Z:%.1f°  %.2f°C\r\n",roll, pitch, yaw, MPU605_GetTemp());

    MPU6050_GetAccel(&ax, &ay, &az);
    printf("ax:%d  ay:%d  az:%d\r\n", ax, ay, az);

    MPU6050_GetGyro(&gx, &gy, &gz);
    printf("gx:%d  gy:%d  gz:%d\r\n", gx, gy, gz);

    delay_ms(100);
  }  
}

