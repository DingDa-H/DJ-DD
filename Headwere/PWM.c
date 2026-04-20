#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void PWM_Init(void)   //初始化PWM
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//打开时钟，通电	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
//	
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//引脚重映射函数(参数1选择部分重映射，结果是将pa0口换成pa15)
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);  //解除这个东西对pa15的调用
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);        //选择内部时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;     //时基单元初始化
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=100 -1 ;     //ARR自动重装    占空比=ccr/(arr+1)
	TIM_TimeBaseInitStructure.TIM_Prescaler=36 - 1;     //PSC预分频器
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCStructInit(&TIM_OCInitStructure);//给TIM_OCInitStructure赋初始值，这个函数有默认值只要调用就行
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;           //设置输出比较的模式，这里设置为PWM1模式
	//在PWM1模式下：

	//CNT < CCR 时：输出有效电平（通常是高电平）

	//CNT ≥ CCR 时：输出无效电平（通常是低电平）

	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;     //设置输出比较的极性

	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;     //设置输出使能(输出状态)
	TIM_OCInitStructure.TIM_Pulse=10;     //设置CCR寄存器的值
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);     //在TIM2的OC1通道上可以输出PWM波形了，但是要借用GPIO口，下面初始化GPIO
	
	TIM_OC2Init(TIM2,&TIM_OCInitStructure); 
	
	
	TIM_Cmd(TIM2,ENABLE);  //启动定时器2 的命令
	
}

void PWM_CCR1(uint16_t a)
{
	TIM_SetCompare1(TIM2,a); //单独更改CCR1的值（通道1）
}

void PWM_CCR2(uint16_t a)
{
	TIM_SetCompare2(TIM2,a); //单独更改CCR2的值
}