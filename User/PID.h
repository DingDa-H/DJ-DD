#ifndef __PID_H
#define __PID_H

typedef struct {
	float MuBiao;
	float ShiJi;
	float ShiJi0;	//用于优化-ShiJi0表示上次实际值
	float Out;
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;
	float Error1;
	float ErrorSum;
	
	float ErrorSumMax;	//用于优化-给积分加上累计上限，防止意外导致饱和
	float ErrorSumMin;	//用于优化
	
	float OutMax;
	float OutMin;
	
	float OutOffset; //输出偏移值（输出比较小时，因为阻力无法运动）
} PID_t;

void PID_Updata(PID_t *p);
void PID_Init(PID_t *p);

#endif