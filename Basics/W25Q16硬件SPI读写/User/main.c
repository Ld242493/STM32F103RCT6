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
#include "W25Q16/w25q16.h"


uint8_t WriteBuffer[20] = "test data";
uint8_t ReadBuffer[20];
uint8_t ManufacturerID;
uint16_t DeviceID;

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  W25Q16_Init();
  OLED_Init();

  OLED_ShowString(20, 0, "W25Q16 Demo", OLED_8X16);
  W25Q16_ReadID(&ManufacturerID, &DeviceID);
  sprintf(ReadBuffer, "ID: %X%X", ManufacturerID, DeviceID);
  OLED_ShowString(20, 20, ReadBuffer, OLED_8X16);

  // W25Q16_SectorErase(0xF000);
  W25Q16_WriteData(0xFFFF, WriteBuffer, 20);
  memset(ReadBuffer, 0, sizeof(ReadBuffer));
  W25Q16_ReadData(0xFFFF, ReadBuffer, 20);
  OLED_ShowString(20, 40, ReadBuffer, OLED_8X16);      
  OLED_Update();  

  while(1)
  {    
    delay_ms(100);
  }  
}
