#include "w25q16.h"

static uint16_t SectorBuffer[SectorSize];

/* SPI Drive Config -------------------------------------------------------------*/

void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PA4,5,7 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = SPI_CS | SPI_SCK | SPI_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure);
  
  /* Configure PA6 in input mode */
  GPIO_InitStructure.GPIO_Pin = SPI_MISO;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure);  

  SPI_CS_Write(1);
  SPI_SCK_Write(0);
}


/**
 * @brief 向模拟SPI总线传输一个8位字节数据（模式0）
 *        实际上就是在SCK上升沿的时候，读取MISO的数据，在SCK下降沿的时候移出MOSI的数据。
 *        因此我们需要在SCK上升沿之前把需要发送的数据位放置在MOSI线上。
 * 		    由于是模拟SPI程序不能同时进行，所以需要在上升沿之前，硬件SPI则不用考虑
 *        模式2，3 修改SPI SCK极性相反即可，程序不变
 * @param byte 
 * @return uint8_t 
 */
static uint8_t SPI_ReadWrite(uint8_t byte)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    SPI_MOSI_Write(byte & 0x80);  //上升沿之前先移出数据到MOSI上,byte高位移出
    byte <<= 1;
    SPI_SCK_Write(1); //上升沿后读取数据，低位读取
    if(SPI_MISO_Read)   byte |= 0x01;
    SPI_SCK_Write(0); //下降沿后写数据
  }
  return byte;
}

#if 0

/**
 * @brief 另一种SPI传输字节程序
 * 
 * @param byteSend 
 * @return uint8_t 
 */
uint8_t SPI_ReadWrite(uint8_t byteSend)
{
  uint8_t byteReceive = 0x00;
  for (uint8_t i = 0; i < 8; i++)
  {
    SPI_MOSI_Write(byteSend & (0x80 >> i));
    SPI_SCK_Write(1);
    if(SPI_MISO_Read)   byteReceive |= (0x80 >> i);
    SPI_SCK_Write(0);
  }
  return byteReceive;
}


/**
 * @brief 向模拟SPI总线传输一个8位字节数据（模式1）
 *        实际上就是在SCK上升沿的时候，移出MOSI的数据，在SCK下降沿的时候读取MISO的数据。
 *        因此我们需要在SCK下降沿之前把需要发送的数据位放置在MOSI线上。
 * 		    由于是模拟SPI程序不能同时进行，所以需要在下降沿之前，硬件SPI则不用考虑
 * @param byte 
 * @return uint8_t 
 */
uint8_t SPI_ReadWrite(uint8_t byte)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    SPI_SCK_Write(1); //上升沿后写数据
    SPI_MOSI_Write(byte & 0x80);  
    byte <<= 1;
    SPI_SCK_Write(0); 
    if(SPI_MISO_Read)   byte |= 0x01; //下降沿后读取数据
    
  }
  return byte;
}

#endif

/* W25Q16 Drive Config -------------------------------------------------------------*/

void W25Q16_Init(void)
{
  SPI_GPIO_Init();
}

/**
 * @brief 读取ID
 * 
 * @param ManufacturerID 厂商ID
 * @param DevieceID 设备ID
 */
void W25Q16_ReadID(uint8_t *ManufacturerID, uint16_t *DevieceID)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_JEDEC_ID);
  *ManufacturerID = SPI_ReadWrite(W25Q16_DUMMY_BYTE);
  *DevieceID = SPI_ReadWrite(W25Q16_DUMMY_BYTE);
  *DevieceID <<= 8;
  *DevieceID |= SPI_ReadWrite(W25Q16_DUMMY_BYTE);
  SPI_Stop;
}

/**
 * @brief 写使能
 * 
 */
void W25Q16_WriteEnable(void)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_WRITE_ENABLE);
  SPI_Stop;
}

/**
 * @brief 等待忙状态
 * 
 */
void W25Q16_WaitBusy(void)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_READ_STATUS_REGISTER_1);
  while(SPI_ReadWrite(W25Q16_DUMMY_BYTE) & 0x01);
  SPI_Stop;
}

/**
 * @brief W25Q16写数据，页编程
 * 
 * @param addr 24位地址, 写数据地址范围(0x000000 ~ 0x0000FF)
 * @param data 要写入的数据
 * @param count 要写入的数据数量
 */
void W25Q16_WriteOnePage(uint32_t addr, uint8_t *data, uint16_t count)
{
  /*  写入和擦除数据之前需要先写使能  */
  W25Q16_WriteEnable();

  SPI_Start;
  SPI_ReadWrite(W25Q16_PAGE_PROGRAM);
  /*  发送24位地址  */
  SPI_ReadWrite(addr >> 16);  
  SPI_ReadWrite(addr >> 8);   
  SPI_ReadWrite(addr);        
  /*  最大写256个字节，超过会覆盖之前写的  */
  if(count > PageSize)   count = PageSize;

  for (uint16_t i = 0; i < count; i++)
  {
    SPI_ReadWrite(data[i]);
  }
  SPI_Stop;
  /*  写完数据后等待，方便读数据之前写等待  */
  W25Q16_WaitBusy();
}

/**
 * @brief 扇区擦除
 * 
 * @param addr 24位地址，擦除数据地址范围(0x~~~FFF)
 */
void W25Q16_SectorErase(uint32_t addr)
{
  /*  写入和擦除数据之前需要先写使能  */
  W25Q16_WriteEnable();

  SPI_Start;
  SPI_ReadWrite(W25Q16_SECTOR_ERASE_4KB);
  /*  发送24位地址  */
  SPI_ReadWrite(addr >> 16);  
  SPI_ReadWrite(addr >> 8);   
  SPI_ReadWrite(addr);   
  SPI_Stop;  
  /*  擦除数据后等待 */
  W25Q16_WaitBusy();
}

/**
 * @brief W25Q16读数据
 * 
 * @param addr 24位地址，读数据地址，读数据没有限制
 * @param data 要读取的数据
 * @param count 要读取的数据数量
 */
void W25Q16_ReadData(uint32_t addr, uint8_t *data, uint32_t count)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_READ_DATA);
  /*  发送24位地址  */
  SPI_ReadWrite(addr >> 16);  
  SPI_ReadWrite(addr >> 8);   
  SPI_ReadWrite(addr);        

  for (uint16_t i = 0; i < count; i++)
  {
    data[i] = SPI_ReadWrite(W25Q16_DUMMY_BYTE);
  }
  SPI_Stop;
}

/**
 * @brief W25Q16写数据, 可以跨页写多页数据,不能跨扇区
 * 
 * @param addr 写数据地址，地址范围(0x000000 ~ 0x~~~FFF)
 * @param data 数据内容
 * @param count 数据个数
 */
void W25Q16_WriteMultiPage(uint32_t addr, uint8_t *data, uint16_t count)
{
  uint16_t PageRemain = PageSize - addr % PageSize;   //计算单页剩余字节数
  /*  当前页任意地址写的数据个数不超过页剩余地址数  */
  if (count <= PageRemain)  PageRemain = count;
  while (1)
  {
    /*  写字节个数比剩余地址要少时，直接写完  */
    W25Q16_WriteOnePage(addr, data, PageRemain);

    if(PageRemain == count)   break;  //全部数据写完，退出循环
    else
    {
      data += PageRemain;   //数据偏移到上次写完剩余字节后
      addr += PageRemain;   //地址偏移到上次写完剩余字节地址后
      count -= PageRemain;  //减去上次写入的字节数
      if (count > PageSize)  PageRemain = PageSize;   //还能写下一页
      else  PageRemain = count;   //可以写完
    }
  }
}

/**
 * @brief W25Q16写数据, 写数据任意长度(不超过总容量,可以跨页跨扇区), 写入前不需要擦除
 * 
 * @param addr 写数据地址，地址范围(0x000000 ~ 0x1FFFFF)
 * @param data 数据内容
 * @param count 数据个数
 */
void W25Q16_WriteData(uint32_t addr, uint8_t *data, uint16_t count)
{
  uint32_t SectorPos;     //扇区位置
  uint16_t SectorOff;     //扇区偏移位置
  uint16_t SectorRemain;  //扇区剩余位置
  uint16_t i;
  uint8_t *SectorBuf;     //扇区缓存区
  /*  指向外部定义的扇区数组，这样做的好处是可以不用改大堆栈  */
  SectorBuf = SectorBuffer;

  SectorPos = addr / SectorSize;          //计算扇区位置
  SectorOff = addr % SectorSize;          //计算扇区偏移位置
  SectorRemain = SectorSize - SectorOff;  //计算扇区剩余位置
  /*  当前扇区任意地址写的数据个数不超过扇区剩余地址数  */
  if(count <= SectorRemain)   SectorRemain = count;

  while (1)
  {
    /*  读取当前整个扇区内容  */
    W25Q16_ReadData(SectorPos*SectorSize, SectorBuf, SectorSize);
    /*  判断扇区剩余位置是否擦除  */
    for (i = 0; i < SectorRemain; i++)
    {
      if(SectorBuf[SectorOff + i] != 0xFF)  //需要擦除,退出循环
        break;
    }
    if (i < SectorRemain) //表示需要擦除
    {
      W25Q16_SectorErase(SectorPos*SectorSize);
      /*  擦除后数据放入缓冲区  */
      for (i = 0; i < SectorRemain; i++)
      {
        SectorBuf[SectorOff + i] = data[i];
      }
      /*  将缓冲区数据写入  */
      W25Q16_WriteMultiPage(SectorPos*SectorSize, SectorBuf, SectorSize);
    }
    else  W25Q16_WriteMultiPage(addr, data, SectorRemain);  //不擦除,直接写
    /*  所有数据写完退出循环  */
    if(count == SectorRemain)   break;
    else
    {
      SectorPos++;              //换下一个扇区写入
      SectorOff = 0;            //偏移地址为0
      data += SectorRemain;     //数据偏移到上次写完剩余字节后
      addr += SectorRemain;     //地址偏移到上次写完剩余字节地址后
      count -= SectorRemain;    //减去上次写入的字节数
      if(count > SectorSize)  SectorRemain = SectorSize;  //还能写下一个扇区
      else    SectorRemain = count;   //可以写完
    }
  }
}