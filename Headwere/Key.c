#include "stm32f10x.h"                  // Device header
#include "Key.h"
#include "LED.h"

#define KEY_TIME_DOUBLE		200   //双击时间阈值
#define KEY_TIME_LONG		2000  //长按时间阈值
#define KEY_TIME_REPEAT		100   //重复时间阈值



//uint8_t Key_Flag[8]={0};   //我以为是用数组，up用的二进制，0000 0000
uint8_t Key_Flag[4];  //改成数组了，但是up是为了每个字节对应一个按键
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

uint8_t Key_GetState(uint8_t n) //n对应按键0123
{
	if(n==KEY_1)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0) //按键按下时接地，低电平
		{
			return KEY_PRESSED;//按键按下
		}
	}
	else if(n==KEY_2)
	{
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0) //按键按下时接地，低电平
		{
			return KEY_PRESSED;//按键按下
		}
	}
	else if(n==KEY_3)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0) 
//按键按下时接地，高电平,移植之前的代码注释，
		//这里应该等于0，之前等于1，因为对应的引脚是A15,按键接不到面包板负极
		{
			return KEY_PRESSED;//按键按下
		}
	}
	else if(n==KEY_4)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0) 
		{
			return KEY_PRESSED;//按键按下
		}
	}
	return KEY_UNPRESSED;	//KEY_UNPRESSED表示0
}

uint8_t Key_Check(uint8_t n,uint8_t Flag)
{

	if(Key_Flag[n]&Flag)
	{
		if(KEY_HOLD!=Flag) //KEY_HOLD是中断自动清除的
		{
			Key_Flag[n]&=~Flag; //，确定置标志位后，清除对应标志位
		}
		return 1;
	}
	return 0;
}

void Key_Tick(void) //函数里面加for循环是为了对应多按键
{
	static uint8_t Count,i,CurState[KEY_COUNT],PrevState[KEY_COUNT];//计数，存储本次状态，存储上次状态,静态变量默认值是0
	static uint8_t S[KEY_COUNT];
	static uint16_t TIme[KEY_COUNT];//对于计时这里使用自减计时
	
	for(i=0;i<KEY_COUNT;i++) 
	{
		if(TIme[i]>0)
		{
			TIme[i]--;
		}
	}
	
	Count++;
	if(Count>=20)
	{
		Count=0;
		
		for(i=0;i<KEY_COUNT;i++) 
		{
			PrevState[i] = CurState[i];
			CurState[i]=Key_GetState(i);//获取当前状态
			if(CurState[i]==KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_HOLD;//HOLD = 1        按住不松手,加或与是为了不影响其他位
			}
			else
			{
				Key_Flag[i] &= ~KEY_HOLD;//HOLD = 0        松手
			}
			if(CurState[i]!=0&PrevState[i]==0)  //按键按下瞬间
			{
				Key_Flag[i] |= KEY_DOWN;
			}
			if(CurState[i]==0&PrevState[i]!=0)  //按键松开瞬间,up把0替换成了KEY_UNPRESSED
			{
				Key_Flag[i] |= KEY_UP;
			}
			
			if(S[i]==0)//状态机判断
			{
				if(CurState[i] == KEY_PRESSED)  //判断按键是否按下
				{
					TIme[i] = KEY_TIME_LONG;//2秒，长按时间阈值
					S[i]=1;
				}
			}
			else if(S[i]==1)
			{
				if(CurState[i] == KEY_UNPRESSED)  //检测按键是否松开
				{
					TIme[i] = KEY_TIME_DOUBLE;//双击时间阈值
					S[i]=2;
				}
				else if(TIme[i]==0) //达到了进入长按状态的时间
				{
					TIme[i] = KEY_TIME_REPEAT; //设定需要等待多久进入重复时间
					Key_Flag[i] |=KEY_LONG;//KEY_Flag的LONG位置字节置1
					S[i]=4;				
				}
				
			}
			else if(S[i]==2)
			{
				if(CurState[i] == KEY_PRESSED)  //判断按键是否按下，双击
				{
					S[i]=3;
					Key_Flag[i] |=KEY_DOUBLE;//DOUBLE=1;   KEY_Flag的DOUBLE位置字节置1
				}
				else if(TIme[i]==0) //超出双击时间
				{
					S[i]=0;
					Key_Flag[i] |=KEY_SINGLE;//SINGLE KEY_Flag的SINCLE位置字节置1
				}
			}
			else if(S[i]==3)
			{
				if(CurState[i] == KEY_UNPRESSED)  //判断按键是否松开
				{
					S[i]=0;
				}
			}
			else if(S[i]==4)
			{
				if(CurState[i] == KEY_UNPRESSED)  //判断按键是否松开
				{
					S[i]=0;
				}
				else if(TIme[i]==0) //达到了进入重复状态的时间，开始设置下次进入重复状态的时间
				{
					TIme[i]=KEY_TIME_REPEAT;
					S[i]=4;
					Key_Flag[i] |=KEY_PEPEAT;//SINCLE KEY_Flag的PEPEAT位置字节置1
				}
			}
		}
	}
}