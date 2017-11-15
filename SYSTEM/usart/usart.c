#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	 
//start
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
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
//UART��ѡ��Ͳ����ʵ�����
//0ΪUART0,1ΪUART1
void UART_init(u32 baud)
{
    CLK->CLKSEL1&=~(3<<24);         //���ԭʱ��Դ������
    CLK->CLKSEL1|=2<<24;//����Ϊ�ڲ�22.1184MHz
    CLK->CLKDIV&=0XFFFFF0FF;       //UART��Ԥ��ƵΪ0    
    CLK->APBCLK|=1<<16;             //����uart��ʱ��
//     /*********���Բ�������IO��************/
//     CLK->AHBCLK|=1<<0;              //ʹ��GPIO��ʱ��ʹ��
//     GPIOC->PMD&=0XFF0FFFFF;
//     GPIOC->PMD|=0X00400000;         //PB0��Ϊ���룬PB1��Ϊ���
//     SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2��GPIO1.3���ù�������ΪUART   
//     SYS->P1_MFP|=1<<10;             //GPIO1.2λ�ó�ΪTX(UART)
//     SYS->P1_MFP|=1<<11;             //GPIO1.3���ó�ΪRX(UART)    
//     /***************************************/ 
    SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
    
//     SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2��GPIO1.3���ù�������ΪUART   
//     SYS->P1_MFP|=1<<10;             //GPIO1.2λ�ó�ΪTX(UART)
//     SYS->P1_MFP|=1<<11;             //GPIO1.3���ó�ΪRX(UART)	
    
    UART->FCR|=1<<1;//��λ����
    UART->FCR|=1<<2;//��λ����  
#ifdef CLK_INTER22MHZ
    UART->BAUD=(22118400/baud)-2;
#endif
#ifdef CLK_EXT12MHZ
    UART->BAUD=(12000000/baud)-1;  //����������
#endif
   UART->BAUD|=3<<28;//ʹ��DIV_X_EN��DIV_X_ONE,�����÷�ʽ2�Ĳ����ʼ��㹫ʽΪ=UART_CLK/(BUAD+2)
    UART->LCR|=3<<0;             //���ݳ���Ϊ8λ       
    UART->LCR&=~(1<<2);          //1λֹͣλ
//     UART1->TLCTL|=1<<3;//У��λʹ��  //�Ƿ����У�飬���ݲ�ͬ��Ƭ��֮���ͨѶЭ�飬����żУ��ʱ���޷�
//     UART1->TLCTL|=1<<4;//żУ��      //�ڵ����ϴ�ӡ���з�\n
//     UART1->TLCTL|=1<<9;             //RFITL�ķ�ֵ��Ϊ8λ,���ֵ�������䣬Ҫ���ݾ�����������ã�����ͻ�ʹ�ô��͵�����ȱ��һһ����
    UART->IER|=1;               //��������ж�ʹ��
    UART->FUN_SEL=0X00000000;      //����ΪUART����ģʽ
	
     MY_NVIC_Init(UART_IRQn,0);     //����NVIC�жϣ����߼������
}
//�������ݺ�����sendnumΪ��Ҫ���͵��ֽ���Ŀ
//����ֵ0Ϊʧ�ܣ���������ֵΪ���͵��ֽ���
void UART_send(u8 *senddata,u32 bytenum)//�������ݺ���
{
    u32 i;
    for(i=0;i<bytenum;i++)
    {
        UART->THR=senddata[i]; 
        while((UART->FSR & (1<<22))==0);//һֱ�ȴ��Ƿ�Ϊ�գ��յ�ֵΪ1�����ǿ�ֵΪ0
    }
}
void UART_Chair(void)
{
	SYS->P0_MFP&=~(3<<8);
    SYS->P0_MFP&=~(3<<0);
    SYS->P1_MFP&=0XFFFFF3F3;        //P1.2��P1.3���ù�������ΪUART   
    SYS->P1_MFP|=1<<10;             //P1.2λ�ó�ΪTX(UART)
    SYS->P1_MFP|=1<<11;             //P1.3���ó�ΪRX(UART)	
    
}
void UART_Key(void)
{
    SYS->P1_MFP&=0XFFFFF3F3;        //GPIO1.2��GPIO1.3���ù�������ΪGPIO    
	SYS->P0_MFP&=~(3<<8);           //P0.0 P0.1����ΪUART
    SYS->P0_MFP&=~(3<<0);
    SYS->P0_MFP|=3<<8;
    SYS->P0_MFP|=3<<0;
}


#if EN_USART_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[15];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void UART_IRQHandler(void)
{
	u8 res;	
    u16 i = 0;
    u16 len = 0;
	if(UART->ISR&(1<<0))                   //���յ�����
	{	 
		res=UART->RBR; 
		if((USART_RX_STA&0x8000)==0)        //����δ���
		{
			if(USART_RX_STA&0x4000)         //���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else                           //��û�յ�0X0D
			{	
// 				if(res==0x0d)USART_RX_STA|=0x4000;
                if(res==0x0d && ((USART_RX_STA & 0X3FFF) == VALID_NUM))USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
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

