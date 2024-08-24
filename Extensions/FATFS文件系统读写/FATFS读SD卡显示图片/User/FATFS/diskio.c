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


/* 为每个设备定义一个物理编号 */
#define ATA			    0     // 预留SD卡使用
#define SPI_FLASH		1     // 外部SPI Flash

/*-----------------------------------------------------------------------*/
/* 获取设备状态                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* 物理编号 */
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
/* 设备初始化                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* 物理编号 */
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
/* 读扇区：读取扇区内容到指定存储区                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* 设备物理编号(0..) */
	BYTE *buff,		/* 数据缓存区 */
	DWORD sector,	/* 扇区首地址 */
	UINT count		/* 扇区个数(1..128) */
)
{
  uint8_t res;
	DRESULT status = RES_PARERR;

  if (!count) return status;      /* count不能等于0，否则返回参数错误 */

	switch (pdrv) {
		case ATA:	/* SD CARD */
      res = sd_read_disk(buff, sector, count);
      while (res)     /* 读出错 */
      {
          if (res != 2)sd_init(); /* 重新初始化SD卡 */
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
/* 写扇区：见数据写入指定扇区空间上                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE

DRESULT disk_write (
	BYTE pdrv,			  /* 设备物理编号(0..) */
	const BYTE *buff,	/* 欲写入数据的缓存区 */
	DWORD sector,		  /* 扇区首地址 */
	UINT count			  /* 扇区个数(1..128) */
)
{
  uint8_t res;
	DRESULT status = RES_PARERR;

  if (!count) return status;      /* count不能等于0，否则返回参数错误 */

	switch (pdrv) {
		case ATA:	/* SD CARD */    
      res = sd_write_disk((uint8_t *)buff, sector, count);
      while (res)     /* 写出错 */
      {
          sd_init();  /* 重新初始化SD卡 */
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
/* 其他控制                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* 物理编号 */
	BYTE cmd,		  /* 控制指令 */
	void *buff		/* 写入或者读取数据地址指针 */
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
/* 返回当前时间戳 */
return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
    | ((DWORD)1 << 21)				/* Month 1 */
    | ((DWORD)1 << 16)				/* Mday 1 */
    | ((DWORD)0 << 11)				/* Hour 0 */
    | ((DWORD)0 << 5)				  /* Min 0 */
    | ((DWORD)0 >> 1);				/* Sec 0 */
}

