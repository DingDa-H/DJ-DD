#include "stm32f10x.h"


void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_Structure;
	GPIO_Structure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Structure.GPIO_Pin=GPIO_Pin_13;
	GPIO_Structure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_Structure);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	
}
void LED_ON(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);//设置为低电平0   作用：点亮
}

void LED_OFF(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
}

void LED_turn(void)
{
	if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13)==0)//如果是点亮状态
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);//设置为高电平1   作用：熄灭
	}else{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);//设置为低电平0   作用：点亮
	}
}
