#include "flash.h"

/**
 * @brief 读取地址上的8位数据
 * 
 * @param addr 32位地址
 * @return uint32_t 
 */
uint32_t FLASH_ReadByte(uint32_t addr)
{
  // uint8_t * 和 uint16_t * 与它们所指向的地址大小无关，
  // 而是与地址上操作的数据范围有关。指针类型定义了它所指向的数据类型，
  // 而不是它所指向的地址的大小。
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
 * @brief 擦除一页，一页1k个字节
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
 * @brief 用来同步上次写入的flash数据，防止之后写数据把之前数据擦除
 *        写入页的第一个字节用来当标志位，勿修改第一个字节，否则无法同步数据
 * @param page 
 * @param writebuf 
 * @param buf_size 
 */
void FLASH_Init(uint32_t page, uint16_t *writebuf, uint16_t buf_size)
{
  /*  使用标志位来判断是否第一次写入数据  */
  if (FLASH_ReadHalfWord(page) != 0xABAB)
  {
    FLASH_EraseOnePage(page);
    FLASH_WriteHalfWord(page, 0xABAB);
  }
  /*  把flash内存数据读回写缓存中，这样可以在下次写的时候把新数据一起写入flash中   */
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
