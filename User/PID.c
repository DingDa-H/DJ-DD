#include "stm32f10x.h"                  // Device header
#include "PID.h"

void PID_Init(PID_t *p)
{
	p->ErrorSum = 0;
	p->Error1 = 0;
	p->Error0 = 0;
	p->MuBiao = 0;
	p->ShiJi = 0;
	p->ShiJi0 = 0;
	p->Out = 0;
}

void PID_Updata(PID_t *p)//定义一个名为 p 的指针变量，它指向一个 PID_t 类型的结构体
{
	p->Error1 = p->Error0;
	p->Error0 = p->MuBiao - p->ShiJi;
	
	if(p->Ki!=0)
	{
		p->ErrorSum += p->Error0;
		
		if(p->ErrorSum > p->ErrorSumMax){p->ErrorSum=p->ErrorSumMax;}
		if(p->ErrorSum < p->ErrorSumMin){p->ErrorSum=p->ErrorSumMin;}
	}
	else
	{
		p->ErrorSum = 0;
	}
	
	p->Out = p->Kp*p->Error0 
		   + p->Ki*p->ErrorSum 
//		   + p->Kd*(p->Error0 - p->Error1);
	
	/*----------微分先行-----------*/
		   - p->Kd*(p->ShiJi - p->ShiJi0);
	/*-----------------------------*/
	
	/*------------输出偏移优化-------------*/
	if(p->Out > 0){p->Out += p->OutOffset;}
	if(p->Out < 0){p->Out -= p->OutOffset;}
	/*-------------------------------------*/
	if(p->Out > p->OutMax)
	{
		p->Out=p->OutMax;
	}
	if(p->Out < p->OutMin)
	{
		p->Out=p->OutMin;
	}
	/*----------微分先行-----------*/
	p->ShiJi0=p->ShiJi;
	/*-----------------------------*/
}