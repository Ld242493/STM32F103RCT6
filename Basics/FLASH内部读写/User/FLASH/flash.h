#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"


uint32_t FLASH_ReadByte(uint32_t addr);
uint32_t FLASH_ReadHalfWord(uint32_t addr);
uint32_t FLASH_ReadWord(uint32_t addr);

void FLASH_EraseFullPages(void);
void FLASH_EraseOnePage(uint32_t addr);
void FLASH_WriteWord(uint32_t addr, uint32_t word);
void FLASH_WriteHalfWord(uint32_t addr, uint16_t halfword);
void FLASH_Init(uint32_t page, uint16_t *writebuf, uint16_t buf_size);
void FLASH_WriteData(uint32_t addr, uint16_t *data, uint16_t datalen);
void FLASH_ReadData(uint32_t addr, uint16_t *data, uint16_t datalen);
#endif
