#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "main.h"


/* SPI GPIO Config -------------------------------------------------------------*/

#define SPI_CS_Write(x)     HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, (GPIO_PinState)x);
#define SPI_SCK_Write(x)    HAL_GPIO_WritePin(SPI_SCK_GPIO_Port, SPI_SCK_Pin, (GPIO_PinState)x);
#define SPI_MOSI_Write(x)   HAL_GPIO_WritePin(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin, (GPIO_PinState)x);
#define SPI_MISO_Read       HAL_GPIO_ReadPin(SPI_MISO_GPIO_Port, SPI_MISO_Pin)  

#define SPI_Start   SPI_CS_Write(0)
#define SPI_Stop    SPI_CS_Write(1)


/* SD Card Config -------------------------------------------------------------*/

/* SD�� SPI �������� �궨��
 * �����ֲ��ʱ��, ������Ҫʵ��: spi1_read_write_byte �� spi1_set_speed
 * ��������������, SD�� SPIģʽ, ��ͨ������������, ʵ�ֶ�SD���Ĳ���.
 */

uint8_t SPI_ReadWrite(uint8_t byte);
void SPI_SetSpeed(uint8_t BaudRatePrescaler);

#define sd_spi_read_write_byte(x)     SPI_ReadWrite(x)                              /* SD�� SPI��д���� */
#define sd_spi_speed_low()            SPI_SetSpeed(SPI_BAUDRATEPRESCALER_256)       /* SD�� SPI����ģʽ */
#define sd_spi_speed_high()           SPI_SetSpeed(SPI_BAUDRATEPRESCALER_2)         /* SD�� SPI����ģʽ */


/* SD�� Ƭѡ ���� ���� */
#define SD_CS(x)   SPI_CS_Write(x)

/* SD�� ����ֵ���� */
#define SD_OK           0
#define SD_ERROR        1

/* SD�� ���Ͷ��� */
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

/* SD�� ����� */
#define CMD0    (0)             /* GO_IDLE_STATE */
#define CMD1    (1)             /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80 + 41)     /* SEND_OP_COND (SDC) */
#define CMD8    (8)             /* SEND_IF_COND */
#define CMD9    (9)             /* SEND_CSD */
#define CMD10   (10)            /* SEND_CID */
#define CMD12   (12)            /* STOP_TRANSMISSION */
#define ACMD13  (0x80 + 13)     /* SD_STATUS (SDC) */
#define CMD16   (16)            /* SET_BLOCKLEN */
#define CMD17   (17)            /* READ_SINGLE_BLOCK */
#define CMD18   (18)            /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)            /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80 + 23)     /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)            /* WRITE_BLOCK */
#define CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
#define CMD32   (32)            /* ERASE_ER_BLK_START */
#define CMD33   (33)            /* ERASE_ER_BLK_END */
#define CMD38   (38)            /* ERASE */
#define CMD55   (55)            /* APP_CMD */
#define CMD58   (58)            /* READ_OCR */

/* SD�������� */
extern uint8_t  sd_type;

uint8_t sd_init(void);                                              /* SD����ʼ�� */
uint32_t sd_get_sector_count(void);                                 /* ��ȡSD������������(������) */
uint8_t sd_get_status(void);                                        /* ��ȡSD��״̬ */
uint8_t sd_get_cid(uint8_t *cid_data);                              /* ��ȡSD����CID��Ϣ */
uint8_t sd_get_csd(uint8_t *csd_data);                              /* ��ȡSD����CSD��Ϣ */

uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);  /* ��SD��(fatfs/usb����) */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt); /* дSD��(fatfs/usb����) */

#endif
