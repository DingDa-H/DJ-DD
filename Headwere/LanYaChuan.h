#ifndef __LANYACHUAN_H
#define __LANYACHUAN_H
#include <stdio.h>

extern uint8_t LanYaChuan_flag2;
extern char LanYaChuan_RxPacket[];

void LanYaChuan_send(uint8_t byte);
void LanYaChuan_Init(void);
void LanYaChuan_sendarry3(uint8_t *arry,uint16_t lenght);
void LanYaChuan_sendString(char *String);
uint32_t pro1(uint32_t x,uint32_t y);
void LanYaChuan_sendNum(uint32_t Num,uint16_t lenght);
void USART1_IRQHandler(void);
uint16_t LanYaChuan_flag1(void);
void LanYaChuan_Printf(char *format, ...);


#endif