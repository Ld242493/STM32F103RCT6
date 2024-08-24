#include "flash.h"

/**
 * @brief ��ȡ��ַ�ϵ�8λ����
 * 
 * @param addr 32λ��ַ
 * @return uint32_t 
 */
uint32_t FLASH_ReadByte(uint32_t addr)
{
  // uint8_t * �� uint16_t * ��������ָ��ĵ�ַ��С�޹أ�
  // �������ַ�ϲ��������ݷ�Χ�йء�ָ�����Ͷ���������ָ����������ͣ�
  // ����������ָ��ĵ�ַ�Ĵ�С��
  return *((__IO uint8_t *)(addr));
}

uint32_t FLASH_ReadHalfWord(uint32_t addr)
{
  return *((__IO uint16_t *)(addr));
}

uint32_t FLASH_ReadWord(uint32_t addr)
{
  return *((__IO uint32_t *)(addr));
}


/**
 * @brief ����һҳ��һҳ1k���ֽ�
 * 
 * @param addr 
 */
void FLASH_EraseOnePage(uint32_t addr)
{
  FLASH_Unlock();
  FLASH_ErasePage(addr);
  FLASH_Lock();
}

void FLASH_EraseFullPages(void)
{
  FLASH_Unlock();
  FLASH_EraseAllPages();
  FLASH_Lock();
}

void FLASH_WriteWord(uint32_t addr, uint32_t word)
{
  FLASH_Unlock();
  FLASH_ProgramWord(addr, word);
  FLASH_Lock();  
}

void FLASH_WriteHalfWord(uint32_t addr, uint16_t halfword)
{
  FLASH_Unlock();
  FLASH_ProgramHalfWord(addr, halfword);
  FLASH_Lock();  
}

/**
 * @brief ����ͬ���ϴ�д���flash���ݣ���ֹ֮��д���ݰ�֮ǰ���ݲ���
 *        д��ҳ�ĵ�һ���ֽ���������־λ�����޸ĵ�һ���ֽڣ������޷�ͬ������
 * @param page 
 * @param writebuf 
 * @param buf_size 
 */
void FLASH_Init(uint32_t page, uint16_t *writebuf, uint16_t buf_size)
{
  /*  ʹ�ñ�־λ���ж��Ƿ��һ��д������  */
  if (FLASH_ReadHalfWord(page) != 0xABAB)
  {
    FLASH_EraseOnePage(page);
    FLASH_WriteHalfWord(page, 0xABAB);
  }
  /*  ��flash�ڴ����ݶ���д�����У������������´�д��ʱ���������һ��д��flash��   */
  for (uint16_t i = 0; i < buf_size; i++)
  {
    *(writebuf + i) = FLASH_ReadHalfWord(page + i*2);
  }
  
}

void FLASH_WriteData(uint32_t addr, uint16_t *data, uint16_t datalen)
{
  FLASH_Unlock();
  FLASH_ErasePage(addr);
  for (uint16_t i = 0; i < datalen; i++)
  {
    FLASH_ProgramHalfWord(addr + i*2, *(data + i));
  }    
  FLASH_Lock();
}

void FLASH_ReadData(uint32_t addr, uint16_t *data, uint16_t datalen)
{
  for (uint16_t i = 0; i < datalen; i++)
  {
    *(data + i) = FLASH_ReadHalfWord(addr + i*2);
  }  
}
