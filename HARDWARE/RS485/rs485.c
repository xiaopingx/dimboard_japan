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
    P0->DOUT&=~(1<<4);//Ĭ��Ϊ����״̬  
    UART_init(baud);
}
//485���ͺ�������0X0D,0XOA����
void RS485_Send(u8* buf,u32 num)
{
    u16 i = 0;
    RS485TREN = SENDEN;//������
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
    RS485TREN = SENDDIS;//�������
     
}
void RS485_RecieveDeal(void)
{
    u8 len=0;
    if(USART_RX_STA&0x8000)
    {    
        len=USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
        RS485_Send(USART_RX_BUF,len);
        USART_RX_STA=0;
    }
}
//16bit��CRCУ��
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
//RS485ͨ��У���Ƿ���ȷ
//CRC_inputΪ�յ����ݵ�CRCֵ��buffΪ��Ҫ����У������ݣ�lenΪ����
//��buff�����ɵ�CRCУ��ֵ��CRC_input��ͬ���򷵻�ֵ1��˵��У����ȷ�����򷵻�ֵ0��У�����
u8 RS485_CRC(u16 CRC_input,u8 buff[],u8 len)
{
    if( CRC_input == CRC_16bit(buff,len) )
        return 1;//У����ȷ
    else
        return 0;//У�����
}
u8 RS485_Recieved(u8* buf)
{
    u8 len=0,i=0;
    u16 crc=0;
    if(USART_RX_STA & 0x8000)//����Ƿ���յ�
    {
        len = USART_RX_STA & 0X3FFF;
        if(len != 7)    //���ݳ��Ȳ�����7�����ղ��ɹ�
            return 0;
        crc = USART_RX_BUF[5];      //CRC_H
        crc <<= 8;
        crc |= USART_RX_BUF[6];     //CRC_L
        if(USART_RX_BUF[0] == 0XAA && USART_RX_BUF[0] == 0XCC)
        {
            if(RS485_CRC(crc,USART_RX_BUF,5))//CRC��⣬��CRCû���󣬷���ֵΪ1
            {
                for(i=2;i<5;i++)
                    *buf++=USART_RX_BUF[i];
                return 1;//У��ɹ�
            } 
        }
    }
    return 0;//û�н��յ�����
}

