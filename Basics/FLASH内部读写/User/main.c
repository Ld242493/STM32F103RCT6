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
#include "FLASH/flash.h"


#define ROM_SIZE    0x1FFFF7E0  // FLASH Size = 256KB = 0x40000
#define LAST_PAGE   0x0803FC00  // 0x08040000 - 0x400
#define BufferSize  512         // 因为是16位数据，所以512刚好为1k

uint16_t WriteBuffer[BufferSize];
uint16_t ReadBuffer[BufferSize];

int main(void)
{
  Systick_Init();
  led_init();
  key_init();
  Timer6_Init();
  OLED_Init();
  FLASH_Init(LAST_PAGE, WriteBuffer, BufferSize);

  OLED_ShowString(10, 0, "Flash demo", OLED_8X16);
  OLED_ShowNum(10, 20, FLASH_ReadHalfWord(ROM_SIZE), 4, OLED_8X16);

  // WriteBuffer[1] = 0xF6F6;
  // WriteBuffer[2] = 0xDFDF;
  // WriteBuffer[3] = 0xC4C4;
  // WriteBuffer[4] = 0xBABA;     
  // FLASH_WriteData(LAST_PAGE, WriteBuffer, BufferSize);
  FLASH_ReadData(LAST_PAGE, ReadBuffer, BufferSize);
  OLED_ShowHexNum(10, 40, ReadBuffer[1], 4, OLED_8X16);
  OLED_ShowHexNum(60, 40, ReadBuffer[2], 4, OLED_8X16);  
  OLED_Update();  

  while(1)
  {
    switch (KeyVal)
    {
      case Key_Enter:
        led_toggle();
        break;  
    }
    
    KeyVal = Key_None;
  
    delay_ms(100);
  }  
}
