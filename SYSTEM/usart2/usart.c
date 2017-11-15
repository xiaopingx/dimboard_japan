#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "usart.h"
// #include "screen.h"
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	 
//start
#if 1
#pragma import(__use_no_semihosting)             
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
void UART_init(u8 sysclk,u32 baud)
{
    CLK->CLKSEL1&=~(3<<24);          //清除原时钟源的设置，即时钟源为HXT
    CLK->CLKDIV&=0XFFFFF0FF;       //UART的预分频为0    
    CLK->APBCLK|=1<<16;             //开启uart的时钟
    SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
    P0->PMD=0XFFFFFFF0;
    P0->PMD|=1<<0;
    P0->PMD|=0<<2;
    UART->FCR&=~(1<<8);                 //接受使能
    UART->FCR|=1<<2;                    //复位发送
    UART->FCR|=1<<1;                    //复位接受
    UART->FUN_SEL=0X00000000;           //UART功能模式
    UART->IER|=1<<0;                    //接受中断使能
    UART->LCR|=3<<0;                    //数据位8位
    UART->LCR&=~(1<<2);                 //一位停止位
    UART->BAUD|=3<<28;                  //模式2
    UART->BAUD|=((sysclk*1000000/baud)-2)<<0; //波特率设置
    MY_NVIC_Init(UART_IRQn,0);
}
//发送数据函数，sendnum为需要发送的字节数目
//返回值0为失败，其他返回值为传送的字节数
void UART_send(u8 *senddata,u32 bytenum)//发送数据函数
{
    u32 i;
    for(i=0;i<bytenum;i++)
    {
        UART->THR=senddata[i];
        while((UART->FSR&(1<<22))==0);//一直等待是否为空（空的值为1），非空值为0
    }
}
#if EN_USART_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到数据长度位
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
u16 USART_RX_LEN=0;         

void UART_IRQHandler(void)
{
	u8 res;	
	if(UART->ISR&(1<<0))                   //接收到数据
	{	 
		res=UART->RBR; 
		if((USART_RX_STA&0x8000)==0)        //接收未完成
		{
            USART_RX_BUF[USART_RX_STA&0X3FFF]=res;            
			if(USART_RX_STA&0x4000)         //接收到了数据长度
			{
                if((USART_RX_STA&0X3FFF)==(USART_RX_LEN+2))
                    USART_RX_STA|=0X8000;
			}
            else                           //还没收到数据长度位
			{	
//                 if(USART_RX_STA==0)
// //                 {
//                     if(USART_RX_BUF[0]!=DWIN_R3)
//                     {
//                         USART_RX_STA=0;
//                         return;
//                     }
//                 }
//                 else if(USART_RX_STA==1)
//                 {
//                     if(USART_RX_BUF[1]!=DWIN_RA)
//                     {
//                         USART_RX_STA=0;
//                         return;
//                     }
//                 }
//                 else if(USART_RX_STA==2)
//                 {
//                     USART_RX_LEN=USART_RX_BUF[2];
//                     USART_RX_STA|=0X4000;//标记已经收到了长度数据位
//                 }           
			}
            USART_RX_STA++;
		}  		 									     
	}
} 
#endif								

