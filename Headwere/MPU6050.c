#include "stm32f10x.h"                  // Device header
#include "MYI2C.h"
#include "MPU6050_Reg.h"                  // Device header

#define MPU6050_Address		0xD0

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)//指定地址写
{
	MYI2C_Start();
	MYI2C_SendByte(MPU6050_Address);      //指定设备
	MYI2C_ReciveAck();
	MYI2C_SendByte(RegAddress);
	MYI2C_ReciveAck();   		//这里应答的判断略过了
	MYI2C_SendByte(Data);      //指定地址
	MYI2C_ReciveAck();
	MYI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)//指定地址读
{
	uint8_t Data;
	
	MYI2C_Start();
	MYI2C_SendByte(MPU6050_Address);      //指定设备
	MYI2C_ReciveAck();
	MYI2C_SendByte(RegAddress);
	MYI2C_ReciveAck();   		//这里应答的判断略过了
	
	MYI2C_Start();
	MYI2C_SendByte(MPU6050_Address+0x01);
	MYI2C_ReciveAck();
	Data=MYI2C_ReciveByte();//从机发送，主机接收
	MYI2C_SendAck(1);//如果想再次接收就发送应答‘0’
	MYI2C_Stop();
	
	return Data;
}

void MPU6050_ReadRegs(uint8_t RegAddress,uint8_t *DataArray,uint8_t Count)//指定地址读
{
	uint8_t Data;
	
	
	MYI2C_Start();
	MYI2C_SendByte(MPU6050_Address);      //指定设备
	MYI2C_ReciveAck();
	MYI2C_SendByte(RegAddress);
	MYI2C_ReciveAck();   		//这里应答的判断略过了
	
	MYI2C_Start();
	MYI2C_SendByte(MPU6050_Address+0x01);
	MYI2C_ReciveAck();
	
	for(uint8_t i=0;i<Count;i++)
	{
		DataArray[i]=MYI2C_ReciveByte();//从机发送，主机接收
		/*I2C读取多字节时，只有最后一个字节给非应答*/
		if(i<Count-1)
		{
			MYI2C_SendAck(0);//如果想再次接收就发送应答‘0’
		}
		else
		{
			MYI2C_SendAck(1);
		}
		
	}
	
	MYI2C_Stop();

}

void MPU6050_Init(void)
{
	MYI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//设置电源寄存器接触睡眠并选择陀螺仪时钟
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);//六个轴不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x07);//设置采样器分频，给7，MPU6050，lms刷新一次
	MPU6050_WriteReg(MPU6050_CONFIG,0x00);	  //低通滤波器，0x06会让数据更平滑，但是有延时18ms左右
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//满量程+-2000°
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//满量程+-16g
}

//void GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,
//	int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)//这里接收数据是up将的第二种指针，我不熟,
//{
//	uint8_t DataH,DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);//低8位
//	
//	*AccX=(DataH<<8)|DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);//低8位
//	
//	*AccY=(DataH<<8)|DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);//低8位
//	
//	*AccZ=(DataH<<8)|DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);//低8位
//	
//	*GyroX=(DataH<<8)|DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);//低8位
//	
//	*GyroY=(DataH<<8)|DataL;
//	
//	DataH=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);//高8位
//	DataL=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);//低8位
//	
//	*GyroZ=(DataH<<8)|DataL;
//}
void GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,
	int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)//这里接收数据是up将的第二种指针，我不熟,
{
	uint8_t Data[14];	//14是根据数据寄存器数量得出来的
	
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H,Data,14);
	
	*AccX=(Data[0]<<8)|Data[1];
	*AccY=(Data[2]<<8)|Data[3];
	*AccZ=(Data[4]<<8)|Data[5];
	
	*GyroX=(Data[8]<<8)|Data[9];
	*GyroY=(Data[10]<<8)|Data[11];
	*GyroZ=(Data[12]<<8)|Data[13];
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}