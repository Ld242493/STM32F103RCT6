#include "sd_card.h"

uint8_t  sd_type = 0; /* SD卡的类型 */

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

void SPI_Configuration(void)
{
  SPI_GPIO_Init();

  SPI_InitTypeDef   SPI_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  /* SPI1 Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);  
}

/**
 * @brief 硬件SPI传输一个字节数据
 * 
 * @param byte 
 * @return uint8_t 
 */
uint8_t SPI_ReadWrite(uint8_t byte)
{
  /*  发送数据前判断TXE标志位是否为1，发送数据会自动清除TXE标志位  */  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1, byte);
  /*  接收数据前判断RXNE标志位是否为1，接收数据会自动清除RXNE标志位  */  
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SPI1);
}

void SPI_SetSpeed(uint8_t BaudRatePrescaler)
{
  SPI_Cmd(SPI1, DISABLE);           //修改前需要先关闭SPI  
  SPI1->CR1 &= 0xFFC7;              //清空  位3~5
  SPI1->CR1 |= BaudRatePrescaler;   //设置波特率
  SPI_Cmd(SPI1, ENABLE);            //修改后打开SPI
}

/* SD Card Drive Config -------------------------------------------------------------*/

/**
 * @brief       SD卡 取消选择, 释放 SPI总线
 * @param       无
 * @retval      无
 */
static void sd_deselect(void)
{
    SD_CS(1);                       /* 取消SD卡片选 */
    sd_spi_read_write_byte(0xff);   /* 提供额外的8个时钟 */
}


/**
 * @brief       等待卡准备好
 * @param       无
 * @retval      等待结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
static uint8_t sd_wait_ready(void)
{
    uint32_t t = 0;

    do
    {
        if (sd_spi_read_write_byte(0XFF) == 0XFF)
        {
            return SD_OK;   /* OK */
        }

        t++;
    } while (t < 0XFFFFFF); /* 等待 */

    return SD_ERROR;
}

/**
 * @brief       SD卡 选中, 并等待卡准备OK
 * @param       无
 * @retval      选中结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
static uint8_t sd_select(void)
{
    SD_CS(0);

    if (sd_wait_ready() == 0)
    {
        return SD_OK;   /* 等待成功 */
    }

    sd_deselect();
    return SD_ERROR;    /* 等待失败 */
}

/**
 * @brief       等待SD卡回应
 * @param       response : 期待得到的回应值
 * @retval      等待结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
static uint8_t sd_get_response(uint8_t response)
{
    uint16_t count = 0xFFFF;    /* 等待次数 */

    while ((sd_spi_read_write_byte(0XFF) != response) && count)
    {
        count--;    /* 等待得到准确的回应 */
    }

    if (count == 0) /* 等待超时 */
    {
        return SD_ERROR;
    }

    return SD_OK;   /* 正确回应 */
}

/**
 * @brief       从SD卡读取一次数据
 *   @note      读取长度不限, 由len指定
 *              不过一般为512字节
 * @param       buf      : 数据缓存区
 * @param       len      : 要读取的数据长度
 * @retval      读取结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
static uint8_t sd_receive_data(uint8_t *buf, uint16_t len)
{
    if (sd_get_response(0xFE))   /* 等待SD卡发回数据起始令牌0xFE */
    {
        return SD_ERROR;
    }

    while (len--)   /* 开始接收数据 */
    {
        *buf = sd_spi_read_write_byte(0xFF);
        buf++;
    }

    /* 下面是2个伪CRC（dummy CRC） */
    sd_spi_read_write_byte(0xFF);
    sd_spi_read_write_byte(0xFF);

    return SD_OK;   /* 读取成功 */
}

/**
 * @brief       向SD卡写入一个数据包
 *   @note      写入长度固定为512字节!!
 * @param       buf      : 数据缓存区
 * @param       cmd      : 指令
* @retval      发送结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
static uint8_t sd_send_block(uint8_t *buf, uint8_t cmd)
{
    uint16_t t;

    if (sd_wait_ready())            /* 等待ready */
    {
        return SD_ERROR;
    }

    sd_spi_read_write_byte(cmd);    /* 发送 CMD */

    if (cmd != 0XFD)                /* 不是结束指令 */
    {
        for (t = 0; t < 512; t++)
        {
            sd_spi_read_write_byte(buf[t]); /* 发送数据 */
        }

        sd_spi_read_write_byte(0xFF);       /* 忽略crc */
        sd_spi_read_write_byte(0xFF);

        t = sd_spi_read_write_byte(0xFF);   /* 接收响应 */

        if ((t & 0x1F) != 0x05)             /* 数据包没有被接收 */
        {
            return SD_ERROR;
        }
    }

    return SD_OK;   /* 写入成功 */
}

/**
 * @brief       向SD卡发送一个命令
 *   @note      不同命令的CRC值在该函数内部自动确认
 * @param       cmd      : 要发送的命令
 *   @note                 最高位为1 表示ACMD(应用命令)
 *                         最高位为0 表示CMD(普通命令)
 * @param       arg      : 命令的参数
 * @retval      SD卡返回的命令响应
 */
static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg)
{
    uint8_t res;
    uint8_t retry = 0;
    uint8_t crc = 0X01; /* 默认 CRC = 忽略CRC + 停止 */

    if (cmd & 0x80)     /* ACMD发送前, 需要先发送一个 CMD55 命令 */
    {
        cmd &= 0x7F;                    /* 清除最高位, 获取ACMD命令 */
        res = sd_send_cmd(CMD55, 0);    /* 发送CMD55 */

        if (res > 1)
        {
            return res;
        }
    }

    if (cmd != CMD12)   /* 当 cmd 不等于 多块读结束命令时(CMD12), 等待卡选中成功 */
    {
        sd_deselect();  /* 取消上次片选 */

        if (sd_select())
        {
            return 0xFF;/* 选中失败 */
        }
    }

    /* 发送命令包 */
    sd_spi_read_write_byte(cmd | 0x40); /* 起始 + 命令索引号 */
    sd_spi_read_write_byte(arg >> 24);  /* 参数[31 : 24] */
    sd_spi_read_write_byte(arg >> 16);  /* 参数[23 : 16] */
    sd_spi_read_write_byte(arg >> 8);   /* 参数[15 : 8] */
    sd_spi_read_write_byte(arg);        /* 参数[7 : 0] */

    if (cmd == CMD0) crc = 0X95;        /* CMD0 的CRC值固定为 0X95 */

    if (cmd == CMD8) crc = 0X87;        /* CMD8 的CRC值固定为 0X87 */

    sd_spi_read_write_byte(crc);

    if (cmd == CMD12)                   /* cmd 等于 多块读结束命令(CMD12)时 */
    {
        sd_spi_read_write_byte(0xff);   /* CMD12 跳过一个字节 */
    }


    retry = 10; /* 重试次数 */

    do          /* 等待响应，或超时退出 */
    {
        res = sd_spi_read_write_byte(0xFF);
    } while ((res & 0X80) && retry--);

    return res; /* 返回状态值 */
}

/**
 * @brief       获取SD卡的状态
 * @param       无
 * @retval      读取结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_get_status(void)
{
    uint8_t res;
    uint8_t retry = 20; /* 发送ACMD经常失败, 多尝试几次 */

    do
    {
        res = sd_send_cmd(ACMD13, 0);   /* 发ACMD13命令，获取状态 */
    }while(res && retry--);

    sd_deselect();      /* 取消片选 */

    return res;
}

/**
 * @brief       获取SD卡的CID信息, 包括制造商信息等
 * @param       cid_data : 存放CID的内存缓冲区(至少16字节)
 * @retval      读取结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_get_cid(uint8_t *cid_data)
{
    uint8_t res;

    res = sd_send_cmd(CMD10, 0);            /* 发CMD10命令，读CID */

    if (res == 0x00)
    {
        res = sd_receive_data(cid_data, 16);/* 接收16个字节的数据 */
    }

    sd_deselect();  /* 取消片选 */

    return res;
}

/**
 * @brief       获取SD卡的CSD信息，包括容量和速度信息
 * @param       csd_data : 存放CSD的内存缓冲区(至少16字节)
 * @retval      读取结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_get_csd(uint8_t *csd_data)
{
    uint8_t res;
    res = sd_send_cmd(CMD9, 0);             /* 发CMD9命令，读CSD */

    if (res == 0)
    {
        res = sd_receive_data(csd_data, 16);/* 接收16个字节的数据 */
    }

    sd_deselect();  /* 取消片选 */

    return res;
}

/**
 * @brief       获取SD卡的总扇区数(扇区数)
 *   @note      每扇区的字节数必为512, 如果不是512, 则初始化不能通过
 * @param       无
 * @retval      SD卡容量(扇区数), 换成字节数要 * 512
 */
uint32_t sd_get_sector_count(void)
{
    uint8_t csd[16];
    uint32_t capacity;
    uint8_t n;
    uint16_t csize;

    if (sd_get_csd(csd) != 0)       /* 取CSD信息，如果期间出错，返回0 */
    {
        return 0;                   /* 返回0表示获取容量失败 */
    }

    /* 如果为SDHC卡，按照下面方式计算 */
    if ((csd[0] & 0xC0) == 0x40)    /* V2.00的卡 */
    {
        csize = csd[9] + ((uint16_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
        capacity = (uint32_t)csize << 10;       /* 得到扇区数 */
    }
    else    /* V1.XX的卡 / MMC V3卡 */
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
        capacity = (uint32_t)csize << (n - 9);  /* 得到扇区数 */
    }

    return capacity;    /* 注意这里返回的是扇区数量, 不是实际容量的字节数, 换成字节数 得 * 512 */
}

/**
 * @brief       初始化SD卡
 * @param       无
 * @retval      初始化结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_init(void)
{
    uint8_t res;        /*  存放SD卡的返回值 */
    uint16_t retry;     /*  用来进行超时计数 */
    uint8_t ocr[4];
    uint16_t i;
    uint8_t cmd;

    SPI_Configuration();      /* 初始化IO */
    sd_spi_speed_low(); /* 设置到低速模式 */

    for (i = 0; i < 10; i++)
    {
        sd_spi_read_write_byte(0XFF);       /* 发送最少74个脉冲 */
    }

    retry = 20;

    do
    {
        res = sd_send_cmd(CMD0, 0);         /* 进入IDLE状态 */
    } while ((res != 0X01) && retry--);

    sd_type = 0;        /* 默认无卡 */

    if (res == 0X01)
    {
        if (sd_send_cmd(CMD8, 0x1AA) == 1)   /* SD V2.0 */
        {
            for (i = 0; i < 4; i++)
            {
                ocr[i] = sd_spi_read_write_byte(0XFF);  /* 得到R7的32位响应 */
            }

            if (ocr[2] == 0X01 && ocr[3] == 0XAA)       /* 卡是否支持2.7~3.6V */
            {
                retry = 1000;

                do
                {
                    res = sd_send_cmd(ACMD41, 1UL << 30);   /* 发送ACMD41 */
                } while (res && retry--);

                if (retry && sd_send_cmd(CMD58, 0) == 0)    /* 鉴别SD2.0卡版本开始 */
                {
                    for (i = 0; i < 4; i++)
                    {
                        ocr[i] = sd_spi_read_write_byte(0XFF);  /* 得到OCR值 */
                    }

                    if (ocr[0] & 0x40)          /* 检查CCS */
                    {
                        sd_type = SD_TYPE_V2HC; /* V2.0 SDHC卡 */
                    }
                    else
                    {
                        sd_type = SD_TYPE_V2;   /* V2.0 卡 */
                    }
                }
            }
        }
        else     /* SD V1.x / MMC V3 */
        {
            res = sd_send_cmd(ACMD41, 0);   /* 发送ACMD41 */
            retry = 1000;

            if (res <= 1)
            {
                sd_type = SD_TYPE_V1;   /* SD V1卡 */
                cmd = ACMD41;           /* 命令等于 ACMD41 */
            }
            else     /* MMC卡不支持 ACMD41 识别 */
            {
                sd_type = SD_TYPE_MMC;  /* MMC V3 */
                cmd = CMD1;             /* 命令等于 CMD1 */
            }

            do
            {
                res = sd_send_cmd(cmd, 0);  /* 发送 ACMD41 / CMD1 */
            } while (res && retry--);       /* 等待退出IDLE模式 */

            if (retry == 0 || sd_send_cmd(CMD16, 512) != 0)
            {
                sd_type = SD_TYPE_ERR;      /* 错误的卡 */
            }
        }
    }

    sd_deselect();          /* 取消片选 */
    sd_spi_speed_high();    /* 高速模式 */

    if (sd_type)            /* 卡类型有效, 初始化完成 */
    {
        return SD_OK;
    }

    return res;
}

/**
 * @brief       读SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      读取结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t res;
    long long lsaddr = saddr;

    if (sd_type != SD_TYPE_V2HC)
    {
        lsaddr <<= 9;   /* 转换为字节地址 */
    }

    if (cnt == 1)
    {
        res = sd_send_cmd(CMD17, lsaddr);       /* 读命令 */

        if (res == 0)   /* 指令发送成功 */
        {
            res = sd_receive_data(pbuf, 512);   /* 接收512个字节 */
        }
    }
    else
    {
        res = sd_send_cmd(CMD18, lsaddr);       /* 连续读命令 */

        do
        {
            res = sd_receive_data(pbuf, 512);   /* 接收512个字节 */
            pbuf += 512;
        } while (--cnt && res == 0);

        sd_send_cmd(CMD12, 0);  /* 发送停止命令 */
    }

    sd_deselect();  /* 取消片选 */
    return res;
}

/**
 * @brief       写SD卡(fatfs/usb调用)
 * @param       pbuf  : 数据缓存区
 * @param       saddr : 扇区地址
 * @param       cnt   : 扇区个数
 * @retval      写入结果
 *              SD_OK,      成功
 *              SD_ERROR,   失败
 */
uint8_t sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt)
{
    uint8_t retry = 20; /* 发送ACMD经常失败, 多尝试几次 */
    uint8_t res;
    long long lsaddr = saddr;

    if (sd_type != SD_TYPE_V2HC)
    {
        lsaddr <<= 9;   /* 转换为字节地址 */
    }

    if (cnt == 1)
    {
        res = sd_send_cmd(CMD24, lsaddr);   /* 读命令 */

        if (res == 0)   /* 指令发送成功 */
        {
            res = sd_send_block(pbuf, 0xFE);/* 写512个字节 */
        }
    }
    else
    {
        if (sd_type != SD_TYPE_MMC)
        {
            do
            {
                sd_send_cmd(ACMD23, cnt);   /* 发送 ACMD23 指令 */
            }while(res && retry--);
        }

        res = sd_send_cmd(CMD25, lsaddr);   /* 连续读命令 */

        if (res == 0)
        {
            do
            {
                res = sd_send_block(pbuf, 0xFC);    /* 写入一个block, 512个字节 */
                pbuf += 512;
            } while (--cnt && res == 0);

            res = sd_send_block(0, 0xFD);   /* 写入一个block, 512个字节 */
        }
    }

    sd_deselect();/* 取消片选 */
    return res;
}
