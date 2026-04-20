#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Mada_Init(void)
{
	PWM_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Structure;
	GPIO_Structure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_12;//左电机是12-13
	GPIO_Structure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Structure);
	//GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5);
}

void Mada_SetPWM(uint8_t n,int8_t PWM)//n为1执行左电机操作
{
	if(n==1)
	{
		if(PWM>=0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_12);//设置为高电平
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);//设置为低电平，通过设置高低电平的设置实现正传反转
			PWM_CCR1(PWM);//通过改变ccr的值来控制速度
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_13);//设置为高电平
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);//设置为低电平，通过设置高低电平的设置实现正传反转
			PWM_CCR1(-PWM);//通过改变ccr的值来控制速度,因为参数接收只能接受数值，所以带负号
		}
	
	}
	else if(n==2)
	{
		if(PWM>=0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_15);//设置为高电平
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);//设置为低电平，通过设置高低电平的设置实现正传反转
			PWM_CCR2(PWM);//通过改变ccr的值来控制速度
		}
		else
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_14);//设置为高电平
			GPIO_ResetBits(GPIOB,GPIO_Pin_15);//设置为低电平，通过设置高低电平的设置实现正传反转
			PWM_CCR2(-PWM);//通过改变ccr的值来控制速度,因为参数接收只能接受数值，所以带负号
		}

	}
	
}