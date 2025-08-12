#ifndef __W25Q16_H
#define __W25Q16_H

#include "main.h"

/* SPI GPIO Config -------------------------------------------------------------*/

#define SPI_PORT    GPIOA

#define SPI_CS      GPIO_Pin_3
#define SPI_SCK     GPIO_Pin_5
#define SPI_MISO    GPIO_Pin_6
#define SPI_MOSI    GPIO_Pin_7 

#define SPI_CS_Write(x)     GPIO_WriteBit(SPI_PORT, SPI_CS, (BitAction)x);
#define SPI_SCK_Write(x)    GPIO_WriteBit(SPI_PORT, SPI_SCK, (BitAction)x);
#define SPI_MOSI_Write(x)   GPIO_WriteBit(SPI_PORT, SPI_MOSI, (BitAction)x);
#define SPI_MISO_Read       GPIO_ReadInputDataBit(SPI_PORT, SPI_MISO)  

#define SPI_Start   SPI_CS_Write(0)
#define SPI_Stop    SPI_CS_Write(1)

/* W25Q16 Config -------------------------------------------------------------*/

#define PageMax            16     //一个扇区16页
#define SectorMax          16     //一个块有16个扇区
#define BlockMax           32     //W25Q16有32个块   

#define PageSize           256    //一页256个字节
#define SectorSize         4096   //一个扇区4k个字节


#define W25Q16_WRITE_ENABLE							          0x06
#define W25Q16_WRITE_DISABLE						          0x04
#define W25Q16_READ_STATUS_REGISTER_1				      0x05
#define W25Q16_READ_STATUS_REGISTER_2				      0x35
#define W25Q16_WRITE_STATUS_REGISTER				      0x01
#define W25Q16_PAGE_PROGRAM							          0x02
#define W25Q16_QUAD_PAGE_PROGRAM					        0x32
#define W25Q16_BLOCK_ERASE_64KB						        0xD8
#define W25Q16_BLOCK_ERASE_32KB						        0x52
#define W25Q16_SECTOR_ERASE_4KB						        0x20
#define W25Q16_CHIP_ERASE							            0xC7
#define W25Q16_ERASE_SUSPEND						          0x75
#define W25Q16_ERASE_RESUME							          0x7A
#define W25Q16_POWER_DOWN							            0xB9
#define W25Q16_HIGH_PERFORMANCE_MODE				      0xA3
#define W25Q16_CONTINUOUS_READ_MODE_RESET			    0xFF
#define W25Q16_RELEASE_POWER_DOWN_HPM_DEVICE_ID		0xAB
#define W25Q16_MANUFACTURER_DEVICE_ID				      0x90
#define W25Q16_READ_UNIQUE_ID						          0x4B
#define W25Q16_JEDEC_ID								            0x9F
#define W25Q16_READ_DATA							            0x03
#define W25Q16_FAST_READ							            0x0B
#define W25Q16_FAST_READ_DUAL_OUTPUT				      0x3B
#define W25Q16_FAST_READ_DUAL_IO					        0xBB
#define W25Q16_FAST_READ_QUAD_OUTPUT				      0x6B
#define W25Q16_FAST_READ_QUAD_IO					        0xEB
#define W25Q16_OCTAL_WORD_READ_QUAD_IO				    0xE3

#define W25Q16_DUMMY_BYTE							            0xFF



void W25Q16_Init(void);
void W25Q16_ReadID(uint8_t *ManufacturerID, uint16_t *DevieceID);
void W25Q16_WriteEnable(void);
void W25Q16_WaitBusy(void);
void W25Q16_WriteOnePage(uint32_t addr, uint8_t *data, uint16_t count);
void W25Q16_ReadData(uint32_t addr, uint8_t *data, uint32_t count);
void W25Q16_SectorErase(uint32_t addr);
void W25Q16_WriteMultiPage(uint32_t addr, uint8_t *data, uint16_t count);
void W25Q16_WriteData(uint32_t addr, uint8_t *data, uint16_t count);
#endif
