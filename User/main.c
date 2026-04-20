#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "MPU6050.h"
#include "Mada.h"
#include "Encoder.h"
#include "Chuan.h"
#include "LanYaChuan.h"
#include "PID.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*速度调整宏定义，对应遥感数据赋值给目标值哪里*/
#define	SPEED_LEVEL 		4

int8_t SpeedLevel = SPEED_LEVEL;
/*MUP6050测试*/
float AngleAcc;//用于存储加速度计得到的俯仰角

float AngleGyro;//用于存储角速度计得到的角度值

float Angle;//用于存储滤波后的稳定角度

int16_t AX,AY,AZ,GX,GY,GZ;
uint8_t TimerErrorFlag,TimeCount; 	//用于判断定时中断是否重叠和查看中断时间

uint8_t RunFlag;

int16_t LeftPWM,RightPWM;
int16_t AvePWM,DifPWM;		//AvePWM表示平均PWM、DifPWM表示差分PWM

float LeftSpeed,RightSpeed;
float AveSpeed,DifSpeed;		//AvePWM表示平均、DifPWM表示差分

PID_t AnglePID = {
	.Kp=3,
	.Ki=0.2,
	.Kd=2.2,
	
	.OutMax=100,
	.OutMin=-100,
	.OutOffset=3,
	
	.ErrorSumMax = 400,
	.ErrorSumMin = -400,
};

PID_t SpeedPID = {
	.Kp=1.2,
	.Ki=0.02,
	.Kd=0,
	
	.OutMax=20,
	.OutMin=-20,
	
	.ErrorSumMax = 100,
	.ErrorSumMin = -100,
};

PID_t TurnPID = {
	.Kp=4,
	.Ki=2.6,
	.Kd=0,
	
	.OutMax=50,
	.OutMin=-50,
	
	.ErrorSumMax=20,
	.ErrorSumMin=-20,

};

int main(void)
{
	
	OLED_Init();
	MPU6050_Init();
	LED_Init();
	Key_Init();
	Mada_Init();
	Encoder_Init();
	Chuan_Init();
	LanYaChuan_Init();
	
	Timer_Init();
	
	while(1)
	{
		if(RunFlag)
		{
			LED_ON();
		}
		else
		{
			LED_OFF();
		}
		if(Key_Check(KEY_1,KEY_SINGLE))//按键1按下，启动并且初始化
		{
			PID_Init(&AnglePID);
			PID_Init(&SpeedPID);
			PID_Init(&TurnPID);
			RunFlag=!RunFlag;
		}
		else if(Key_Check(KEY_2,KEY_SINGLE))//按键2按下，加速
		{
			if(SpeedLevel<6)
			SpeedLevel++;
		}
		else if(Key_Check(KEY_3,KEY_SINGLE))//按键3按下，减速
		{
			if(SpeedLevel>2)
			SpeedLevel--;
		}
		
		OLED_Clear();
		OLED_Printf(0,0,OLED_6X8,"  Angle");
		OLED_Printf(0,8,OLED_6X8,"P:%05.2f",AnglePID.Kp);
		OLED_Printf(0,16,OLED_6X8,"I:%05.2f",AnglePID.Ki);
		OLED_Printf(0,24,OLED_6X8,"D:%05.2f",AnglePID.Kd);
		OLED_Printf(0,32,OLED_6X8,"T:%+05.1f",AnglePID.MuBiao);
		OLED_Printf(0,40,OLED_6X8,"A:%+05.1f",Angle);
		OLED_Printf(0,48,OLED_6X8,"O:%+05.0f",AnglePID.Out);
		OLED_Printf(0,56,OLED_6X8,"GY:%+05d",GY);
		OLED_Printf(56,56,OLED_6X8,"Offset:%02.0f",AnglePID.OutOffset);
		
		OLED_Printf(50,0,OLED_6X8,"Speed");
		OLED_Printf(50,8,OLED_6X8,"%05.2f",SpeedPID.Kp);
		OLED_Printf(50,16,OLED_6X8,"%05.2f",SpeedPID.Ki);
		OLED_Printf(50,24,OLED_6X8,"%05.2f",SpeedPID.Kd);
		OLED_Printf(50,32,OLED_6X8,"%+05.1f",SpeedPID.MuBiao);
		OLED_Printf(50,40,OLED_6X8,"%+05.1f",AveSpeed);
		OLED_Printf(50,48,OLED_6X8,"%+05.0f",SpeedPID.Out);
		
		OLED_Printf(88,0,OLED_6X8,"Turn");
		OLED_Printf(88,8,OLED_6X8,"%05.2f",TurnPID.Kp);
		OLED_Printf(88,16,OLED_6X8,"%05.2f",TurnPID.Ki);
		OLED_Printf(88,24,OLED_6X8,"%05.2f",TurnPID.Kd);
		OLED_Printf(88,32,OLED_6X8,"%+05.1f",TurnPID.MuBiao);
		OLED_Printf(88,40,OLED_6X8,"%+05.1f",DifSpeed);
		OLED_Printf(88,48,OLED_6X8,"%+05.0f",TurnPID.Out);
		
		OLED_Update();
		
		if(LanYaChuan_flag2==1)
		{
			char *Tag=strtok(LanYaChuan_RxPacket,",");
			if(strcmp(Tag,"key")==0)//判断是不是收到按键数据包
			{
				char *Name=strtok(NULL,","); //NULL表示substr1的后续分割
				char *Action=strtok(NULL,",");
				
			}
			else if(strcmp(Tag,"slider")==0)
			{
				char *Name=strtok(NULL,","); //NULL表示substr1的后续分割
				char *Value=strtok(NULL,",");
				
				if(strcmp(Name,"AngleKp")==0)
				{
					AnglePID.Kp = atof(Value);
				}
				else if(strcmp(Name,"AngleKi")==0)
				{
					AnglePID.Ki = atof(Value);
				}
				else if(strcmp(Name,"AngleKd")==0)
				{
					AnglePID.Kd = atof(Value);
				}
				else if(strcmp(Name,"SpeedKp")==0)
				{
					SpeedPID.Kp = atof(Value);
				}
				else if(strcmp(Name,"SpeedKi")==0)
				{
					SpeedPID.Ki = atof(Value);
				}
				else if(strcmp(Name,"SpeedKd")==0)
				{
					SpeedPID.Kd = atof(Value);
				}
				else if(strcmp(Name,"TurnKp")==0)
				{
					TurnPID.Kp = atof(Value);
				}
				else if(strcmp(Name,"TurnKi")==0)
				{
					TurnPID.Ki = atof(Value);
				}
				else if(strcmp(Name,"TurnKd")==0)
				{
					TurnPID.Kd = atof(Value);
				}
				else if(strcmp(Name,"Offset")==0)
				{
					AnglePID.OutOffset = atof(Value);
				}
				
			}
			else if(strcmp(Tag,"joystick")==0)
			{
				int8_t LH = atoi(strtok(NULL,","));	//摇杆横向值
				int8_t LV = atoi(strtok(NULL,","));	//摇杆纵向值
				int8_t RH = atoi(strtok(NULL,","));
				int8_t RV = atoi(strtok(NULL,","));
				
				SpeedPID.MuBiao = LV/ 100.0*SpeedLevel;	//LV的范围是（-100，100）要缩放,SpeedLevel默认是4
				TurnPID.MuBiao = RH / 100.0*SpeedLevel;	//RH的范围是（-100，100）要缩放
			}
			
			LanYaChuan_flag2=0;
		}
		LanYaChuan_Printf("[plot,%f,%f,%f]",AnglePID.ErrorSum,SpeedPID.ErrorSum,TurnPID.ErrorSum);
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count0,Count1;

	if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET)
	{
		
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);   //进中断立刻清标志位
		
		Key_Tick();
		
		/***************/
		/*这里是PID控制*/
		/***************/
		Count0++;
		if(Count0>=10)
		{
			Count0=0;
			/*读取计算俯仰角*/
			GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
			
			/*atan的返回值是弧度，需要除以pai,然后*180得到角度
			  atan的返回值只能是90，-90，atan2的返回值是180，-180
			
			  0.01表示中断的时间，也指周期
			  陀螺仪的“±2000°/s”是它每秒钟能测量的“旋转速度”的范围
			  假设你的陀螺仪是16位的，原始输出值范围是 -32768 到 +32767。
			  除以32768后，就把原始值映射到了 -1 到 +1 的范围。
			  AX表示X轴加速度
			  AX/AZ的值是三角函数tan的值，atan可以得到目标角度的弧度值
			*/
			GY-=25;		//对陀螺仪进行校准初步抑制零漂
			
			AngleAcc = -atan2(AX,AZ)/3.14159*180;//
			
			AngleAcc +=2;	//对加速度计进行校准初步抑制零漂
			
			AngleGyro = Angle + GY*0.01/32768*2000;
			
			float a = 0.01;//a是滤波系数，范围（0，1）
			Angle = a * AngleAcc + (1-a)*AngleGyro;
			
			//实现自动停止
			if(Angle>50||Angle<-50)
			{
				RunFlag=0;
			}
			if(RunFlag)
			{
				/***************/
				/*----角度环---*/
				/***************/
				AnglePID.ShiJi = Angle;
				PID_Updata(&AnglePID); //PID_Updata计算函数
				AvePWM = -AnglePID.Out;
				
				LeftPWM = AvePWM + DifPWM/2;
				RightPWM = AvePWM - DifPWM/2;
				
				/*上面的计算会有LeftPWM>100的情况，限制一下*/
				if(LeftPWM > 100){LeftPWM = 100;}
				if(RightPWM > 100){RightPWM = 100;}
				
				if(LeftPWM < -100){LeftPWM = -100;}
				if(RightPWM < -100){RightPWM = -100;}
				
				Mada_SetPWM(1,LeftPWM);//1表示左电机
				Mada_SetPWM(2,RightPWM);
			}
			else
			{
				Mada_SetPWM(1,0);
				Mada_SetPWM(2,0);
			}
		}
		
		Count1++;
		if(Count1>=50)
		{
			Count1=0;
			/***************/
			/*----速度环\转向环---*/
			/***************/
				
			/*磁铁一圈计数44，0.05是50ms，9.27666是减速比*/
			LeftSpeed = Encoder_Get(1)/44.0/0.05/9.27666;
			RightSpeed = Encoder_Get(2)/44.0/0.05/9.27666;
				
			AveSpeed = (LeftSpeed + RightSpeed)/2.0;
			DifSpeed = LeftSpeed - RightSpeed;
				
			if(RunFlag)
			{
				SpeedPID.ShiJi = AveSpeed;
				PID_Updata(&SpeedPID); //PID_Updata计算函数
				AnglePID.MuBiao = SpeedPID.Out;
				
				TurnPID.ShiJi = DifSpeed;
				PID_Updata(&TurnPID);
				DifPWM = TurnPID.Out;
			}
		}
		
		
		if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) //如果又置1，表示中断重叠
		{
			TimerErrorFlag=1;
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update); 
		}
		TimeCount = TIM_GetCounter(TIM1);
	}
}

