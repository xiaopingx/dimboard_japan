#include "rs485.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
u8 RS485_CRC(u16 CRC_input,u8 buff[],u8 len);
TypeDef_RS485 RS485=
{
    RS485_Init,
    RS485_Send,
    RS485_RecieveDeal,
    RS485_CRC,
    0,
};
void RS485_Init(u32 baud)
{
    P0->PMD&=~(3<<8);
    P0->PMD|=1<<8;
    P0->DOUT&=~(1<<4);//默认为接收状态  
    UART_init(baud);
}
//485发送函数，以0X0D,0XOA结束
void RS485_Send(u8* buf,u32 num)
{
    u16 i = 0;
    RS485TREN = SENDEN;//允许发送
//     delay_us(50);
    while(1)
    {
        i ++;
        if(i >= 40){break;}
        delay_us(10);
    }
    UART_send(buf,num);
    printf("\r\n");
    i = 0;
    while(1)
    {
        i ++;
        if(i >= 40)break;
        delay_us(10);
    }
//     delay_us(150);    
    RS485TREN = SENDDIS;//允许接收
     
}
void RS485_RecieveDeal(void)
{
    u8 len=0;
    if(USART_RX_STA&0x8000)
    {    
        len=USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
        RS485_Send(USART_RX_BUF,len);
        USART_RX_STA=0;
    }
}
//16bit的CRC校验
u16 CRC_16bit(u8 buff[],u8 len)
{
    u16 crc = 0xffff;
	u8 i,j,flag;   
	for(i=0;i<len;i++)
	{
		crc^=buff[i];
		for(j=0;j<8;j++)
		{
			flag = crc & 0x01;
			crc>>=1;
			if(flag)
				crc^=0xa001;
		}
    }
    return crc;
}
//RS485通信校验是否正确
//CRC_input为收到数据的CRC值，buff为需要进行校验的数据，len为长度
//若buff所生成的CRC校验值与CRC_input相同，则返回值1，说明校验正确，否则返回值0，校验错误
u8 RS485_CRC(u16 CRC_input,u8 buff[],u8 len)
{
    if( CRC_input == CRC_16bit(buff,len) )
        return 1;//校验正确
    else
        return 0;//校验错误
}
u8 RS485_Recieved(u8* buf)
{
    u8 len=0,i=0;
    u16 crc=0;
    if(USART_RX_STA & 0x8000)//检测是否接收到
    {
        len = USART_RX_STA & 0X3FFF;
        if(len != 7)    //数据长度不等于7，接收不成功
            return 0;
        crc = USART_RX_BUF[5];      //CRC_H
        crc <<= 8;
        crc |= USART_RX_BUF[6];     //CRC_L
        if(USART_RX_BUF[0] == 0XAA && USART_RX_BUF[0] == 0XCC)
        {
            if(RS485_CRC(crc,USART_RX_BUF,5))//CRC检测，若CRC没错误，返回值为1
            {
                for(i=2;i<5;i++)
                    *buf++=USART_RX_BUF[i];
                return 1;//校验成功
            } 
        }
    }
    return 0;//没有接收到数据
}

