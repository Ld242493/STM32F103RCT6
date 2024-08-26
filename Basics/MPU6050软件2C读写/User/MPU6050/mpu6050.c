#include "mpu6050.h"

/* IIC Config -------------------------------------------------------------*/

void IIC_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = IIC_SCL | IIC_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
  
  GPIO_SetBits(IIC_PORT, IIC_SCL | IIC_SDA);  //拉高电平，释放总线
}

/**
 * @brief IIC起始条件，先把SDA拉低，然后拉低SCL
 * 
 */
static void IIC_Start(void)
{
  /*  为了兼容指定地址读数据，生成重复起始条件,需要先拉高SDA，然后拉高SCL
      如果先释放SCL，假设此时SDA是低电平的话，随后释放SDA就相当于终止位的情形了 
      或者当SCL高时，可对SDA进行读操作，为防止误读，先将SDA释放   */
  IIC_SDA_Write(1); 
  IIC_SCL_Write(1);
  IIC_SDA_Write(0);
  IIC_SCL_Write(0);
}

/**
 * @brief IIC停止条件，SCL先拉高，SDA再拉高
 * 
 */
static void IIC_Stop(void)
{
  IIC_SDA_Write(0);
  IIC_SCL_Write(1);
  IIC_SDA_Write(1);
}

/**
 * @brief IIC发送一个字节数据，SCL低电平主机写数据到SDA上，高电平从机读数据
 * 
 * @param byte 
 */
static void IIC_SendByte(uint8_t byte)
{
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    IIC_SDA_Write(byte & (0x80 >> i));  //高位先行
    IIC_SCL_Write(1);
    IIC_SCL_Write(0);
  }
}

/**
 * @brief IIC接收一个字节，SCL高电平主机读取数据，低电平从机写数据到SDA
 * 
 * @return uint8_t 
 */
static uint8_t IIC_ReceiveByte(void)
{
  uint8_t i,byte = 0x00;
  /*  主机释放SDA，防止干扰从机写数据  */
  IIC_SDA_Write(1);

  for (i = 0; i < 8; i++)
  {
    IIC_SCL_Write(1);   
    if(IIC_SDA_Read)  byte |= (0x80 >> i);  //读到高电平，byte对应位写1，否则默认为0
    IIC_SCL_Write(0);
  }
  return byte;
}


/**
 * @brief IIC发送一个数据位，SCL低电平主机写数据到SDA上，高电平从机读数据
 * 
 * @param bit 0 表示应答， 1 表示非应答
 */
static void IIC_SendAck(uint8_t bit)
{
    IIC_SDA_Write(bit);
    IIC_SCL_Write(1);
    IIC_SCL_Write(0);
}

/**
 * @brief IIC接收一个字节，SCL高电平主机读取数据，低电平从机写数据到SDA
 * 
 * @return uint8_t 
 */
static uint8_t IIC_ReceiveAck(void)
{
  uint8_t bit;
  /*  主机释放SDA，防止干扰从机写数据  */
  IIC_SDA_Write(1);   //这里写1不代表后面读到的位为1，从机如果有应答会拉低
  IIC_SCL_Write(1);   
  bit = IIC_SDA_Read; // 0 表示接收到应答， 1表示没有接收到应答
  IIC_SCL_Write(0);
  return bit;
}

/* MPU6050 Config -------------------------------------------------------------*/

/**
 * @brief 寄存器写一个字节数据
 *        先发送从机地址，然后发送寄存器地址，最后发送一个字节数据
 * @param RegAddr 寄存器地址
 * @param data 字节数据
 */
void MPU6050_WriteReg(uint8_t RegAddr, uint8_t data)
{
  IIC_Start();
  IIC_SendByte((MPU6050_ADDRESS << 1) | 0);  //最低位为0，默认写数据
  IIC_ReceiveAck();   //应答位不做处理
  IIC_SendByte(RegAddr);
  IIC_ReceiveAck();
  IIC_SendByte(data);
  IIC_ReceiveAck();
  IIC_Stop();
}

/**
 * @brief 读取寄存器一个字节数据
 *        先发送从机地址，然后发送寄存器地址，重新发送从机地址，最后读取一个字节数据
 * @param RegAddr 寄存器地址
 * @return uint8_t 
 */
uint8_t MPU6050_ReadReg(uint8_t RegAddr)
{
  uint8_t data;
  IIC_Start();
  IIC_SendByte((MPU6050_ADDRESS << 1) | 0);  //最低位为0，默认写数据
  IIC_ReceiveAck();   //应答位不做处理
  IIC_SendByte(RegAddr);
  IIC_ReceiveAck();

  IIC_Start();
  IIC_SendByte((MPU6050_ADDRESS << 1) | 1); //最低位为1，设置读数据
  IIC_ReceiveAck();
  data = IIC_ReceiveByte();
  IIC_SendAck(1);
  IIC_Stop();
  
  return data;
}

/**
 * @brief 寄存器连续写多个字节数据
 *        先发送从机地址，然后发送寄存器地址，最后连续发送多个字节数据
 * @param MPUAddr MPU6050从机地址
 * @param RegAddr 寄存器地址
 * @param len 数据长度
 * @param data 字节数据
 */
uint8_t MPU6050_WriteData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data)
{
  IIC_Start();
  IIC_SendByte((MPUAddr << 1) | 0);  //最低位为0，默认写数据
  IIC_ReceiveAck();   //应答位不做处理
  IIC_SendByte(RegAddr);
  IIC_ReceiveAck();
  for (uint8_t i = 0; i < len; i++)
  {
    IIC_SendByte(data[i]);
    IIC_ReceiveAck();    
  }
  IIC_Stop();
  return 0;
}

/**
 * @brief 读取寄存器多个个字节数据
 *        先发送从机地址，然后发送寄存器地址，重新发送从机地址，最后连续读取多个字节数据  
 * @param RegAddr 寄存器地址
 * @return uint8_t 
 */
uint8_t MPU6050_ReadData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data)
{
  IIC_Start();
  IIC_SendByte((MPUAddr << 1) | 0);  //最低位为0，默认写数据
  IIC_ReceiveAck();   //应答位不做处理
  IIC_SendByte(RegAddr);
  IIC_ReceiveAck();

  IIC_Start();
  IIC_SendByte((MPUAddr << 1) | 1); //最低位为1，设置读数据
  IIC_ReceiveAck();
  for (uint8_t i = len; i > 0; i--)
  {
    *data = IIC_ReceiveByte();
    if(i == 1)  IIC_SendAck(1); //最后一个字节发送非应答信号
    else        IIC_SendAck(0);
    data++;
  }
  IIC_Stop();
  return 0;
}

void MPU6050_Init(void)
{
  IIC_GPIO_Init();
	
  /*  电源管理寄存器1，复位  */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80);
  delay_ms(100);
  if(MPU6050_ADDRESS == MPU6050_GetID())
  {
    /*  电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪  */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    /*  电源管理寄存器2，保持默认值0，所有轴均不待机  */
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);    
  }
  else return;
  /*  采样率分频寄存器，配置采样率  */
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
  /*  配置寄存器，配置DLPF  */
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			  
  /*  陀螺仪配置寄存器，选择满量程为±2000°  */
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
  /*  加速度计配置寄存器，选择满量程为±16g  */
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	
  /*  INT引脚低电平有效  */
	MPU6050_WriteReg(MPU6050_INTBP_CONFIG, 0x80);	  
}

/**
 * @brief 读取MPU6050从机地址
 * 
 * @return uint8_t 
 */
uint8_t MPU6050_GetID(void)
{
  return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

/**
 * @brief 获取加速度值
 * 
 * @param accx 
 * @param accy 
 * @param accz 
 */
void MPU6050_GetAccel(int16_t *accx, int16_t *accy, int16_t *accz)
{
  uint8_t data[6];
  MPU6050_ReadData(MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, 6, data);
  *accx = (data[0] << 8) | data[1];
  *accy = (data[2] << 8) | data[3];
  *accz = (data[4] << 8) | data[5];
}

/**
 * @brief 获取陀螺仪值
 * 
 * @param gyrox 
 * @param gyroy 
 * @param gyroz 
 */
void MPU6050_GetGyro(int16_t *gyrox, int16_t *gyroy, int16_t *gyroz)
{
  uint8_t data[6];
  MPU6050_ReadData(MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, 6, data);
  *gyrox = (data[0] << 8) | data[1];
  *gyroy = (data[2] << 8) | data[3];
  *gyroz = (data[4] << 8) | data[5];
}

/**
 * @brief 获取温度值
 * 
 * @return int16_t 
 */
float MPU605_GetTemp(void)
{
  uint8_t data[2];
  int16_t raw;
  MPU6050_ReadData(MPU6050_ADDRESS, MPU6050_TEMP_OUT_H, 2, data);
  raw = (data[0] << 8) | data[1];
  return (36.53 + (double)raw / 340);
}
