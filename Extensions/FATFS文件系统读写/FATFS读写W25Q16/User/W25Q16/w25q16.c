#include "w25q16.h"

static uint16_t SectorBuffer[SectorSize];

/* SPI Drive Config -------------------------------------------------------------*/

static void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure SCK and MOSI pins as Alternate Function Push Pull */
  GPIO_InitStructure.GPIO_Pin = SPI_SCK | SPI_MOSI;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_PORT, &GPIO_InitStructure);
  
  /* Configure PA4 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = SPI_CS;
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

static void SPI_Configuration(void)
{
  SPI_InitTypeDef   SPI_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  /* SPI1 Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);	
}

/**
 * @brief Ӳ��SPI����һ���ֽ�����
 * 
 * @param byte 
 * @return uint8_t 
 */
static uint8_t SPI_ReadWrite(uint8_t byte)
{
  /*  ��������ǰ�ж�TXE��־λ�Ƿ�Ϊ1���������ݻ��Զ����TXE��־λ  */  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1, byte);
  /*  ��������ǰ�ж�RXNE��־λ�Ƿ�Ϊ1���������ݻ��Զ����RXNE��־λ  */  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SPI1);
}


/* W25Q16 Config -------------------------------------------------------------*/

void W25Q16_Init(void)
{
  SPI_GPIO_Init();
  SPI_Configuration();
}

/**
 * @brief ��ȡID
 * 
 * @param ManufacturerID ����ID
 * @param DevieceID �豸ID
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
 * @brief дʹ��
 * 
 */
void W25Q16_WriteEnable(void)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_WRITE_ENABLE);
  SPI_Stop;
}

/**
 * @brief �ȴ�æ״̬
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
 * @brief W25Q16д���ݣ�ҳ���
 * 
 * @param addr 24λ��ַ, д���ݵ�ַ��Χ(0x000000 ~ 0x0000FF)
 * @param data Ҫд�������
 * @param count Ҫд�����������
 */
void W25Q16_WriteOnePage(uint32_t addr, uint8_t *data, uint16_t count)
{
  /*  д��Ͳ�������֮ǰ��Ҫ��дʹ��  */
  W25Q16_WriteEnable();

  SPI_Start;
  SPI_ReadWrite(W25Q16_PAGE_PROGRAM);
  /*  ����24λ��ַ  */
  SPI_ReadWrite(addr >> 16);  
  SPI_ReadWrite(addr >> 8);   
  SPI_ReadWrite(addr);        
  /*  ���д256���ֽڣ������Ḳ��֮ǰд��  */
  if(count > PageSize)   count = PageSize;

  for (uint16_t i = 0; i < count; i++)
  {
    SPI_ReadWrite(data[i]);
  }
  SPI_Stop;
  /*  д�����ݺ�ȴ������������֮ǰд�ȴ�  */
  W25Q16_WaitBusy();
}

/**
 * @brief ��������
 * 
 * @param addr 24λ��ַ���������ݵ�ַ��Χ(0x~~~FFF)
 */
void W25Q16_SectorErase(uint32_t addr)
{
  /*  д��Ͳ�������֮ǰ��Ҫ��дʹ��  */
  W25Q16_WriteEnable();

  SPI_Start;
  SPI_ReadWrite(W25Q16_SECTOR_ERASE_4KB);
  /*  ����24λ��ַ  */
  SPI_ReadWrite(addr >> 16);  
  SPI_ReadWrite(addr >> 8);   
  SPI_ReadWrite(addr);   
  SPI_Stop;  
  /*  �������ݺ�ȴ� */
  W25Q16_WaitBusy();
}

/**
 * @brief W25Q16������
 * 
 * @param addr 24λ��ַ�������ݵ�ַ��������û������
 * @param data Ҫ��ȡ������
 * @param count Ҫ��ȡ����������
 */
void W25Q16_ReadData(uint32_t addr, uint8_t *data, uint32_t count)
{
  SPI_Start;
  SPI_ReadWrite(W25Q16_READ_DATA);
  /*  ����24λ��ַ  */
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
 * @brief W25Q16д����, ���Կ�ҳд��ҳ����,���ܿ�����
 * 
 * @param addr д���ݵ�ַ����ַ��Χ(0x000000 ~ 0x~~~FFF)
 * @param data ��������
 * @param count ���ݸ���
 */
void W25Q16_WriteMultiPage(uint32_t addr, uint8_t *data, uint16_t count)
{
  uint16_t PageRemain = PageSize - addr % PageSize;   //���㵥ҳʣ���ֽ���
  /*  ��ǰҳ�����ַд�����ݸ���������ҳʣ���ַ��  */
  if (count <= PageRemain)  PageRemain = count;
  while (1)
  {
    /*  д�ֽڸ�����ʣ���ַҪ��ʱ��ֱ��д��  */
    W25Q16_WriteOnePage(addr, data, PageRemain);

    if(PageRemain == count)   break;  //ȫ������д�꣬�˳�ѭ��
    else
    {
      data += PageRemain;   //����ƫ�Ƶ��ϴ�д��ʣ���ֽں�
      addr += PageRemain;   //��ַƫ�Ƶ��ϴ�д��ʣ���ֽڵ�ַ��
      count -= PageRemain;  //��ȥ�ϴ�д����ֽ���
      if (count > PageSize)  PageRemain = PageSize;   //����д��һҳ
      else  PageRemain = count;   //����д��
    }
  }
}

/**
 * @brief W25Q16д����, д�������ⳤ��(������������,���Կ�ҳ������), д��ǰ����Ҫ����
 * 
 * @param addr д���ݵ�ַ����ַ��Χ(0x000000 ~ 0x1FFFFF)
 * @param data ��������
 * @param count ���ݸ���
 */
void W25Q16_WriteData(uint32_t addr, uint8_t *data, uint16_t count)
{
  uint32_t SectorPos;     //����λ��
  uint16_t SectorOff;     //����ƫ��λ��
  uint16_t SectorRemain;  //����ʣ��λ��
  uint16_t i;
  uint8_t *SectorBuf;     //����������
  /*  ָ���ⲿ������������飬�������ĺô��ǿ��Բ��øĴ��ջ  */
  SectorBuf = SectorBuffer;

  SectorPos = addr / SectorSize;          //��������λ��
  SectorOff = addr % SectorSize;          //��������ƫ��λ��
  SectorRemain = SectorSize - SectorOff;  //��������ʣ��λ��
  /*  ��ǰ���������ַд�����ݸ�������������ʣ���ַ��  */
  if(count <= SectorRemain)   SectorRemain = count;

  while (1)
  {
    /*  ��ȡ��ǰ������������  */
    W25Q16_ReadData(SectorPos*SectorSize, SectorBuf, SectorSize);
    /*  �ж�����ʣ��λ���Ƿ����  */
    for (i = 0; i < SectorRemain; i++)
    {
      if(SectorBuf[SectorOff + i] != 0xFF)  //��Ҫ����,�˳�ѭ��
        break;
    }
    if (i < SectorRemain) //��ʾ��Ҫ����
    {
      W25Q16_SectorErase(SectorPos*SectorSize);
      /*  ���������ݷ��뻺����  */
      for (i = 0; i < SectorRemain; i++)
      {
        SectorBuf[SectorOff + i] = data[i];
      }
      /*  ������������д��  */
      W25Q16_WriteMultiPage(SectorPos*SectorSize, SectorBuf, SectorSize);
    }
    else  W25Q16_WriteMultiPage(addr, data, SectorRemain);  //������,ֱ��д
    /*  ��������д���˳�ѭ��  */
    if(count == SectorRemain)   break;
    else
    {
      SectorPos++;              //����һ������д��
      SectorOff = 0;            //ƫ�Ƶ�ַΪ0
      data += SectorRemain;     //����ƫ�Ƶ��ϴ�д��ʣ���ֽں�
      addr += SectorRemain;     //��ַƫ�Ƶ��ϴ�д��ʣ���ֽڵ�ַ��
      count -= SectorRemain;    //��ȥ�ϴ�д����ֽ���
      if(count > SectorSize)  SectorRemain = SectorSize;  //����д��һ������
      else    SectorRemain = count;   //����д��
    }
  }
}