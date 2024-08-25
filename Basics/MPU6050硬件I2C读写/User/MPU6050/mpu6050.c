#include "mpu6050.h"

/* IIC Config -------------------------------------------------------------*/

static void IIC_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  /*  IICʱ�ӱ������GPIO��ʼ��ǰ����������������  */
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
	Timeout = 10000;									//������ʱ����ʱ��
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//ѭ���ȴ�ָ���¼�
	{
		Timeout --;										//�ȴ�ʱ������ֵ�Լ�
		if (Timeout == 0)								//�Լ���0�󣬵ȴ���ʱ
		{
			/*��ʱ�Ĵ�������룬������ӵ��˴�*/
			break;										//�����ȴ���������
		}
	}
}

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);  //Ӳ��I2C������ʼ����
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//�ȴ�EV5
	/*  Ӳ��I2C���ʹӻ���ַ������Ϊ���� */
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//�ȴ�EV6
	
	I2C_SendData(I2C2, RegAddress);		//Ӳ��I2C���ͼĴ�����ַ
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	//�ȴ�EV8
	
	I2C_SendData(I2C2, data);		//Ӳ��I2C��������
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//�ȴ�EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);	//Ӳ��I2C������ֹ����

}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
  uint8_t data;

	I2C_GenerateSTART(I2C2, ENABLE);										
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	
	
	I2C_SendData(I2C2, RegAddress);											
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//�ȴ�EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);	//Ӳ��I2C�����ظ���ʼ����
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//�ȴ�EV5
	/*  Ӳ��I2C���ʹӻ���ַ������Ϊ���� */
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//�ȴ�EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);		//�ڽ������һ���ֽ�֮ǰ��ǰ��Ӧ��ʧ��
	I2C_GenerateSTOP(I2C2, ENABLE);		      //�ڽ������һ���ֽ�֮ǰ��ǰ����ֹͣ����
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	//�ȴ�EV7
	data = I2C_ReceiveData(I2C2);		//�������ݼĴ���
	/*  ��Ӧ��ָ�Ϊʹ�ܣ�Ϊ�˲�Ӱ��������ܲ����Ķ�ȡ���ֽڲ���  */
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
  return data;
}

void MPU6050_Init(void)
{
  IIC_GPIO_Init();

  /*  ��Դ����Ĵ���1��ȡ��˯��ģʽ��ѡ��ʱ��ԴΪX��������  */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
  /*  ��Դ����Ĵ���2������Ĭ��ֵ0���������������  */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
  /*  �����ʷ�Ƶ�Ĵ��������ò�����  */
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
  /*  ���üĴ���������DLPF  */
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			  
  /*  ���������üĴ�����ѡ��������Ϊ��2000��  */
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
  /*  ���ٶȼ����üĴ�����ѡ��������Ϊ��16g  */
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