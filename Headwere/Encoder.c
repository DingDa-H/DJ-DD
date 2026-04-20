#include "stm32f10x.h"                  // Device header


void Encoder_Init(void)
{
	/*TIM3初始化*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;     //时基单元初始化
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=65536-1;     //ARR自动重装    占空比=ccr/(arr+1)    
	TIM_TimeBaseInitStructure.TIM_Prescaler=0;     //PSC预分频器
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInStructure;
	TIM_ICStructInit(&TIM_ICInStructure);//给TIM_ICInStructure赋初始值，这个函数有默认值只要调用就行
	TIM_ICInStructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInStructure.TIM_ICFilter=0X10;
	TIM_ICInStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInStructure);
	
	TIM_ICInStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInStructure.TIM_ICFilter=0X10;
	TIM_ICInStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM3,&TIM_ICInStructure);      //配置两个通道
	
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	// 别忘了最后启动定时器
	TIM_Cmd(TIM3, ENABLE);
	
	/*TIM4初始化*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=65536-1;     //ARR自动重装    占空比=ccr/(arr+1)    
	TIM_TimeBaseInitStructure.TIM_Prescaler=0;     //PSC预分频器
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ICStructInit(&TIM_ICInStructure);//给TIM_ICInStructure赋初始值，这个函数有默认值只要调用就行
	TIM_ICInStructure.TIM_Channel=TIM_Channel_1;
	TIM_ICInStructure.TIM_ICFilter=0X10;
	TIM_ICInStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM4,&TIM_ICInStructure);
	
	TIM_ICInStructure.TIM_Channel=TIM_Channel_2;
	TIM_ICInStructure.TIM_ICFilter=0X10;
	TIM_ICInStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;
	TIM_ICInit(TIM4,&TIM_ICInStructure);      //配置两个通道
	
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Falling,TIM_ICPolarity_Rising);//修改编码器极性
	// 别忘了最后启动定时器
	TIM_Cmd(TIM4, ENABLE);

}

int16_t Encoder_Get(uint8_t n)//n为1表示左电机
{
	int16_t t;
	//电机旋转cnt自增
	if(n==1)
	{
		t=TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3,0);
		return t;
	}
	else if(n==2)
	{
		t=TIM_GetCounter(TIM4);
		TIM_SetCounter(TIM4,0);
		return t;
	}
	return 0;
}