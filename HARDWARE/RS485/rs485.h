#ifndef _RS485_H
#define _RS485_H
#include "sys.h"
#define RS485TREN   P04
#define SENDEN      1
#define SENDDIS     0

typedef struct
{
    void (*init)(u32);
    void (*send)(u8*,u32);
    void (*recieve)(void);
    u8 (*crc)(u16,u8*,u8);
    u8 revievedflag;
}TypeDef_RS485;
extern TypeDef_RS485 RS485;
void RS485_Master2slave(u8* buf);
void RS485_Init(u32 baud);
void RS485_Send(u8* buf,u32 num);
void RS485_RecieveDeal(void);
u8 RS485_CRC(u16 CRC_input,u8 buff[],u8 len);
u16 CRC_16bit(u8 buff[],u8 len);
#endif
