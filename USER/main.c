/*
                ���ƻ����ư�
�����д������Ƽ ��Сƽ  ʱ�䣺2015-8-7
2015-08-08: 1���޸������ӻ��Ĵӻ��ϵ���Դ������״̬
            2������WATER�µ����÷�ʽ
2015-11-24������SPITE�Ĳ��书��
2015-11-26: �����Լ칦��
2015-12-1 : ���봫������⹦��
2016-1-5  : �޸Ĵ���ͨѶ����������е�0X0D                                                      By Shoopping
2016-1-6  : ����DC 24V 36Vͨ�ϵĿ���P5.1,��D��������ͨ��                                        By Shoopping
2016-1-7  : 1���޸ĵ��������޷�����ʱ��ִֻ�м����͵Ĺ���                                       By Shoopping
            2���¼����ַΪ0X03�Ĵӻ��������ͽſع���                                           
            3��ԭ���Ӱ���������Ϊ0x04�Ŵӻ�                                                     
2016-1-20 ��������ƻ��տ�ʼ����ʱ�����̰���˸������                                            By Shoopping
2016-1-21 ��������������̼����Ļ��λ�������Դ��������                                        By Shoopping
2016-1-23 : 1���޸���SET״̬ʱ��CUP�պ�ʱҲӦ�򿪼̵���                                             
            2���ڽ�������ʱ��CUP,WATER,DC24V36VӦ�ر�                                           
2016-1-30 : �ڻ��λ����£�S1~S5����ʱ���������죬����ִ��                                       By Shoopping
2016-3-5  : LIGHTΪ����                                                                         By Shoopping
2016-3-9  : LIGHTΪ�޼����ⷽʽ                                                                 By Shoopping
2016-3-28 : �޸����Լ������£���SET�������쳣�����                                             By Shoopping
2016-3-29 : 1���޸���S1~S5,RESET,LP,SP�Զ����������°���SET����ֱ�ӽ���SET״̬                  By Shoopping
            2���޸��ڻ��λ��������£�ֻ�ܽ������ƻ���SET״̬,���ֶ����Զ����Լ�״̬���ܽ���     
            3����SET�߼�״̬(�ֶ����Զ����Լ�״̬)�������ֻ��λ��������˳�SET״̬����ֹͣ����   
2016-3-30 : 1���޸Ķ��ͬʱ����ʱ��ֻ��һ������Ч                                               By Shoopping
            2���޸�������̲�ͬ��ͬʱ����ʱ��ֻ��һ����̵�һ������Ч                           
/___________________________________________________________________________________________________________/
2016-7-11 : �ձ�������


*/
#include "sys.h"
#include "clk.h"
#include "delay.h"
#include "timer.h"
#include "mini51eeprom.h"
#include "watchdog.h"
#include "steplessdimming.h"
int main()
{     
    u32 time=0;//���Ʒ�����Ϣ�����ӻ��ļ��
    MINI51_Clock_Init();
    delay_init(22); 
	TIM0_init(22-1,1000-1);             //TIMER0��ʼ��
	CURE_Stepless_Dimming_Init();	
    DATAFLASH_init(DF_STARTADDR);       //оƬ�ڲ�EEPROM��ʼ��
    WATCHDOG_init(WDT_2POW16,IN_10K);   //�򿪿��Ź������ÿ��Ź�ʱ��Ϊ10KHz,����ʱ��Ϊ1.638S

    while(1)
    { 
        Japan_Dim_Start_ParamterVariable();
         CURE_Peripheral_Carryout();

		
         CURE_Dimming_Detect(&STEPLESSDIMMING);

        //������ѭ�����Ʋ���
        time++;
        if(time > 1000)
        {
            time=0;
            WATCHDOG_feeddog(); //ι���Ź�
        }
        
    }//while����   
}//main()����
