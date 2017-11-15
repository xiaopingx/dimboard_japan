#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include <stdio.h>
#define USART_REC_LEN  			200  	    //�����������ֽ��� 200
#define EN_USART_RX 			1		    //ʹ�ܣ�1��/��ֹ��0������1����
extern u8  USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		    //����״̬���	
extern u16 USART_RX_LEN;
void UART_init(u8 sysclk,u32 baud);
void UART_send(u8 *senddata,u32 bytenum);
#endif

