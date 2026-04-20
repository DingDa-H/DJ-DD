#ifndef __MYI2C_H
#define __MYI2C_H


void MYI2C_Init(void);

void MYI2C_Start(void);
void MYI2C_Stop(void);
void MYI2C_SendByte(uint8_t byte);
uint8_t MYI2C_ReciveByte(void);
void MYI2C_SendAck(uint8_t Ackbit);
uint8_t MYI2C_ReciveAck(void);

#endif