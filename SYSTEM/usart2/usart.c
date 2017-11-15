#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "usart.h"
// #include "screen.h"
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	 
//start
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((UART->FSR&(1<<22))==0);//ѭ������,ֱ���������   
	UART->THR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////
void UART_init(u8 sysclk,u32 baud)
{
    CLK->CLKSEL1&=~(3<<24);          //���ԭʱ��Դ�����ã���ʱ��ԴΪHXT
    CLK->CLKDIV&=0XFFFFF0FF;       //UART��Ԥ��ƵΪ0    
    CLK->APBCLK|=1<<16;             //����uart��ʱ��
    SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
    P0->PMD=0XFFFFFFF0;
    P0->PMD|=1<<0;
    P0->PMD|=0<<2;
    UART->FCR&=~(1<<8);                 //����ʹ��
    UART->FCR|=1<<2;                    //��λ����
    UART->FCR|=1<<1;                    //��λ����
    UART->FUN_SEL=0X00000000;           //UART����ģʽ
    UART->IER|=1<<0;                    //�����ж�ʹ��
    UART->LCR|=3<<0;                    //����λ8λ
    UART->LCR&=~(1<<2);                 //һλֹͣλ
    UART->BAUD|=3<<28;                  //ģʽ2
    UART->BAUD|=((sysclk*1000000/baud)-2)<<0; //����������
    MY_NVIC_Init(UART_IRQn,0);
}
//�������ݺ�����sendnumΪ��Ҫ���͵��ֽ���Ŀ
//����ֵ0Ϊʧ�ܣ���������ֵΪ���͵��ֽ���
void UART_send(u8 *senddata,u32 bytenum)//�������ݺ���
{
    u32 i;
    for(i=0;i<bytenum;i++)
    {
        UART->THR=senddata[i];
        while((UART->FSR&(1<<22))==0);//һֱ�ȴ��Ƿ�Ϊ�գ��յ�ֵΪ1�����ǿ�ֵΪ0
    }
}
#if EN_USART_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ����ݳ���λ
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
u16 USART_RX_LEN=0;         

void UART_IRQHandler(void)
{
	u8 res;	
	if(UART->ISR&(1<<0))                   //���յ�����
	{	 
		res=UART->RBR; 
		if((USART_RX_STA&0x8000)==0)        //����δ���
		{
            USART_RX_BUF[USART_RX_STA&0X3FFF]=res;            
			if(USART_RX_STA&0x4000)         //���յ������ݳ���
			{
                if((USART_RX_STA&0X3FFF)==(USART_RX_LEN+2))
                    USART_RX_STA|=0X8000;
			}
            else                           //��û�յ����ݳ���λ
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
//                     USART_RX_STA|=0X4000;//����Ѿ��յ��˳�������λ
//                 }           
			}
            USART_RX_STA++;
		}  		 									     
	}
} 
#endif								

