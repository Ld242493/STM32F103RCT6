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
#include "../W25Q16/w25q16.h"


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
  uint8_t ManufacturerID;
  uint16_t DeviceID;
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			break;
    
		case SPI_FLASH:      
      W25Q16_ReadID(&ManufacturerID, &DeviceID);
      /* SPI Flash状态检测：读取SPI Flash 设备ID */
      if((ManufacturerID == 0xEF) && (DeviceID == 0x4015))
        status &= ~STA_NOINIT;  //设备ID读取结果正确
      else
        status = STA_NOINIT;  //设备ID读取结果错误
			break;

		default:
			status = STA_NOINIT;
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
			break;
    
		case SPI_FLASH:    /* SPI Flash */ 
      /* 初始化SPI Flash */
			W25Q16_Init();
      /* 获取SPI Flash芯片状态 */
      status=disk_status(SPI_FLASH);
			break;
      
		default:
			status = STA_NOINIT;
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
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			break;
     
		case SPI_FLASH:
      for (; count > 0; count--)
      {
        W25Q16_ReadData(sector*SectorSize, buff, SectorSize);
        sector++;
        buff += SectorSize;
      }      
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
	DRESULT status = RES_PARERR;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case ATA:	/* SD CARD */      
		break;

		case SPI_FLASH:
      for (; count > 0; count--)
      {
        W25Q16_WriteData(sector*SectorSize, buff, SectorSize); 
        sector++;
        buff += SectorSize;
      }   
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
			break;
    
		case SPI_FLASH:
			switch (cmd) {
        /* 扇区数量：1536*4096/1024/1024=6(MB) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 512;		
        break;
        /* 扇区大小  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = SectorSize;
        break;
        /* 同时擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 16;
        break;        
      }
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

