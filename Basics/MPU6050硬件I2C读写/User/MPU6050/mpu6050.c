#include "mpu6050.h"

/* IIC Config -------------------------------------------------------------*/

static void IIC_GPIO_Init(void)
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

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);  //硬件I2C生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为发送 */
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, RegAddress);		//硬件I2C发送寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	//等待EV8
	
	I2C_SendData(I2C2, data);		//硬件I2C发送数据
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);	//硬件I2C生成终止条件

}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
  uint8_t data;

	I2C_GenerateSTART(I2C2, ENABLE);										
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
	
	I2C_SendData(I2C2, RegAddress);											
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);	//硬件I2C生成重复起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5
	/*  硬件I2C发送从机地址，方向为接收 */
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);		//在接收最后一个字节之前提前将应答失能
	I2C_GenerateSTOP(I2C2, ENABLE);		      //在接收最后一个字节之前提前申请停止条件
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	//等待EV7
	data = I2C_ReceiveData(I2C2);		//接收数据寄存器
	/*  将应答恢复为使能，为了不影响后续可能产生的读取多字节操作  */
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
  return data;
}

void MPU6050_Init(void)
{
  IIC_GPIO_Init();

  /*  电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪  */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
  /*  电源管理寄存器2，保持默认值0，所有轴均不待机  */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
  /*  采样率分频寄存器，配置采样率  */
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
  /*  配置寄存器，配置DLPF  */
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			  
  /*  陀螺仪配置寄存器，选择满量程为±2000°  */
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
  /*  加速度计配置寄存器，选择满量程为±16g  */
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	  
}

uint8_t MPU6050_GetID(void)
{
  return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetAccel(int16_t *accx, int16_t *accy, int16_t *accz)
{
  uint8_t dataH, dataL;
  dataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
  *accx = (dataH << 8) | dataL;

  dataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
  *accy = (dataH << 8) | dataL;

  dataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
  *accz = (dataH << 8) | dataL;
}

void MPU6050_GetGyro(int16_t *gyrox, int16_t *gyroy, int16_t *gyroz)
{
  uint8_t dataH, dataL;
  dataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
  *gyrox = (dataH << 8) | dataL;

  dataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
  *gyroy = (dataH << 8) | dataL;

  dataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
  dataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
  *gyroz = (dataH << 8) | dataL;
}