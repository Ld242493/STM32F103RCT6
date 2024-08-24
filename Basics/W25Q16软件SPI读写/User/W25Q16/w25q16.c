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
 * @brief ��ģ��SPI���ߴ���һ��8λ�ֽ����ݣ�ģʽ0��
 *        ʵ���Ͼ�����SCK�����ص�ʱ�򣬶�ȡMISO�����ݣ���SCK�½��ص�ʱ���Ƴ�MOSI�����ݡ�
 *        ���������Ҫ��SCK������֮ǰ����Ҫ���͵�����λ������MOSI���ϡ�
 * 		    ������ģ��SPI������ͬʱ���У�������Ҫ��������֮ǰ��Ӳ��SPI���ÿ���
 *        ģʽ2��3 �޸�SPI SCK�����෴���ɣ����򲻱�
 * @param byte 
 * @return uint8_t 
 */
static uint8_t SPI_ReadWrite(uint8_t byte)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    SPI_MOSI_Write(byte & 0x80);  //������֮ǰ���Ƴ����ݵ�MOSI��,byte��λ�Ƴ�
    byte <<= 1;
    SPI_SCK_Write(1); //�����غ��ȡ���ݣ���λ��ȡ
    if(SPI_MISO_Read)   byte |= 0x01;
    SPI_SCK_Write(0); //�½��غ�д����
  }
  return byte;
}

#if 0

/**
 * @brief ��һ��SPI�����ֽڳ���
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
 * @brief ��ģ��SPI���ߴ���һ��8λ�ֽ����ݣ�ģʽ1��
 *        ʵ���Ͼ�����SCK�����ص�ʱ���Ƴ�MOSI�����ݣ���SCK�½��ص�ʱ���ȡMISO�����ݡ�
 *        ���������Ҫ��SCK�½���֮ǰ����Ҫ���͵�����λ������MOSI���ϡ�
 * 		    ������ģ��SPI������ͬʱ���У�������Ҫ���½���֮ǰ��Ӳ��SPI���ÿ���
 * @param byte 
 * @return uint8_t 
 */
uint8_t SPI_ReadWrite(uint8_t byte)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    SPI_SCK_Write(1); //�����غ�д����
    SPI_MOSI_Write(byte & 0x80);  
    byte <<= 1;
    SPI_SCK_Write(0); 
    if(SPI_MISO_Read)   byte |= 0x01; //�½��غ��ȡ����
    
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