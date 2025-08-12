#include "mpu6050.h"

/* IIC Config -------------------------------------------------------------*/

void IIC_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /*  IIC时钟必须放在GPIO初始化前，否则会读不出数据  */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

  GPIO_InitStructure.GPIO_Pin = IIC_SCL | IIC_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_ClockSpeed = 50000;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_Init(I2C2, &I2C_InitStructure);

  I2C_Cmd(I2C2, ENABLE);
}


/* MPU6050 Config -------------------------------------------------------------*/

/**
 * @brief 硬件I2C等待事件
 * 
 * @param I2Cx 
 * @param I2C_EVENT 
 */
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//给定超时计数时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
	{
		Timeout --;										//等待时，计数值自减
		if (Timeout == 0)								//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;										//跳出等待，不等了
		}
	}
}

/**
 * @brief 寄存器写一个字节数据
 * 
 * @param RegAddr 
 * @param data 
 */
void MPU6050_WriteReg(uint8_t RegAddr, uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);  //硬件I2C生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为发送 */
	I2C_Send7bitAddress(I2C2, (MPU6050_ADDRESS << 1) , I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, RegAddr);		//硬件I2C发送寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	//等待EV8
	
	I2C_SendData(I2C2, data);		//硬件I2C发送数据
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);	//硬件I2C生成终止条件
}

/**
 * @brief 寄存器读一个字节数据
 * 
 * @param RegAddr 
 * @return uint8_t 
 */
uint8_t MPU6050_ReadReg(uint8_t RegAddr)
{
  uint8_t data;

	I2C_GenerateSTART(I2C2, ENABLE);										
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					
	
	I2C_Send7bitAddress(I2C2, (MPU6050_ADDRESS << 1) , I2C_Direction_Transmitter);	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
	
	I2C_SendData(I2C2, RegAddr);											
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);	//硬件I2C生成重复起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为接收 */
	I2C_Send7bitAddress(I2C2, (MPU6050_ADDRESS << 1) , I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);		//在接收最后一个字节之前提前将应答失能
	I2C_GenerateSTOP(I2C2, ENABLE);		      //在接收最后一个字节之前提前申请停止条件
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	//等待EV7
	data = I2C_ReceiveData(I2C2);		//接收数据寄存器
	/*  将应答恢复为使能，为了不影响后续可能产生的读取多字节操作  */
	I2C_AcknowledgeConfig(I2C2, ENABLE);
  return data;
}

/**
 * @brief 寄存器连续写多个字节数据
 * 
 * @param MPUAddr MPU6050从机地址
 * @param RegAddr 寄存器地址
 * @param len 数据长度
 * @param data 字节数据
 * @return uint8_t 
 */
uint8_t MPU6050_WriteData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data)
{
	I2C_GenerateSTART(I2C2, ENABLE);  //硬件I2C生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为发送 */
	I2C_Send7bitAddress(I2C2, (MPUAddr << 1) , I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, RegAddr);		//硬件I2C发送寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	//等待EV8
	for (uint8_t i = 0; i < len; i++)
  {
    I2C_SendData(I2C2, data[i]);		//硬件I2C发送数据
    MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2    
  }	
	I2C_GenerateSTOP(I2C2, ENABLE);	//硬件I2C生成终止条件
  return 0;
}

/**
 * @brief 寄存器连续读多个字节数据
 * 
 * @param MPUAddr MPU6050从机地址
 * @param RegAddr 寄存器地址
 * @param len 数据长度
 * @param data 字节数据
 * @return uint8_t 
 */
uint8_t MPU6050_ReadData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data)
{
	I2C_GenerateSTART(I2C2, ENABLE);										
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					
	
	I2C_Send7bitAddress(I2C2, (MPUAddr << 1) , I2C_Direction_Transmitter);	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	

	I2C_SendData(I2C2, RegAddr);											
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);	//硬件I2C生成重复起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为接收 */
	I2C_Send7bitAddress(I2C2, (MPUAddr << 1) , I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	for (uint8_t i = len; i > 0; i--)
  {
    if (i == 1)
    {
      I2C_AcknowledgeConfig(I2C2, DISABLE);		//在接收最后一个字节之前提前将应答失能
      I2C_GenerateSTOP(I2C2, ENABLE);		      //在接收最后一个字节之前提前申请停止条件      
    }
    MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	//等待EV7
    *data = I2C_ReceiveData(I2C2);		//接收数据寄存器    
    data++;
  }
	/*  将应答恢复为使能，为了不影响后续可能产生的读取多字节操作  */
	I2C_AcknowledgeConfig(I2C2, ENABLE);
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
