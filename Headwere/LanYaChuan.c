#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t LanYaChuan_flag;
uint8_t LanYaChuan_flag2;
char LanYaChuan_RxPacket[100];

void LanYaChuan_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   //配置TX引脚,复用推挽
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;   //配置RX引脚为上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//	USART_ITConfig                        //中断
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//开启中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2,ENABLE);
}

void LanYaChuan_send(uint8_t byte) //发送字节
{
	USART_SendData(USART2,byte);
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
}

void LanYaChuan_sendarry3(uint8_t *arry,uint16_t lenght)//发送数组
{
	uint16_t i;
	for(i=0;i<lenght;i++)
	{
		LanYaChuan_send(arry[i]);
	}
}

void LanYaChuan_sendString(char *String)
{
	uint8_t i;
	for(i=0;String[i]!=0;i++)
	{
		LanYaChuan_send(String[i]);
	}
}

uint32_t pro1(uint32_t x,uint32_t y)//实现x^y,将x固定为10就能实现取余了
{
	uint32_t r=1;
	while(y--){
		r=r*x;
	}
	return r;
}

void LanYaChuan_sendNum(uint32_t Num,uint16_t lenght)
{
	uint16_t i;
	for(i=0;i<lenght;i++)
	{
		
		LanYaChuan_send(Num/pro1(10,lenght-i-1)%10+0x30);
	}
}


void LanYaChuan_flag1(void) //状态机 
{

	uint16_t t;
	t=USART_ReceiveData(USART2);
	static uint8_t Bao=0;
	if(LanYaChuan_flag==0)
	{
		if(t=='['&&LanYaChuan_flag2==0)//单引号：就是一个数字（字符的ASCII码
		{
			Bao=0;
			LanYaChuan_flag=1;
		}
	}
	else if(LanYaChuan_flag==1)
	{
		if(t==']')
		{
			LanYaChuan_RxPacket[Bao]='\0';
			LanYaChuan_flag=0;
			LanYaChuan_flag2=1;
		}
		else
		{
			LanYaChuan_RxPacket[Bao]=t;
			Bao++;
		}
	}
	

}

int fputc(int ch,FILE *f)
{
	LanYaChuan_send(ch);
	return ch;
}
void LanYaChuan_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg,format);
	vsprintf(String,format,arg);
	va_end(arg);
	LanYaChuan_sendString(String);
}

void USART2_IRQHandler(void)           //中断函数
{
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)   //收到数据
	{
		LanYaChuan_flag1();
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);	//清楚标志位
	}
}
