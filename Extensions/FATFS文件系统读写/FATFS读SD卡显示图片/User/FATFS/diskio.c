/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "../SD_CARD/sd_card.h"


/* Ϊÿ���豸����һ�������� */
#define ATA			    0     // Ԥ��SD��ʹ��
#define SPI_FLASH		1     // �ⲿSPI Flash

/*-----------------------------------------------------------------------*/
/* ��ȡ�豸״̬                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* ������ */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
      status = RES_OK;
			break;
    
		case SPI_FLASH:
			break;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* �豸��ʼ��                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* ������ */
)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case ATA:	         /* SD CARD */
      if(sd_init())   status = STA_NOINIT;  
      status = RES_OK;
			break;
    
		case SPI_FLASH:    /* SPI Flash */ 
			break;
	}
	return status;
}


/*-----------------------------------------------------------------------*/
/* ����������ȡ�������ݵ�ָ���洢��                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* �豸������(0..) */
	BYTE *buff,		/* ���ݻ����� */
	DWORD sector,	/* �����׵�ַ */
	UINT count		/* ��������(1..128) */
)
{
  uint8_t res;
	DRESULT status = RES_PARERR;

  if (!count) return status;      /* count���ܵ���0�����򷵻ز������� */

	switch (pdrv) {
		case ATA:	/* SD CARD */
      res = sd_read_disk(buff, sector, count);
      while (res)     /* ������ */
      {
          if (res != 2)sd_init(); /* ���³�ʼ��SD�� */
          res = sd_read_disk(buff, sector, count);
      }
      status = RES_OK;
		break;
     
		case SPI_FLASH:
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

/*-----------------------------------------------------------------------*/
/* д������������д��ָ�������ռ���                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE

DRESULT disk_write (
	BYTE pdrv,			  /* �豸������(0..) */
	const BYTE *buff,	/* ��д�����ݵĻ����� */
	DWORD sector,		  /* �����׵�ַ */
	UINT count			  /* ��������(1..128) */
)
{
  uint8_t res;
	DRESULT status = RES_PARERR;

  if (!count) return status;      /* count���ܵ���0�����򷵻ز������� */

	switch (pdrv) {
		case ATA:	/* SD CARD */    
      res = sd_write_disk((uint8_t *)buff, sector, count);
      while (res)     /* д���� */
      {
          sd_init();  /* ���³�ʼ��SD�� */
          res = sd_write_disk((uint8_t *)buff, sector, count);
          //printf("sd wr error:%d\r\n", res);
      }
      status = RES_OK;
		break;

		case SPI_FLASH:
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* ��������                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* ������ */
	BYTE cmd,		  /* ����ָ�� */
	void *buff		/* д����߶�ȡ���ݵ�ַָ�� */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
      switch (cmd)
      {
          case CTRL_SYNC:
              status = RES_OK;
              break;

          case GET_SECTOR_SIZE:
              *(DWORD *)buff = 512;
              status = RES_OK;
              break;

          case GET_BLOCK_SIZE:
              *(WORD *)buff = 8;
              status = RES_OK;
              break;

          case GET_SECTOR_COUNT:
              *(DWORD *)buff = sd_get_sector_count();
              status = RES_OK;
              break;

          default:
              status = RES_PARERR;
              break;
      }      
		break;
    
		case SPI_FLASH:
      status = RES_OK;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

DWORD get_fattime(void) {
/* ���ص�ǰʱ��� */
return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
    | ((DWORD)1 << 21)				/* Month 1 */
    | ((DWORD)1 << 16)				/* Mday 1 */
    | ((DWORD)0 << 11)				/* Hour 0 */
    | ((DWORD)0 << 5)				  /* Min 0 */
    | ((DWORD)0 >> 1);				/* Sec 0 */
}

