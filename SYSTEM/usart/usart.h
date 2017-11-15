#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include <stdio.h>
#define EN_USART_RX 1
#define CLK_INTER22MHZ
// #define CLK_EXT12MHZ
#define MCU_MINI51S
// #define MCU_NANO100S
#define USART_REC_LEN  			200  	    //�����������ֽ��� 200
#define EN_USART_RX 			1		    //ʹ�ܣ�1��/��ֹ��0������1����
extern u8  USART_RX_BUF[15];     //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		    //����״̬���	
extern u16 USART_RX_LEN;

#define VALID_NUM       9

void UART_init(u32 baud);
// void UART_init(u8 sysclk,u32 baud);
void UART_send(u8 *senddata,u32 bytenum);
void UART_Chair(void);
void UART_Key(void);
#endif

