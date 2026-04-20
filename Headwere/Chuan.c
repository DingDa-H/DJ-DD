#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "LanYaChuan.h"
uint8_t data;
uint8_t Serial_RxFlag;
void Chuan_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;   //配置TX引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;   //配置RX引脚为上拉输入
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
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}
uint8_t Serial_GetRxdata(void)
{
	return data;
}



void Chuan_send(uint8_t byte)
{
	USART_SendData(USART1,byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void Chuan_sendarry3(uint8_t *arry,uint16_t lenght)
{
	uint16_t i;
	for(i=0;i<lenght;i++)
	{
		Chuan_send(arry[i]);
	}
}

void Chuan_sendString(char *String)
{
	uint8_t i;
	for(i=0;String[i]!=0;i++)
	{
		Chuan_send(String[i]);
	}
}

//uint32_t pro1(uint32_t x,uint32_t y)//实现x^y,将x固定为10就能实现取余了
//{
//	uint32_t r=1;
//	while(y--){
//		r=r*x;
//	}
//	return r;
//}

void Chuan_sendNum(uint32_t Num,uint16_t lenght)
{
	uint16_t i;
	for(i=0;i<lenght;i++)
	{
		
		Chuan_send(Num/pro1(10,lenght-i-1)%10+0x30);
	}
}
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}
//int fputc(int ch,FILE *f)
//{
//	Chuan_send(ch);
//	return ch;
//}

void USART1_IRQHandler(void)           //中断函数
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		data = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}