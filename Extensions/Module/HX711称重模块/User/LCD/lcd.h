#ifndef __LCD_H
#define __LCD_H

#include "main.h"


/* SPI GPIO Config -------------------------------------------------------------*/

#define SPI_PORT    GPIOB

#define SPI_SCK     GPIO_Pin_13
#define SPI_CS      GPIO_Pin_14
#define SPI_MOSI    GPIO_Pin_15 


#define SPI_CS_Write(x)     GPIO_WriteBit(SPI_PORT, SPI_CS, (BitAction)x)
#define SPI_SCK_Write(x)    GPIO_WriteBit(SPI_PORT, SPI_SCK, (BitAction)x)
#define SPI_MOSI_Write(x)   GPIO_WriteBit(SPI_PORT, SPI_MOSI, (BitAction)x)

#define SPI_Start   SPI_CS_Write(0)
#define SPI_Stop    SPI_CS_Write(1)

/* LCD Config -------------------------------------------------------------*/

#define LCD_PORT    GPIOC

#define LCD_BLK     GPIO_Pin_12
#define LCD_DC      GPIO_Pin_6
#define LCD_RST     GPIO_Pin_7

#define LCD_BLK_Write(x)     GPIO_WriteBit(SPI_PORT, LCD_BLK, (BitAction)x)
#define LCD_DC_Write(x)      GPIO_WriteBit(LCD_PORT, LCD_DC, (BitAction)x)
#define LCD_RST_Write(x)     GPIO_WriteBit(LCD_PORT, LCD_RST, (BitAction)x)


typedef struct 
{
  uint16_t  width;
  uint16_t  height;
  uint16_t  fontColor;
  uint16_t  bgColor;
  uint16_t  fontSize;
  uint8_t   dir;
}lcd_dev;

extern lcd_dev lcddev;

//定义LCD的尺寸
#define LCD_WIDTH    128
#define LCD_HEIGHT   160

//定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
#define USE_HORIZONTAL    0

//定义液晶屏字体大小
#define FontSize12      12
#define FontSize16      16
#define FontSize24      24

//定义液晶屏字体颜色
#define WHITE       0xFFFF  //  白色
#define BLACK      	0x0000  //  黑色
#define BLUE       	0x001F  //  蓝色
#define RED         0xF800  //  红色
#define MAGENTA     0xF81F  //  紫色
#define GREEN       0x07E0  //  绿色
#define CYAN        0x7FFF  //  青色
#define YELLOW      0xFFE0  //  黄色
#define LIGHTBlUE   0X7D7C  //  浅蓝色
#define ORANGE 			0XFC07  //  橙色
#define GRAY  			0X8430  //  灰色


void LCD_Init(void);
void LCD_Clear(uint16_t Color);
void LCD_ShowString(uint16_t x, uint16_t y, uint8_t *String);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len);
void LCD_ShowChinese(uint16_t x, uint16_t y, uint8_t *text);
void LCD_ShowImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image);
void LCD_DrawLine(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd);
void LCD_DrawRectangle(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd);
#endif
