#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include <stdio.h>
#define EN_USART_RX 1
#define CLK_INTER22MHZ
// #define CLK_EXT12MHZ
#define MCU_MINI51S
// #define MCU_NANO100S
#define USART_REC_LEN  			200  	    //定义最大接收字节数 200
#define EN_USART_RX 			1		    //使能（1）/禁止（0）串口1接收
extern u8  USART_RX_BUF[15];     //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		    //接收状态标记	
extern u16 USART_RX_LEN;

#define VALID_NUM       9

void UART_init(u32 baud);
// void UART_init(u8 sysclk,u32 baud);
void UART_send(u8 *senddata,u32 bytenum);
void UART_Chair(void);
void UART_Key(void);
#endif

