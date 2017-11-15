#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	 
//start
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((UART->FSR&(1<<22))==0);//循环发送,直到发送完毕   
	UART->THR = (u8) ch;      
	return ch;
}
#endif 


//end
//////////////////////////////////////////////////////////////////
//UART的选择和波特率的设置
//0为UART0,1为UART1
void UART_init(u32 baud)
{
    CLK->CLKSEL1&=~(3<<24);         //清除原时钟源的设置
    CLK->CLKSEL1|=2<<24;//设置为内部22.1184MHz
    CLK->CLKDIV&=0XFFFFF0FF;       //UART的预分频为0    
    CLK->APBCLK|=1<<16;             //开启uart的时钟
//     /*********可以不用配置IO口************/
//     CLK->AHBCLK|=1<<0;              //使能GPIO的时钟使能
//     GPIOC->PMD&=0XFF0FFFFF;
//     GPIOC->PMD|=0X00400000;         //PB0设为输入，PB1设为输出
//     SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2和GPIO1.3复用功能设置为UART   
//     SYS->P1_MFP|=1<<10;             //GPIO1.2位置成为TX(UART)
//     SYS->P1_MFP|=1<<11;             //GPIO1.3配置成为RX(UART)    
//     /***************************************/ 
    SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
    
//     SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2和GPIO1.3复用功能设置为UART   
//     SYS->P1_MFP|=1<<10;             //GPIO1.2位置成为TX(UART)
//     SYS->P1_MFP|=1<<11;             //GPIO1.3配置成为RX(UART)	
    
    UART->FCR|=1<<1;//复位接受
    UART->FCR|=1<<2;//复位发送  
#ifdef CLK_INTER22MHZ
    UART->BAUD=(22118400/baud)-2;
#endif
#ifdef CLK_EXT12MHZ
    UART->BAUD=(12000000/baud)-1;  //波特率设置
#endif
   UART->BAUD|=3<<28;//使能DIV_X_EN和DIV_X_ONE,即采用方式2的波特率计算公式为=UART_CLK/(BUAD+2)
    UART->LCR|=3<<0;             //数据长度为8位       
    UART->LCR&=~(1<<2);          //1位停止位
//     UART1->TLCTL|=1<<3;//校验位使能  //是否进行校验，根据不同单片机之间的通讯协议，进行偶校验时，无法
//     UART1->TLCTL|=1<<4;//偶校验      //在电脑上打印换行符\n
//     UART1->TLCTL|=1<<9;             //RFITL的阀值设为8位,这个值不能乱射，要根据具体情况来设置，否则就会使得传送的数据缺少一一部分
    UART->IER|=1;               //允许接受中断使能
    UART->FUN_SEL=0X00000000;      //设置为UART功能模式
	
     MY_NVIC_Init(UART_IRQn,0);     //设置NVIC中断，有线级别最高
}
//发送数据函数，sendnum为需要发送的字节数目
//返回值0为失败，其他返回值为传送的字节数
void UART_send(u8 *senddata,u32 bytenum)//发送数据函数
{
    u32 i;
    for(i=0;i<bytenum;i++)
    {
        UART->THR=senddata[i]; 
        while((UART->FSR & (1<<22))==0);//一直等待是否为空（空的值为1），非空值为0
    }
}
void UART_Chair(void)
{
	SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P1_MFP&=0XFFFFF3F3;        //P1.2和P1.3复用功能设置为UART   
    SYS->P1_MFP|=1<<10;             //P1.2位置成为TX(UART)
    SYS->P1_MFP|=1<<11;             //P1.3配置成为RX(UART)	
    
}
void UART_Key(void)
{
    SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2和GPIO1.3复用功能设置为GPIO    
	SYS->P0_MFP&=~(3<<8);           //P0.0 P0.1配置为UART
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
}


#if EN_USART_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[15];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void UART_IRQHandler(void)
{
	u8 res;	
    u16 i = 0;
    u16 len = 0;
	if(UART->ISR&(1<<0))                   //接收到数据
	{	 
		res=UART->RBR; 
		if((USART_RX_STA&0x8000)==0)        //接收未完成
		{
			if(USART_RX_STA&0x4000)         //接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else                           //还没收到0X0D
			{	
// 				if(res==0x0d)USART_RX_STA|=0x4000;
                if(res==0x0d && ((USART_RX_STA & 0X3FFF) == VALID_NUM))USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}
        len = USART_RX_STA&0x3fff;
        switch(len)
        {
            case 1:
                if(USART_RX_BUF[0] != 0XAA)
                {
                    for(i = 0;i < len;i ++)
                        USART_RX_BUF[i] = 0;
                    USART_RX_STA = 0;
                }
                
            break;
            
            case 2:
                if(USART_RX_BUF[1] != 0X0F)
                {
                    for(i = 0;i < len;i ++)
                        USART_RX_BUF[i] = 0;
                    USART_RX_STA = 0;       
                }
                             
            break;
            default:
                
            break;
        }

	}
} 

#endif								

