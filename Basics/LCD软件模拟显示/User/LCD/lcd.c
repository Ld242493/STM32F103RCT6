#include "lcd.h"
#include "lcd_font.h"

/*  LCD全局配置变量，必须按规则修改，不规则修改会使显示出问题  */
lcd_dev lcddev = {
  .fontColor  =   RED,                  //  字体颜色
  .bgColor    =   BLACK,               //  背景颜色
  .fontSize   =   FontSize24,           //  字体大小
  .height     =   LCD_HEIGHT,           //  屏幕高度
  .width      =   LCD_WIDTH,            //  屏幕宽度
  .dir        =   USE_HORIZONTAL,       //  显示方向
};

/* SPI Drive Config -------------------------------------------------------------*/

static void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure SCK and MOSI pins as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pin = SPI_SCK | SPI_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure);
  
  /* Configure PA4 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = SPI_CS;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure);

  SPI_CS_Write(1);
  SPI_SCK_Write(0);
}

/**
 * @brief 向模拟SPI总线传输一个8位字节数据
 *        实际上就是在SCK上升沿的时候，移出MOSI的数据，在SCK下降沿的时候读取MISO的数据。
 *        因此我们需要在SCK上升沿之前把需要发送的数据位放置在MOSI线上。
 * 
 * @param byte 
 * @return uint8_t 
 */
static uint8_t SPI_ReadWrite(uint8_t byte)
{
  for (uint8_t i = 8; i > 0; i--)
  {
    if(byte & 0x80)   SPI_MOSI_Write(1);
    else    SPI_MOSI_Write(0);
    SPI_SCK_Write(0);
    SPI_SCK_Write(1);
    byte <<= 1;     //  时钟上升沿数据传输
  }
}

/* LCD Drive Config -------------------------------------------------------------*/

static void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);

  /* Configure SCK and MOSI pins as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pin = LCD_DC | LCD_RST;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LCD_PORT, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = LCD_BLK;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure); 
}

/**
 * @brief 写命令，将CS片选拉低，然后操作D/C
 *        数据手册中，D/C高电平表示数据，低电平表示命令
 * 
 * @param cmd 
 */
static void LCD_WriteCmd(uint8_t cmd)
{
  SPI_CS_Write(0);
  LCD_DC_Write(0);
  SPI_ReadWrite(cmd);
  SPI_CS_Write(1);
}

/**
 * @brief 写数据，将CS片选拉低，然后操作D/C
 *        数据手册中，D/C高电平表示数据，低电平表示命令
 * 
 * @param data 
 */
static void LCD_WriteData(uint8_t data)
{
  SPI_CS_Write(0);
  LCD_DC_Write(1);
  SPI_ReadWrite(data);
  SPI_CS_Write(1);  
}

/**
 * @brief 写两个字节，16位数据，将CS片选拉低，然后操作D/C
 *        数据手册中，D/C高电平表示数据，低电平表示命令
 * 
 * @param data 
 */
static void LCD_Write16BitData(uint16_t data)
{
  SPI_CS_Write(0);
  LCD_DC_Write(1);
  SPI_ReadWrite(data >> 8);
  SPI_ReadWrite(data);
  SPI_CS_Write(1);    
}

/**
 * @brief 设置显示地址范围
 * 
 * @param xStar 
 * @param yStar 
 * @param xEnd 
 * @param yEnd 
 */
void LCD_SetAddr(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
  LCD_WriteCmd(0x2A);   //  Set x position command
  LCD_WriteData(0x00);
  LCD_WriteData(xStar);
  LCD_WriteData(0x00);
  LCD_WriteData(xEnd);

  LCD_WriteCmd(0x2B);   //  Set y position command
  LCD_WriteData(0x00);
  LCD_WriteData(yStar);
  LCD_WriteData(0x00);
  LCD_WriteData(yEnd);  

  LCD_WriteCmd(0x2C);   //  Set write ram command 
}

/**
 * @brief 设置LCD显示方向, 具体参考定义 USE_HORIZONTAL
 * 
 * @param direction 
 */
void LCD_SetDir(uint8_t direction)
{
  switch(direction)
  {
    case 0:						 	 		
      lcddev.width  = LCD_WIDTH;
      lcddev.height = LCD_HEIGHT;		
      LCD_WriteCmd(0x36); //BGR==1,MY==0,MX==0,MV==0
      LCD_WriteData((0 << 3) | (1 << 6) | (1 << 7));  
    break;

    case 1:
      lcddev.width  = LCD_HEIGHT;
      lcddev.height = LCD_WIDTH;
      LCD_WriteCmd(0x36); //BGR==1,MY==1,MX==0,MV==1
      LCD_WriteData((0 << 3) | (1 << 7) | (1 << 5));
    break;

    case 2:						 	 		
      lcddev.width  = LCD_WIDTH;
      lcddev.height = LCD_HEIGHT;	
      LCD_WriteCmd(0x36); //BGR==1,MY==0,MX==0,MV==0
      LCD_WriteData((0 << 3) | (0 << 6) | (0 << 7));  
    break;

    case 3:
      lcddev.width  = LCD_HEIGHT;
      lcddev.height = LCD_WIDTH;
      LCD_WriteCmd(0x36); //BGR==1,MY==1,MX==0,MV==1
      LCD_WriteData((0 << 3) | (1 << 6) | (0 << 7) | (1 << 5)); 
    break;	

    default:break;
  }		  
}

void LCD_Clear(uint16_t Color)
{
  LCD_SetAddr(0, 0, lcddev.width, lcddev.height);

  SPI_CS_Write(0);
  LCD_DC_Write(1);

  for (int i = 0; i < lcddev.height; i++)
  {
    for (int j = 0; j < lcddev.width; j++)
    {
      LCD_Write16BitData(Color);
    }
  }

  SPI_CS_Write(1);
}

void LCD_RESET(void)
{
  LCD_RST_Write(0);
  delay_ms(100);
  LCD_RST_Write(1);
  delay_ms(50);
}

void LCD_Init(void)
{
  SPI_GPIO_Init();
  LCD_GPIO_Init();
  LCD_RESET();

  LCD_WriteCmd(0x11);
  delay_ms(120);

  LCD_WriteCmd(0xB1);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);

  LCD_WriteCmd(0xB2);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);

  LCD_WriteCmd(0xB3);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);
  LCD_WriteData(0x01);
  LCD_WriteData(0x2C);
  LCD_WriteData(0x2D);
  
  LCD_WriteCmd(0xB4);
  LCD_WriteData(0x07);

  LCD_WriteCmd(0xC0);
  LCD_WriteData(0xA2);
  LCD_WriteData(0x02);
  LCD_WriteData(0x84);

  LCD_WriteCmd(0xC1);
  LCD_WriteData(0xC5);

  LCD_WriteCmd(0xC2);
  LCD_WriteData(0x0A);
  LCD_WriteData(0x00);

  LCD_WriteCmd(0xC3);
  LCD_WriteData(0x8A);
  LCD_WriteData(0x2A);

  LCD_WriteCmd(0xC4);
  LCD_WriteData(0x8A);
  LCD_WriteData(0xEE);

  LCD_WriteCmd(0xC5);
  LCD_WriteData(0x0E);
  LCD_WriteCmd(0x36);
  LCD_WriteData(0xC0);

  //ST7735R Gamma Sequence
  LCD_WriteCmd(0xe0); 
  LCD_WriteData(0x0f); 
  LCD_WriteData(0x1a); 
  LCD_WriteData(0x0f); 
  LCD_WriteData(0x18); 
  LCD_WriteData(0x2f); 
  LCD_WriteData(0x28); 
  LCD_WriteData(0x20); 
  LCD_WriteData(0x22); 
  LCD_WriteData(0x1f); 
  LCD_WriteData(0x1b); 
  LCD_WriteData(0x23); 
  LCD_WriteData(0x37); 
  LCD_WriteData(0x00); 	
  LCD_WriteData(0x07); 
  LCD_WriteData(0x02); 
  LCD_WriteData(0x10); 
  LCD_WriteCmd(0xe1); 
  LCD_WriteData(0x0f); 
  LCD_WriteData(0x1b); 
  LCD_WriteData(0x0f); 
  LCD_WriteData(0x17); 
  LCD_WriteData(0x33); 
  LCD_WriteData(0x2c); 
  LCD_WriteData(0x29); 
  LCD_WriteData(0x2e); 
  LCD_WriteData(0x30); 
  LCD_WriteData(0x30); 
  LCD_WriteData(0x39); 
  LCD_WriteData(0x3f); 
  LCD_WriteData(0x00); 
  LCD_WriteData(0x07); 
  LCD_WriteData(0x03); 
  LCD_WriteData(0x10);  	
  LCD_WriteCmd(0x2a);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x7f);
  LCD_WriteCmd(0x2b);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x00);
  LCD_WriteData(0x9f);
  LCD_WriteCmd(0xF0); //Enable test command  
  LCD_WriteData(0x01); 
  LCD_WriteCmd(0xF6); //Disable ram power save mode 
  LCD_WriteData(0x00); 	
  LCD_WriteCmd(0x3A); //65k mode 
  LCD_WriteData(0x05); 
  LCD_WriteCmd(0x29); //Display on

  LCD_SetDir(lcddev.dir);
  LCD_BLK_Write(1);
  LCD_Clear(WHITE);
}

/* LCD Show Config -------------------------------------------------------------*/

/**
 * @brief 显示单个字符(6x12, 8x16)
 * 
 * @param x 
 * @param y 
 * @param Char 
 */
static void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t Char)
{
  uint8_t temp, cnt = 0;   //   用来计数需要显示的位数   
  uint8_t len;             //   这个变量对应字符字模数据长度
  /*  边界检查  */
  if (x + lcddev.fontSize / 2 > lcddev.width || y + lcddev.fontSize > lcddev.height) return;
  
  Char = Char - ' ';  // 字符偏移，对应数组索引
  LCD_SetAddr(x, y, x + lcddev.fontSize / 2 - 1, y + lcddev.fontSize - 1);
  /*  对不同长度的字模数据需要不同处理  */
  if(lcddev.fontSize != FontSize24) len = lcddev.fontSize;
  else  len = 2 * lcddev.fontSize;

  for (uint8_t index = 0; index < len; index++)  // 遍历各行
  {
    if(lcddev.fontSize == FontSize24)  temp = ACSLL_12X24[Char][index];
    else  if(lcddev.fontSize == FontSize16)  temp = ACSLL_8X16[Char][index];
    else  temp = ACSLL_6X12[Char][index];

    for (uint8_t bit = 0; bit < 8; bit++)  // 遍历各列
    {
      /*  判断每一位是否需要打点  */
      if(temp & (0x01 << bit))  LCD_Write16BitData(lcddev.fontColor);   //  低位在前
      else  LCD_Write16BitData(lcddev.bgColor);
      /*  对于 6x12，只需要低6位数据，高2位不用，循环6次后退出  */
      /*  对于 8x16，8位都需要，刚好循环8次后退出  */
      /*  对于 12x24，前8位都需要，后一个字节低4位需要，然后退出  */
      if (++cnt % (lcddev.fontSize / 2) == 0)
      {
        cnt = 0;
        break;
      }
      
    }
  }  
}

/**
 * @brief 显示字符串(6x12, 8x16)
 * 
 * @param x 
 * @param y 
 * @param String 
 */
void LCD_ShowString(uint16_t x, uint16_t y, uint8_t *String)
{
  while ((*String <= '~') && (*String >= ' '))  //判断字母是否非法
  {
    LCD_ShowChar(x, y, *String);
    x += lcddev.fontSize / 2;
    String++;
  }
}

/**
 * @brief 次方函数
 * 
 * @param x 底数
 * @param y 指数
 * @return uint32_t 
 */
static uint32_t LCD_Pow(uint8_t x, uint8_t y)
{
  uint32_t Result = 1;
  while(y--)  Result *= x;
  return  Result;
}

/**
 * @brief 显示数字
 * 
 * @param x 
 * @param y 
 * @param num 数字
 * @param len 数字长度
 */
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len)
{
  for (uint8_t i = 0; i < len; i++)
  {
    /*Number / LCD_Pow(10, Length - i - 1) % 10 可以十进制提取数字的每一位*/
		/*+ '0' 可将数字转换为字符格式*/
    LCD_ShowChar(x + i * lcddev.fontSize / 2, y, num / LCD_Pow(10, len - i -1) % 10 + '0');
  }
}


/**
 * @brief 显示单个 12x12 中文字符
 * 
 * @param x 
 * @param y 
 * @param text 
 */
static void LCD_ShowChinese12(uint16_t x, uint16_t y, uint8_t *text)
{
  uint8_t cnt = 0;
  /*  计算汉字个数  */
  uint16_t len = sizeof(fontgb12) / sizeof(typeFontGB12);
  /*  边界检查  */
  if((x > lcddev.width - 12) || (y > lcddev.height - 12)) return;

  for (uint16_t i = 0; i < len; i++)
  {
    /*  判断需要显示的汉字  */
    if ((fontgb12[i].index[0] == text[0]) && (fontgb12[i].index[1] == text[1]))
    {
      LCD_SetAddr(x, y, x + 12 - 1, y + 12 - 1);

      for (uint16_t byte = 0; byte < 24; byte++)  //  遍历每个字节数据
      {
        for (uint16_t bit = 0; bit < 8; bit++)    //  遍历每一位
        {
          if(fontgb12[i].data[byte] & (0x01 << bit))  LCD_Write16BitData(lcddev.fontColor);
          else  LCD_Write16BitData(lcddev.bgColor);
          /*  取第一个字节8位，第二个字节低4位  */
          if (++cnt % 12 == 0)
          {
            cnt = 0;
            break;
          }
        }
      }
    }
  }
}

/**
 * @brief 显示单个 16x16 中文字符
 * 
 * @param x 
 * @param y 
 * @param text 
 */
static void LCD_ShowChinese16(uint16_t x, uint16_t y, uint8_t *text)
{
  /*  计算汉字个数  */
  uint16_t len = sizeof(fontgb16) / sizeof(typeFontGB16);
  /*  边界检查  */
  if((x > lcddev.width - 16) || (y > lcddev.height - 16)) return;

  for (uint16_t cnt = 0; cnt < len; cnt++)
  {
    /*  判断需要显示的汉字  */
    if ((fontgb16[cnt].index[0] == text[0]) && (fontgb16[cnt].index[1] == text[1]))
    {
      LCD_SetAddr(x, y, x + 16 - 1, y + 16 - 1);

      for (uint16_t byte = 0; byte < 32; byte++)  //  遍历每个字节数据
      {
        for (uint16_t bit = 0; bit < 8; bit++)    //  遍历每一位
        {
          if(fontgb16[cnt].data[byte] & (0x01 << bit))  LCD_Write16BitData(lcddev.fontColor);
          else  LCD_Write16BitData(lcddev.bgColor);
        }
      }
    }
  }
}


/**
 * @brief 显示单个 24x24 中文字符
 * 
 * @param x 
 * @param y 
 * @param text 
 */
static void LCD_ShowChinese24(uint16_t x, uint16_t y, uint8_t *text)
{
  /*  计算汉字个数  */
  uint16_t len = sizeof(fontgb24) / sizeof(typeFontGB24);
  /*  边界检查  */
  if((x > lcddev.width - 24) || (y > lcddev.height - 24)) return;

  for (uint16_t cnt = 0; cnt < len; cnt++)
  {
    /*  判断需要显示的汉字  */
    if ((fontgb24[cnt].index[0] == text[0]) && (fontgb24[cnt].index[1] == text[1]))
    {
      LCD_SetAddr(x, y, x + 24 - 1, y + 24 - 1);

      for (uint16_t byte = 0; byte < 72; byte++)  //  遍历每个字节数据
      {
        for (uint16_t bit = 0; bit < 8; bit++)    //  遍历每一位
        {
          if(fontgb24[cnt].data[byte] & (0x01 << bit))  LCD_Write16BitData(lcddev.fontColor);
          else  LCD_Write16BitData(lcddev.bgColor);
        }
      }
    }
  }
}

/**
 * @brief 显示中文
 * 
 * @param x 
 * @param y 
 * @param text 
 */
void LCD_ShowChinese(uint16_t x, uint16_t y, uint8_t *text)
{
  if(*text < 0x80)  return;
  
  while (*text != '\0')
  {
    /*  字体匹配  */
    if(lcddev.fontSize == FontSize24) LCD_ShowChinese24(x, y, text);  
    else if(lcddev.fontSize == FontSize16)  LCD_ShowChinese16(x, y, text);
    else LCD_ShowChinese12(x, y, text);

    text += 3;
    x += lcddev.fontSize; 
  }  
}

/**
 * @brief 显示图片
 * 
 * @param x 
 * @param y 
 * @param width 
 * @param height 
 * @param image 
 */
void LCD_ShowImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *image)
{
  uint8_t byteH, byteL;

  LCD_SetAddr(x, y, x + width - 1, y + height - 1);

  for (uint32_t i = 0; i < width * height; i++)
  {
    byteL = *(image + i * 2);
    byteH = *(image + i * 2 + 1);
    LCD_Write16BitData(byteH << 8 | byteL);
  }
}

/**
 * @brief 画点
 * 
 * @param x 
 * @param y 
 * @param color 
 */
static void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
  LCD_SetAddr(x, y, x, y);
  LCD_Write16BitData(color);
}

/**
 * @brief 画线
 * 
 * @param xStar 
 * @param yStar 
 * @param xEnd 
 * @param yEnd 
 */
void LCD_DrawLine(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
  int16_t deltaX, deltaY, distance;
  int16_t xErr = 0, yErr = 0;
  int16_t incx, incy, uRow = xStar, uCol = yStar;
  /*  计算坐标增量  */
  deltaX = xEnd - xStar;
  deltaY = yEnd - yStar;
  /*  判断终点与起点间的方向和距离  */
  if(deltaX > 0)  incx = 1;   //  终点在起点右边
  else if(deltaX == 0)  incx = 0;   //  垂直线
  else  { incx = -1; deltaX = -deltaX;}   //  终点在左边

  if(deltaY > 0)  incy = 1;   //  终点在起点下方
  else if(deltaY == 0)  incy = 0;   //  水平线
  else  { incy = -1; deltaY = -deltaY;}   //  终点在起点上方 

  if(deltaX > deltaY)   distance = deltaX;    //  选取基本增量坐标轴
  else  distance = deltaY;

  for (uint16_t i = 0; i < distance + 1; i++)
  {
    LCD_DrawPoint(uRow, uCol, lcddev.fontColor);
    xErr += deltaX;
    yErr += deltaY;
    if (xErr > distance)
    {
      xErr -= distance;
      uRow += incx;
    }
    if (yErr > distance)
    {
      yErr -= distance;
      uCol += incy;
    }    
  }
}

/**
 * @brief 画矩形
 * 
 * @param xStar 
 * @param yStar 
 * @param xEnd 
 * @param yEnd 
 */
void LCD_DrawRectangle(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
  LCD_DrawLine(xStar, yStar, xEnd, yStar);
  LCD_DrawLine(xStar, yStar, xStar, yEnd);
  LCD_DrawLine(xStar, yEnd, xEnd, yEnd);
  LCD_DrawLine(xEnd, yStar, xEnd, yEnd);
}
