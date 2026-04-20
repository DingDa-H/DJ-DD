#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
    /* 1. 使能TIM1的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   // 打开TIM1的时钟（TIM1挂在APB1总线上）
    
    /* 2. 选择定时器时钟源（默认就是内部时钟，可省略） */
    TIM_InternalClockConfig(TIM1);                         // 选择内部时钟作为TIM1的时钟源（72MHz）
    
    /* 3. 定义定时器初始化结构体并配置 */
    TIM_TimeBaseInitTypeDef TimeBaseStructIniture;        // 定义定时器基本配置结构体变量
    
    /* 配置定时器的各个参数 */
    TimeBaseStructIniture.TIM_ClockDivision = TIM_CKD_DIV1;       // 时钟分频：不分频（滤波采样用）
    TimeBaseStructIniture.TIM_CounterMode = TIM_CounterMode_Up;   // 计数模式：向上计数（0→ARR）
    TimeBaseStructIniture.TIM_Period = 1000- 1;                  // 自动重装载值：计数到10000溢出（定时1秒）
    TimeBaseStructIniture.TIM_Prescaler = 72 - 1;                // 预分频值：7200分频（72MHz/7200=10kHz）
    TimeBaseStructIniture.TIM_RepetitionCounter = 0;           // 重复计数器（高级定时器才需要，这里注释掉）
    
    /* 4. 将配置写入TIM1寄存器 */
    TIM_TimeBaseInit(TIM1, &TimeBaseStructIniture);        // 初始化TIM1的时基单元
	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
    
    /* 5. 配置定时器中断 */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);             // 使能TIM1的更新中断（计数器溢出时触发）
    
    /* 6. 配置中断优先级（NVIC） */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);        // 设置NVIC优先级分组为组2（2位抢占优先级，2位响应优先级）
    
    NVIC_InitTypeDef NVIC_InitStrycture;                   // 定义NVIC初始化结构体变量
    NVIC_InitStrycture.NVIC_IRQChannel = TIM1_UP_IRQn;         // 指定中断通道为TIM1中断
    NVIC_InitStrycture.NVIC_IRQChannelCmd = ENABLE;         // 使能该中断通道
    NVIC_InitStrycture.NVIC_IRQChannelPreemptionPriority = 2; // 设置抢占优先级为2
    NVIC_InitStrycture.NVIC_IRQChannelSubPriority = 1;      // 设置响应优先级为1
    NVIC_Init(&NVIC_InitStrycture);                          // 将配置写入NVIC寄存器
    
    /* 7. 启动定时器 */
    TIM_Cmd(TIM1, ENABLE);                                   // 使能TIM1，开始计数
}

//void TIM1_UP_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET)
//	{
//		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
//	}
//}
	