#ifndef __CHAUN_H
#define __CHUAN_H
#include <stdio.h>
void Chuan_send(uint8_t byte);
void Chuan_Init(void);
void Chuan_sendarry3(uint8_t *arry,uint16_t lenght);
void Chuan_sendString(char *String);
void Chuan_sendNum(uint32_t Num,uint16_t lenght);

uint8_t Serial_GetRxFlag(void);
uint8_t Serial_GetRxdata(void);
//uint32_t pro1(uint32_t x,uint32_t y);

#endif