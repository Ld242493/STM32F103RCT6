#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"

/* IIC GPIO Config -------------------------------------------------------------*/
#define IIC_PORT  GPIOB
#define IIC_SCL   GPIO_Pin_10
#define IIC_SDA   GPIO_Pin_11

#define IIC_SCL_Write(x)    GPIO_WriteBit(IIC_PORT, IIC_SCL, (BitAction)x)
#define IIC_SDA_Write(x)    GPIO_WriteBit(IIC_PORT, IIC_SDA, (BitAction)x)
#define IIC_SDA_Read        GPIO_ReadInputDataBit(IIC_PORT, IIC_SDA)        


/* MPU6050 Config -------------------------------------------------------------*/

#define MPU6050_ADDRESS       0x68

#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			  0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C
#define MPU6050_INTBP_CONFIG	0X37

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		  0x75


void IIC_GPIO_Init(void);
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
uint8_t MPU6050_WriteData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data);
uint8_t MPU6050_ReadData(uint8_t MPUAddr, uint8_t RegAddr, uint8_t len, uint8_t *data);
void MPU6050_Init(void);

uint8_t MPU6050_GetID(void);
void MPU6050_GetAccel(int16_t *accx, int16_t *accy, int16_t *accz);
void MPU6050_GetGyro(int16_t *gyrox, int16_t *gyroy, int16_t *gyroz);
float MPU605_GetTemp(void);
#endif
