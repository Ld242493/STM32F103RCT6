#include "mpu6050.h"

/* IIC Config -------------------------------------------------------------*/

static void IIC_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = IIC_SCL | IIC_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(IIC_PORT, &GPIO_InitStructure);
  
  GPIO_SetBits(IIC_PORT, IIC_SCL | IIC_SDA);  //���ߵ�ƽ���ͷ�����
}

/**
 * @brief IIC��ʼ�������Ȱ�SDA���ͣ�Ȼ������SCL
 * 
 */
static void IIC_Start(void)
{
  /*  Ϊ�˼���ָ����ַ�����ݣ������ظ���ʼ����,��Ҫ������SDA��Ȼ������SCL
      ������ͷ�SCL�������ʱSDA�ǵ͵�ƽ�Ļ�������ͷ�SDA���൱����ֹλ�������� 
      ���ߵ�SCL��ʱ���ɶ�SDA���ж�������Ϊ��ֹ������Ƚ�SDA�ͷ�   */
  IIC_SDA_Write(1); 
  IIC_SCL_Write(1);
  IIC_SDA_Write(0);
  IIC_SCL_Write(0);
}

/**
 * @brief IICֹͣ������SCL�����ߣ�SDA������
 * 
 */
static void IIC_Stop(void)
{
  IIC_SDA_Write(0);
  IIC_SCL_Write(1);
  IIC_SDA_Write(1);
}

/**
 * @brief IIC����һ���ֽ����ݣ�SCL�͵�ƽ����д���ݵ�SDA�ϣ��ߵ�ƽ�ӻ�������
 * 
 * @param byte 
 */
static void IIC_SendByte(uint8_t byte)
{
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    IIC_SDA_Write(byte & (0x80 >> i));  //��λ����
    IIC_SCL_Write(1);
    IIC_SCL_Write(0);
  }
}

/**
 * @brief IIC����һ���ֽڣ�SCL�ߵ�ƽ������ȡ���ݣ��͵�ƽ�ӻ�д���ݵ�SDA
 * 
 * @return uint8_t 
 */
static uint8_t IIC_ReceiveByte(void)
{
  uint8_t i,byte = 0x00;
  /*  �����ͷ�SDA����ֹ���Ŵӻ�д����  */
  IIC_SDA_Write(1);

  for (i = 0; i < 8; i++)
  {
    IIC_SCL_Write(1);   
    if(IIC_SDA_Read)  byte |= (0x80 >> i);  //�����ߵ�ƽ��byte��Ӧλд1������Ĭ��Ϊ0
    IIC_SCL_Write(0);
  }
  return byte;
}


/**
 * @brief IIC����һ������λ��SCL�͵�ƽ����д���ݵ�SDA�ϣ��ߵ�ƽ�ӻ�������
 * 
 * @param bit 0 ��ʾӦ�� 1 ��ʾ��Ӧ��
 */
static void IIC_SendAck(uint8_t bit)
{
    IIC_SDA_Write(bit);
    IIC_SCL_Write(1);
    IIC_SCL_Write(0);
}

/**
 * @brief IIC����һ���ֽڣ�SCL�ߵ�ƽ������ȡ���ݣ��͵�ƽ�ӻ�д���ݵ�SDA
 * 
 * @return uint8_t 
 */
static uint8_t IIC_ReceiveAck(void)
{
  uint8_t bit;
  /*  �����ͷ�SDA����ֹ���Ŵӻ�д����  */
  IIC_SDA_Write(1);   //����д1��������������λΪ1���ӻ������Ӧ�������
  IIC_SCL_Write(1);   
  bit = IIC_SDA_Read; // 0 ��ʾ���յ�Ӧ�� 1��ʾû�н��յ�Ӧ��
  IIC_SCL_Write(0);
  return bit;
}

/* MPU6050 Config -------------------------------------------------------------*/

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t data)
{
  IIC_Start();
  IIC_SendByte(MPU6050_ADDRESS);
  IIC_ReceiveAck();
  IIC_SendByte(RegAddress);
  IIC_ReceiveAck();
  IIC_SendByte(data);
  IIC_ReceiveAck();
  IIC_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
  uint8_t data;
  IIC_Start();
  IIC_SendByte(MPU6050_ADDRESS);
  IIC_ReceiveAck();
  IIC_SendByte(RegAddress);
  IIC_ReceiveAck();

  IIC_Start();
  IIC_SendByte(MPU6050_ADDRESS | 0x01);
  IIC_ReceiveAck();
  data = IIC_ReceiveByte();
  IIC_SendAck(1);
  IIC_Stop();
  
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