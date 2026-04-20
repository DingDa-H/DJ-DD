#ifndef __KEY_H
#define __KEY_H

#define KEY_PRESSED			1 //按下是1
#define KEY_UNPRESSED		0

#define KEY_1				0 //0,1,2,3对应的是数组索引，这里宏定义改了
#define KEY_2				1
#define KEY_3				2
#define KEY_4				3
#define KEY_COUNT			4   //按键数量

#define KEY_HOLD			0X01
#define KEY_DOWN			0X02
#define KEY_UP				0X04
#define KEY_SINGLE			0X08
#define KEY_DOUBLE			0X10
#define KEY_LONG			0X20
#define KEY_PEPEAT			0X40

void Key_Init(void);
void Key_Tick(void);
uint8_t Key_Check(uint8_t n,uint8_t Flag);
uint8_t Key_GetState(uint8_t n);

#endif
