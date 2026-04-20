#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//#define SCL_xain		GPIOB
//#define SCL_pin			GPIO_Pin_10

void MYI2C_W_SCL(uint16_t Bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)Bitvalue);//用来向指定的 GPIO 引脚写入指定电平的函数
//	Delay_us(10);//保险起见，防止mu6050跟不上
}

void MYI2C_W_SDA(uint16_t Bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)Bitvalue);//用来向指定的 GPIO 引脚写入指定电平的函数
//	Delay_us(10);//保险起见，防止mu6050跟不上
}

uint8_t MYI2C_R_SDA(void)
{
	uint8_t Bitvalue;
	Bitvalue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
//	Delay_us(10);
	return Bitvalue;
}
void MYI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Structure;
	GPIO_Structure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Structure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_Structure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Structure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);
	
}

void MYI2C_Start(void) //设置起始条件，先全部置高电平，然后按照顺序sda->scl释放
{
	MYI2C_W_SDA(1);
	MYI2C_W_SCL(1);
	MYI2C_W_SDA(0);
	MYI2C_W_SCL(0);
}

void MYI2C_Stop(void)
{
	MYI2C_W_SDA(0);
	MYI2C_W_SCL(1);
	MYI2C_W_SDA(1);
}

void MYI2C_SendByte(uint8_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		MYI2C_W_SDA(byte & (0x80>>i));//用与操作取最高位(0x80>>i)是右移1位
		MYI2C_W_SCL(1);
		MYI2C_W_SCL(0);
	}
}

uint8_t MYI2C_ReciveByte(void)
{
	uint8_t i,Byte=0x00;
	MYI2C_W_SDA(1); //释放，
	for(i=0;i<8;i++)
	{
		MYI2C_W_SCL(1);
		if(MYI2C_R_SDA()==1)
		{
			Byte = Byte | (0x80>>i);//用或操作取最高位(0x80>>i)是右移1位
		}
		MYI2C_W_SCL(0);
	}
	return Byte;  
}

void MYI2C_SendAck(uint8_t Ackbit)
{
	MYI2C_W_SDA(Ackbit);
	MYI2C_W_SCL(1);
	MYI2C_W_SCL(0);
}

uint8_t MYI2C_ReciveAck(void) //主机读取
{
	uint8_t Ackbit;
	MYI2C_W_SDA(1); //释放
	MYI2C_W_SCL(1);
	Ackbit=MYI2C_R_SDA();
	MYI2C_W_SCL(0); //进去下一个时序单元
	return Ackbit;  
}
