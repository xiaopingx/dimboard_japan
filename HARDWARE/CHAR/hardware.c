#include "hardware.h"
#include "sys.h"
#include "mini51eeprom.h"
#include "timer.h"
#include "usart.h"
#include "rs485.h"
#include "delay.h"
#include "withmemory.h"
#include "steplessdimming.h"
#include "curetype.h"
#include "version_type.h"
/***************************************/
static void CURE_Beep_start(u8 times,u32 worklength,u32 stoplength);
static void CURE_Beep_stop(u8 type);
/***************************************/
static void CURE_Key_UP(void);          //0
static void CURE_Key_DN(void);          //1
static void CURE_Key_BU(void);          //2
static void CURE_Key_BD(void);          //3
static void CURE_Key_PRG(void);         //4
static void CURE_Key_CUP(void);         //5
static void CURE_Key_WATER(void);       //6
static void CURE_Key_HEAT(void);        //7
static void CURE_Key_WATCH(void);       //8
static void CURE_Key_LIGHT(void);       //9
static void CURE_Key_S1(void);          //10
static void CURE_Key_S2(void);          //11
static void CURE_Key_S3(void);          //12
static void CURE_Key_S4(void);          //13
static void CURE_Key_S5(void);          //14
static void CURE_Key_SET(void);         //15
static void CURE_Key_RESET(void);       //16
static void CURE_Key_LP(void);          //17
static void CURE_Key_SP(void);          //18
static void CURE_Key_POWER(void);       //19
static void CURE_Key_A(void);           //20
static void CURE_Key_B(void);           //21
static void CURE_Key_C(void);           //22
static void CURE_Key_D(void);           //23
static void CURE_Key_E(void);           //24
static void CURE_Key_LOCK_GAS(void);    //25
static void CURE_Key_LOCK_CC(void);     //26
/***************************************/
void CURE_Set_Clear(u8 except);


/***************************************/

/***************************************/

/***************************************/

/***************************************/
void Hardware_init(void)
{
    P0->PMD&=~(3<<12);//BEEP
    P0->PMD|=1<<12;
    P0->DOUT&=~(1<<6);
    P0->PMD&=~(3<<2*7);//ֻ��һ��������
    P0->PMD|=1<<2*7;
    P0->DOUT&=~(1<<7);
//------------------------------------------------
    P5->PMD&=~(3<<(5*2));   //P55��WATCH
    P5->PMD|=1<<(5*2);
    P5->DOUT&=~(1<<5);
    P5->PMD&=~(3<<(2*2));   //P52,WATER
    P5->PMD|=1<<(2*2);
    P5->DOUT&=~(1<<2);
//------------------------------------------------	
    P2->PMD=0X00001550 ;
    P2->DOUT&=~(0X3F<<1);
    P3->PMD&=~(3<<2*6);//P3.6ΪWK�ķ���ֵ��Ϊ����ģʽ
//------------------------------------------------
    CURE_DC24V36V_init();
//------------------------------------------------
    //���ƻ��������
    switch(CURETYPE.board)
    {
        case CUREBOARD_1RANK:break;
        case CUREBOARD_3RANK:CURE_3TYPE_LIGHT_Init();break;//�������Ƶ��������� 2016-3-5
        case CUREBOARD_DIM:CURE_Stepless_Dimming_Init();break;//�޼���������У�P0.5Ϊ���뷽ʽ
        default:break;
    }
//------------------------------------------------
}
//ֱ��24V 36Vͨ�Ͽ���
void CURE_DC24V36V_init(void)
{
    P5->PMD &= ~(3 << 1*2);
    P5->PMD |= 1 << 1*2;
    P5->DOUT &= ~(1 << 1);   
}
void CURE_3TYPE_LIGHT_Init(void)
{
    //�������Ƶ��������� 2016-3-5
    //������
    P0->PMD &= ~(3 << (5*2));
    P0->PMD |= 1 << 5*2;
    P0->DOUT &= ~(1 << 5);
    //�ڶ���
    P2->PMD &= ~(3 << (6*2));
    P2->PMD |= 1 << 6*2;
    P2->DOUT &= ~(1 << 6); 
}
//����ṹ��
TypeDef_PRO PRO_dev=
{
	0,//JZ
    0,//DZ
    {0},//RS485REV
    {0XAA,0XAB},//RS485SEND
    0,//addr_count
    0,//addr_err
    0,//temp_dz
};	
//��־�ṹ��
TypeDef_FLAG FLAG=
{
    0,0,0,0,0,0,0,  //up,cup,s1,spite,dn,water,s2
     /************************/
    0,0,0,0,0,0,0,  //bu,heat,s3,bd,watch,s4,s5
     /************************/
    0,0,0,          //prg,set,light
     /************************/
    0,0,0,0,        //cupset,cupsetted,waterset,watersetted
     /************************/
    0,0,0,0,0,      //s1setted,s2setted,s3setted,s4setted,s5setted
     /************************/
    0,0,0,0,         //keyboard1,keyboard2,chairboard,rec_board 
     /************************/
    0,0,0,0,0,0,    //lock_gas1,lock_gas1,lock_cc1,lock_cc2,lock_gas,lock_cc
     /************************/
    0,0,0,            //reset,lp,power
    /*************************/
    0,0,            //dc24v36v,dc24v36vsetted
};
//ʱ������ṹ��
TypeDef_TIMEPARAM TIMEPARAM =
{
    0,//cup
    0,//water
};
//���������ƽṹ��
TypeDef_BEEP BEEP_dev =
{
    CURE_Beep_start,
    CURE_Beep_stop,
    0,          //beepflag
    0,0,        //beepcount[0],beepcount[1]
    0,          //beeptimes
    0,          //beeptimescounter
    0,          //beepcounterflag
    0,          //worktime
    0,          //stoptime
};
//��ֵ�������ṹ��
TypeDef_KEYFUNC KEYFUNC = 
{
    CURE_Key_UP,        //0    
    CURE_Key_DN,        //1
    CURE_Key_BU,        //2
    CURE_Key_BD,        //3
    CURE_Key_PRG,       //4
    CURE_Key_CUP,       //5
    CURE_Key_WATER,     //6
    CURE_Key_HEAT,      //7
    CURE_Key_WATCH,     //8
    CURE_Key_LIGHT,     //9
    CURE_Key_S1,        //10
    CURE_Key_S2,        //11
    CURE_Key_S3,        //12
    CURE_Key_S4,        //13
    CURE_Key_S5,        //14
    CURE_Key_SET,       //15
    CURE_Key_RESET,     //16
    CURE_Key_LP,        //17
    CURE_Key_SP,        //18
    CURE_Key_POWER,     //19
    CURE_Key_A,         //20
    CURE_Key_B,         //21
    CURE_Key_C,         //22
    CURE_Key_D,         //23
    CURE_Key_E,         //24    
    CURE_Key_LOCK_GAS,  //25
    CURE_Key_LOCK_CC,   //26
};
//�˽ṹ����Ҫ������Ӱ��շ������ݽ�����Ӧ�Ĵ���
TypeDef_CHAIR CHAIR=
{
    0,//DZ
    0,//JZ
};
//���Ƚṹ��
TypeDef_HEATED HEATED=
{
    0,//heated
    0,//light_interval
    0,//delaycount
    0,//delayfinish
    0,//count
    0,//first
};
//�����ṹ��
TypeDef_LOCK LOCK=
{
    0,//lock
    0,//counter
    0,//unlock
    0,//start
    0,//effective
    0,//gased
    0,//effective_count
};
//���ýṹ��
TypeDef_SET SET=
{
    0,      //priority
    0,      //start
    0,0,    //count
    0,      //prio_pre
    0,      //manual
    0,      //stage_in
    0,      //stage_out
    0,      //fail
    0,      //chairprio
};
//water�ṹ��
TypeDef_WATER WATER=
{
    1,//type��Ϊ���ֲ�ͬ�İ汾:0Ϊ��ԭʼ�汾��1Ϊ�¼Ӱ汾(2015-11-17)
    0,0,//watercount[2]
    0,//watersettimes
    0,//start
    0,//min
    0,//sec
    0,//setttimes
    0,//work
    0,//unlimit
};
TypeDef_DC24V36V DC24V36V=
{
    0,//time
    0,0,//timecount
    0,//setted
};
TypeDef_CURELIGHT CURELIGHT = 
{
    1,//rank
    0,//ranklast
    0,//timecounter;
    0,//temp
    0,//keylightflag
    0,//stop
    {0},//RANK3_SWITCHDELAY
};
Typedef_KEYBOARD KEYBOARD =
{
    {0},
    {0},
    {0},
};
/************************************************************************/
void KEY_deal(void)
{
    /*********************************************************************/
    //��������
    //25 == ��Դ����
    KEYFUNC.lock_gas();//�Ƚ��л����ж�
    //26 == ���λ���
    KEYFUNC.lock_cc();
    CURE_LOCK_Detect();
    CURE_SetAdvanceStop_Lock();
    if(LOCK.lock == 2)//������Դ����������£�ֻ��SET�����ã���������
    {
        if(PRO_dev.JZ & (1 << POS_SET)) {
            if(LOCK.start == 0){BEEP_dev.start(1,150,300);LOCK.start = 1;PRO_dev.DZ |= 1 << POS_SET;}   
        }
        else{LOCK.start = 0;LOCK.counter = 0;PRO_dev.DZ &= ~(1 << POS_SET);}
        return;
    }
    /********************************************************************/
    //�յ��ļ�ֵ
    /*________________________________________________________________________*/
    //0~7bit
    //0 == UP
    KEYFUNC.up();   
    //1 == DN
    KEYFUNC.dn();
    //2 == BU
    KEYFUNC.bu();
    //3 == BD
    KEYFUNC.bd();
    //4 == PRG
    KEYFUNC.prg();
    //5 == CUP
    KEYFUNC.cup();
    //6 == WATER
    KEYFUNC.water();
    //7 == HEAT
    KEYFUNC.heat();
    /*________________________________________________________________________*/
    //8~15bit
    //8 == WATCH
    KEYFUNC.watch();
    //9 == LIGHT
    KEYFUNC.light();
    //10 == S1
    KEYFUNC.s1();
    //11 == S2
    KEYFUNC.s2();
    //12 == S3
    KEYFUNC.s3();
    //13 == S4
    KEYFUNC.s4();
    //14 == S5
    KEYFUNC.s5();
    //15 == SET
    KEYFUNC.set();
//     /*________________________________________________________________________*/
//     //16~23bit
    //16 == RESET
    KEYFUNC.reset();
    //17 == LPԤ��λ
    KEYFUNC.lp();
    //18 == SP��̵λ
    KEYFUNC.sp();
    //19 == POWER
    KEYFUNC.power();
    //20 == A
    KEYFUNC.a();
    //21 == B
    KEYFUNC.b();
    //22 == C
    KEYFUNC.c();
    //23 == D
    KEYFUNC.d();
    
    /*________________________________________________________________________*/
    //24~31bit
    // 24 == E
    KEYFUNC.e();
    //27Ԥ��
    //28~31Ϊ�ſ�2�ļ�ֵ
    //28 �ſ�2LP
    //29 �ſ�2SP
    //30 �ſ�2CUP
    //31 �ſ�2WATER
}
/*****************************************************************************************************************/ 
void DATAFLASH_init(u32 dfba_startaddr)
{
    u32 temp = 0;
    DATAFLASH.init(dfba_startaddr);
    if(0XAC != DATAFLASH.read(OFFSET_FLAG,DF_PAGE1))
    {
        TIMEPARAM.cup = 15000;//15S
        /**********************************************************/
        TIMEPARAM.water = 5000;//��ʱ����Ϊ5S,��Ӧ15min
        /**********************************************************/
        //���ƻ��������ѡ��
        switch(CURETYPE.board)
        {
            case CUREBOARD_1RANK:CURELIGHT.rank = 1;break;//1��
            case CUREBOARD_3RANK:CURELIGHT.rank = 1;break;//3��
            case CUREBOARD_DIM:STEPLESSDIMMING.param = 10;break;//�޼�����
            default:break;
        }
        /*********************************************************/
        DC24V36V.time = 15000;//15S
        temp |= 1 << POS_A;//abcd�ĵ�ֵ��Ĭ��Ϊ1
        DATAFLASH.erase(DF_PAGE1);
        DATAFLASH.write(DF_PAGE1+OFFSET_CUP,TIMEPARAM.cup);
        DATAFLASH.write(DF_PAGE1+OFFSET_WATER,TIMEPARAM.water);
        /***************************************************************/
        //���ƻ��������ѡ��
        switch(CURETYPE.board)
        {
            case CUREBOARD_1RANK:DATAFLASH.write(DF_PAGE1+OFFSET_LIGHT,CURELIGHT.rank);break;
            case CUREBOARD_3RANK:DATAFLASH.write(DF_PAGE1+OFFSET_LIGHT,CURELIGHT.rank);break;
            case CUREBOARD_DIM:DATAFLASH.write(DF_PAGE1+OFFSET_STEPLESSDIMMING,STEPLESSDIMMING.param);break;
            default:break;
        }
        /***************************************************************/
        DATAFLASH.write(DF_PAGE1+OFFSET_DC24V36V,DC24V36V.time);
        DATAFLASH.write(DF_PAGE1+OFFSET_ABCDZ,temp);
        DATAFLASH.write(DF_PAGE1+OFFSET_FLAG,0XAC);     
    }
    else
    {
        TIMEPARAM.cup=DATAFLASH.read(OFFSET_CUP,DF_PAGE1);
        temp = DATAFLASH.read(OFFSET_WATER,DF_PAGE1);
        if(temp == 0XFFFFAABB)
        {WATER.unlimit = 1;TIMEPARAM.water = temp;}
        else TIMEPARAM.water = temp;
        /**************************************************************/
        //���ƻ��������ѡ��
        switch(CURETYPE.board)
        {
            case CUREBOARD_1RANK:CURELIGHT.rank = DATAFLASH.read(OFFSET_LIGHT,DF_PAGE1);break;
            case CUREBOARD_3RANK:CURELIGHT.rank = DATAFLASH.read(OFFSET_LIGHT,DF_PAGE1);break;
            case CUREBOARD_DIM:STEPLESSDIMMING.param = DATAFLASH.read(OFFSET_STEPLESSDIMMING,DF_PAGE1);break;
            default:break;
        }
        /**************************************************************/
        DC24V36V.time = DATAFLASH.read(OFFSET_DC24V36V,DF_PAGE1);
        temp = DATAFLASH.read(OFFSET_ABCDZ,DF_PAGE1);
        PRO_dev.DZ |= temp;
    }
}
/*****************************************************************************************************************/ 
//�ⲿ��Դִ�к���
void CURE_Peripheral_Carryout(void)
{
    if(FLAG.cup && LOCK.lock != 2) CUP_DRIVER = 1;
    else CUP_DRIVER = 0;
    /*****************************/
    if(FLAG.water && LOCK.lock != 2) WATER_DRIVER = 1;
    else WATER_DRIVER = 0;
    /*****************************/
    if(FLAG.light && LOCK.lock != 2){
        /*****************************************************************************************/
        switch(CURETYPE.board){
            case CUREBOARD_1RANK:LIGHT_L_DRIVER = 1;LIGHT_SAMPLE_DRIVER = 1;break;//һ��
            case CUREBOARD_3RANK://����
                switch(CURELIGHT.RANK3_SWITCHDELAY.step)
                {
                    case 1:
                        switch(CURELIGHT.rank){ 
                            case LIGHT_RANK1:LIGHT_M_DRIVER = 0;LIGHT_H_DRIVER = 0;LIGHT_L_DRIVER = 1;LIGHT_SAMPLE_DRIVER = 1;break;
                            case LIGHT_RANK2:LIGHT_L_DRIVER = 0;LIGHT_H_DRIVER = 0;LIGHT_M_DRIVER = 1;break;
                            case LIGHT_RANK3:LIGHT_L_DRIVER = 0;LIGHT_M_DRIVER = 0;LIGHT_H_DRIVER = 1;break;  
                            default:break;
                        }                        
                    break;
                    case 2:LIGHT_L_DRIVER = 0;LIGHT_M_DRIVER = 0;LIGHT_H_DRIVER = 0;break;
                }
            break;
            case CUREBOARD_DIM:
//                 if(SPITE.work == 0){//SPITEû�й���  
//                 
//                     
//                     LIGHT_L_DRIVER = 1;
//                 }
//                 else {//SPITE�ڹ���״̬ 
//                     
//                     
//                     
//                 }LIGHT_L_DRIVER = 0;
                if(SPITE.work)
                {
                    if(STEPLESSDIMMING.SPITEFUNC.spite)
                    {
                        LIGHT_L_DRIVER = 1;
                    }
                    else
                    {
                        LIGHT_L_DRIVER = 0;
                    }
                }
                else
                {
                    LIGHT_L_DRIVER = 1;
                }
            
                break;//�޼�����
            default:break;
        }
        /*****************************************************************************************/
    }
    else{
        /*****************************************************************************************/
            switch(CURETYPE.board){
                case CUREBOARD_1RANK:LIGHT_L_DRIVER = 0;LIGHT_SAMPLE_DRIVER = 0;break;//һ��
                case CUREBOARD_3RANK:LIGHT_L_DRIVER = 0;LIGHT_M_DRIVER = 0;LIGHT_H_DRIVER = 0;LIGHT_SAMPLE_DRIVER = 0;break;//����
                case CUREBOARD_DIM:LIGHT_L_DRIVER = 0;break;//�޼�����
                default:break;
            }
        /*****************************************************************************************/
        }
     /*****************************/
    if(FLAG.watch && LOCK.lock != 2) WATCH_DRIVER = 1;
    else WATCH_DRIVER = 0;
    /*****************************/
    if((FLAG.heat == 1 && HEATED.heated == 0) && LOCK.lock != 2) HEAT_DRIVER = 1;
    else HEAT_DRIVER = 0;
    /*****************************/
    if(BEEP_dev.beepflag ) BEEP = 1;
    else BEEP = 0;
    /*****************************/   
    if(FLAG.dc24v36v) DC24V36V_DRIVER = 1;
    else DC24V36V_DRIVER = 0;
    /*****************************/ 
}
void CUREBOARD_FirstTime(u8 type)
{
    u8 JZ_BUF[10]={0xAA,0X0F,1,0,0,0,0,0,0},i=0;    
    if(type == 1)
    {
        BEEP=1;
        JZ_BUF[3] = 0XF8;JZ_BUF[4] = 0xFF;JZ_BUF[5] = 0xFF;JZ_BUF[6] = 0xFF;JZ_BUF[7] = 0xFF; 
        JZ_BUF[8] = JZ_BUF[3] ^ JZ_BUF[4] ^ JZ_BUF[5] ^ JZ_BUF[6] ^ JZ_BUF[7];
        JZ_BUF[2] = 0X01;
        RS485_Send(JZ_BUF,9);
        while(1)
        {
            CURE_Communication_Rec_Sample();
            i++;
            if(i>40)//��ʱ50*100us=5ms
            {i = 0;break;}
            delay_us(100);
        }
        JZ_BUF[2]=0X02;
        RS485_Send(JZ_BUF,9);
        while(1)
        {
            CURE_Communication_Rec_Sample();
            i++;
            if(i>40)//��ʱ50*100us=5ms
            {i = 0;break;}
            delay_us(100);
        }    
        JZ_BUF[2]=0X03;
        RS485_Send(JZ_BUF,9); 
    }
    else if(type == 0)
    {
        BEEP=0;
        JZ_BUF[3] = 0XF8;JZ_BUF[4] = 0;JZ_BUF[5] = 0;JZ_BUF[6] = 0;JZ_BUF[7] = 0; 
        JZ_BUF[8] = JZ_BUF[3] ^ JZ_BUF[4] ^ JZ_BUF[5] ^ JZ_BUF[6] ^ JZ_BUF[7];
        JZ_BUF[2] = 0X01;
        RS485_Send(JZ_BUF,9);  
        while(1)
        {
            CURE_Communication_Rec_Sample();
            i++;
            if(i>40)//��ʱ50*100us=5ms
            {i = 0;break;}
            delay_us(100);
        }
        JZ_BUF[2]=0X02;
        RS485_Send(JZ_BUF,9);
        while(1)
        {
            CURE_Communication_Rec_Sample();
            i++;
            if(i>40)//��ʱ50*100us=5ms
            {i = 0;break;}
            delay_us(100);
        }  
        JZ_BUF[2]=0X03;
        RS485_Send(JZ_BUF,9);       
    }
}
/*******************************************************************/
//��������йصĺ���
void CUREBOARD_FirstBeep(void)
{
    u8 i = 0;
    CUREBOARD_FirstTime(1);
    delay_ms(300);
    CUREBOARD_FirstTime(0);
    delay_ms(300);
    CUREBOARD_FirstTime(1);
    delay_ms(300);
    CUREBOARD_FirstTime(0);
    delay_ms(10);
    CUREBOARD_FirstToChair();
    while(i < 100)//�ȴ����Ӱ��ִ�ź�
    {
        CURE_Communication_Recieved();
        i++;
        delay_us(100);
    }
}
void CUREBOARD_FirstToChair(void)
{
    u32 temp = 0;
    temp = CHAIR.JZ;
    PRO_dev.RS485SEND[0] = 0XAA;
    PRO_dev.RS485SEND[1] = 0X0F;
    PRO_dev.RS485SEND[2] = 4;
    PRO_dev.RS485SEND[3] = 0XFF & ~(0X7);
    PRO_dev.RS485SEND[4] = temp & 0XFF;
    PRO_dev.RS485SEND[5] = (temp >> 8) & 0XFF;
    PRO_dev.RS485SEND[6] = (temp >> 16) & 0XFF;
    PRO_dev.RS485SEND[7] = (temp >> 24) & 0XFF;	
    PRO_dev.RS485SEND[8] = PRO_dev.RS485SEND[3] ^ PRO_dev.RS485SEND[4] ^ PRO_dev.RS485SEND[5] ^ PRO_dev.RS485SEND[6] ^ PRO_dev.RS485SEND[7];
    RS485_Send(PRO_dev.RS485SEND,9);//���͵��ǵ�ֵ     
}
//��������ʼ������timesΪ��������Ӧ�Ĵ���
//times:0,ֵ������־��1��������ͬʱ���ô���
static void CURE_Beep_start(u8 times,u32 worklength,u32 stoplength)
{
    if(times)
    {
        BEEP_dev.beeptimes = times;
        BEEP_dev.worktime = worklength;
        BEEP_dev.beeptimescounter = 0; 
        if(times > 1)
            BEEP_dev.stoptime = stoplength;
    }        
    BEEP_dev.beepcount[1]= 0;
    BEEP_dev.beepcount[0]= 0;
    BEEP_dev.beepflag = 1;
}
//����typeΪ0,ֹֻͣ��־λ��������ͬʱֹͣ����
static void CURE_Beep_stop(u8 type)
{
    if(type)
    {
        BEEP_dev.beeptimes = 0;
        BEEP_dev.beeptimescounter = 0;
    }
    BEEP_dev.beepcount[1] = 0;
    BEEP_dev.beepcount[0] = 0;
    BEEP_dev.beepflag = 0; 
}
/**********************************************************************/
static void CURE_Key_UP(void)       //0
{
    if(PRO_dev.JZ & (1 << POS_UP))//UP
    {
        if(FLAG.up == 0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 3)//������״̬ʱ���˳�����״̬��ǰ����������Ч
        { CURE_ExitSet_SaveNone(POS_UP);}
        else {FLAG.up = 1;PRO_dev.DZ |= 1 << POS_UP;} //һ��UP����
                   
    }
    else {FLAG.up=0;PRO_dev.DZ &= ~(1 << POS_UP);}
}
static void CURE_Key_DN(void)       //1
{
    if(PRO_dev.JZ & (1 << POS_DN))//DN
    {
        if(FLAG.dn == 0)
            BEEP_dev.start(1,150,300);;
        if(FLAG.set==1 && SET.priority == 3)
        {CURE_ExitSet_SaveNone(POS_DN);} 
        else{FLAG.dn = 1;PRO_dev.DZ |= 1 << POS_DN;}
    }
    else{FLAG.dn = 0;PRO_dev.DZ &= ~(1 << POS_DN);}
}
static void CURE_Key_BU(void)       //2
{
    if(PRO_dev.JZ & (1 << POS_BU))//BU
    {
        if(FLAG.bu == 0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set==1 && SET.priority == 3)
        {CURE_ExitSet_SaveNone(POS_BU);}  
        else{FLAG.bu = 1;PRO_dev.DZ |= 1 << POS_BU;}        
    }
    else {FLAG.bu=0;PRO_dev.DZ &= ~(1 << POS_BU);}
}
static void CURE_Key_BD(void)       //3
{
    if(PRO_dev.JZ & (1 << POS_BD))//BD
    {
        if(FLAG.bd == 0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 3)
        {CURE_ExitSet_SaveNone(POS_BD);}
        else {FLAG.bd = 1;PRO_dev.DZ |= 1 << POS_BD;}   
    }
    else
    {FLAG.bd = 0;PRO_dev.DZ &= ~(1 << POS_BD);}
}
static void CURE_Key_PRG(void)      //4
{
    if(PRO_dev.JZ& (1 << POS_PRG))//PRG
    {
        if(FLAG.set == 1 && SET.priority == 3)
        {CURE_ExitSet_SaveNone(POS_PRG);}
        else PRO_dev.DZ |= 1 << POS_PRG;
    } 
    else PRO_dev.DZ &= ~(1 << POS_PRG);
}
static void CURE_Key_CUP(void)      //5
{
    if(PRO_dev.JZ & (1 << 5))   //CUP��
    {
        BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 1)                   //��������״̬
        {
            FLAG.cupset = 1;                //cupset == 1ʱ��������ʼ����
            TIME_dev.cupcount[0] = 0;       //���ü���������
            FLAG.cup = 1;
            PRO_dev.DZ |= 1 << 5;
            CURE_Set_Clear( CUP_SETTED );   //���CUP�Ѿ������ù�
        }
        else  //û�д�������״̬
        {
            if(FLAG.cup == 0)         //ԭ���رգ��Ϳ���
            {PRO_dev.DZ |= 1 << 5;FLAG.cup = 1;}
            else
            {
                PRO_dev.DZ &= ~(1 << 5);
                FLAG.cup = 0;  
                TIME_dev.cupcount[1] = 0; 
            }
        }
    }  
    else
    {
        if(FLAG.set == 1 && SET.priority == 1) //CUPʱ������ֹͣ
        {
            FLAG.cupset = 0;//����ֹͣ
            FLAG.cup = 0;
            PRO_dev.DZ &= ~(1 << 5);
        }
        else if(FLAG.cup == 1){PRO_dev.DZ |= 1 << 5;FLAG.cup = 1;}
        else if(FLAG.cup == 0)PRO_dev.DZ &= ~(1 << 5);
    }
}
static void CURE_Key_WATER(void)    //6
{
    switch(WATER.type)
    {
        /**********************************************************************/
        //ԭ���Ŀ��Ʒ�ʽ
        case 0:
            if(PRO_dev.JZ & ( 1 << POS_WATER))//WATER
            {
                if(FLAG.set==1 && SET.priority == 1)//������SET��
                {
                    BEEP_dev.start( WATER.watersettimes + 1,150,300); 
                    FLAG.waterset = 1;
                    CURE_Set_Clear( WATER_SETTED );
                    switch(WATER.watersettimes)
                    {
                        case 0:
                            WATER.watercount[0] = 5000;//5S
                            break;
                        case 1:
                            WATER.watercount[0] = 10000;//10S
                            break;
                        case 2:
                            WATER.watercount[0] = 15000;//15S
                            break;
                        case 3:
                            WATER.watercount[0] = 20000;//20S
                            break;
                        default:
                            break;
                    }    
                    WATER.watersettimes ++;
                    if(WATER.watersettimes > 3)
                        WATER.watersettimes = 0;
                    PRO_dev.DZ |= 1 << 6;
                }
                else//һ���WATER��
                {
                    BEEP_dev.start(1,150,300);
                    if(FLAG.water == 0)         //ԭ���رգ��Ϳ���
                    {
                        PRO_dev.DZ |= 1 << 6;
                        FLAG.water = 1;
                    } 
                    else if(FLAG.water == 1)  //ԭ���������͹رգ���ʱֹͣ
                    {
                        PRO_dev.DZ &= ~(1 << 6);
                        FLAG.water = 0;  
                        WATER.watercount[1] = 0;
                    }    
                }
            }
            else
            { 
                if(FLAG.set == 1 && SET.priority == 1)//�Ѿ�������SET��
                {
                    FLAG.waterset = 1;
                    PRO_dev.DZ &= ~(1 << 6);
                }
                else if(FLAG.water == 1)//һ���WATER
                {
                    PRO_dev.DZ |= 1 << 6;
                    FLAG.water = 1;
                }
                else if(FLAG.water == 0)
                {
                    PRO_dev.DZ &= ~(1 << 6);
                }
            }            
        break;
            
        /**********************************************************************/
        //�µĿ��Ʒ�ʽ:2015-11-17
        case 1:
            if(PRO_dev.JZ & (1 << POS_WATER))
            {
                if(FLAG.set==1 && SET.priority == 1)//������SETģʽ
                {
                    FLAG.waterset = 1;
                    CURE_Set_Clear( WATER_SETTED );
                    WATER.setttimes++;
                    if(WATER.setttimes >= 3)
                    {WATER.setttimes = 3;WATER.work = 1;}
                    WATER.start = 1;
                    PRO_dev.DZ |= 1 << 6;
                }
                else//һ��ģʽ
                {
                    BEEP_dev.start(1,150,300);
                    if(FLAG.water == 0)         //ԭ���رգ��Ϳ���
                    {PRO_dev.DZ |= 1 << 6;FLAG.water = 1;}
                    else if(FLAG.water == 1)  //ԭ���������͹رգ���ʱֹͣ
                    {PRO_dev.DZ &= ~(1 << 6);FLAG.water = 0;WATER.watercount[1] = 0;}                     
                }
            }
            else
            {
                if(FLAG.set == 1 && SET.priority == 1)//�Ѿ�������SET��
                {WATER.start = 0;FLAG.waterset = 1;PRO_dev.DZ &= ~(1 << 6);}
                else if(FLAG.water == 1)//һ���WATER
                {PRO_dev.DZ |= 1 << 6;FLAG.water = 1;}
                else if(FLAG.water == 0)
                {PRO_dev.DZ &= ~(1 << 6);}            
            }
        break;
    }
}
static void CURE_Key_HEAT(void)     //7
{
    if( PRO_dev.JZ & (1 << 7) )//HEAT
    {

        BEEP_dev.start(1,150,300);
        if(FLAG.heat == 0)         //ԭ���رգ��Ϳ���
        {
            FLAG.heat = 1;
        } 
        else if(FLAG.heat == 1)  //ԭ���������͹رգ���ʱֹͣ
        {
            FLAG.heat = 0;  
        }
    }
    else
    {
        if(FLAG.heat == 1)//
        {PRO_dev.DZ |= 1 << 7;FLAG.heat = 1;}
        else if(FLAG.heat == 0)
        {PRO_dev.DZ &= ~(1 << 7);}        
    }  
}
static void CURE_Key_WATCH(void)    //8
{
    if(PRO_dev.JZ & (1 << 8))          //WATCH
    {
        BEEP_dev.start(1,150,300);
        if(FLAG.watch == 0)           //ԭ���رգ��Ϳ���
        {PRO_dev.DZ |= 1 << 8;FLAG.watch = 1;} 
        else if(FLAG.watch == 1)      //ԭ���������͹رգ���ʱֹͣ
        {PRO_dev.DZ &= ~(1 << 8);FLAG.watch = 0;}         
    }
    else
    {
        if(FLAG.watch == 1)           //һ���WATER
        {PRO_dev.DZ |= 1 << 8;FLAG.watch = 1;}
        else if(FLAG.watch == 0)
        {PRO_dev.DZ &= ~(1 << 8);}        
    }
}
static void CURE_Key_LIGHT(void)    //9
{
    if(PRO_dev.JZ & (1 << POS_LIGHT)){//LIGHT
        switch(CURETYPE.board){
            case CUREBOARD_1RANK:BEEP_dev.start(1,150,300);CURE_1RANK_Deal(KEY_DOWN);break;//һ��
            case CUREBOARD_3RANK:CURE_3RANK_Deal(&CURELIGHT,KEY_DOWN);break;               //����
            case CUREBOARD_DIM:CURE_Dimming_KeyDeal(&STEPLESSDIMMING,KEY_DOWN);break;      //�޼����� 
            default:break;
        }
    }
    else{ 
        switch(CURETYPE.board){
            case CUREBOARD_1RANK:CURE_1RANK_Deal(KEY_UP);break;                     //һ��
            case CUREBOARD_3RANK:CURE_3RANK_Deal(&CURELIGHT,KEY_UP);break;          //����
            case CUREBOARD_DIM:CURE_Dimming_KeyDeal(&STEPLESSDIMMING,KEY_UP);break; //�޼�����
            default:break;
        }
    }
}
static void CURE_Key_S1(void)       //10��S1Ϊ�����
{
    if(PRO_dev.JZ & (1 << POS_S1))//S1
    {
        if(LOCK.lock == 1)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 1 && LOCK.lock == 0)
            CURE_Set_Clear( S1_SETTED );
        else if(FLAG.set == 1 && (SET.priority == 2 || SET.priority == 3))
            CURE_ExitSet_SaveNone(POS_S1);        
    }        
}
static void CURE_Key_S2(void)       //11��S2Ϊ�����
{
    
    if(PRO_dev.JZ & (1 << POS_S2))
    {      
        if(LOCK.lock == 1)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 1 && LOCK.lock == 0)//��SET״̬ʱ���˳�SET״̬��������
            CURE_Set_Clear( S2_SETTED );
        else if(FLAG.set == 1 && (SET.priority == 2 || SET.priority == 3))
            CURE_ExitSet_SaveNone(POS_S2);
    }   
}
static void CURE_Key_S3(void)       //12��S3Ϊ�����
{
    if(PRO_dev.JZ & (1 << POS_S3))
    {  
        if(LOCK.lock == 1)
            BEEP_dev.start(1,150,300);        
        if(FLAG.set == 1 && SET.priority == 1 && LOCK.lock == 0)//��SET״̬ʱ���˳�SET״̬��������
            CURE_Set_Clear( S3_SETTED );
        else if(FLAG.set == 1 && (SET.priority == 2 || SET.priority == 3))
            CURE_ExitSet_SaveNone(POS_S3);        
    }   
}
static void CURE_Key_S4(void)       //13��S4Ϊ�����
{
    if(PRO_dev.JZ & (1 << POS_S4))
    {   
        if(LOCK.lock == 1)
            BEEP_dev.start(1,150,300);        
        if(FLAG.set == 1 && SET.priority == 1 && LOCK.lock == 0)//��SET״̬ʱ���˳�SET״̬��������
            CURE_Set_Clear( S5_SETTED ); 
        else if(FLAG.set == 1 && (SET.priority == 2 || SET.priority == 3))
            CURE_ExitSet_SaveNone(POS_S4);
    }   
}
static void CURE_Key_S5(void)       //14��S5��Ϊ���װ�
{

    if(PRO_dev.JZ & (1 << POS_S5))
    {      
        if(LOCK.lock == 1)
            BEEP_dev.start(1,150,300);        
        if(FLAG.set == 1 && SET.priority == 1 && LOCK.lock == 0)//��SET״̬ʱ���˳�SET״̬��������
            CURE_Set_Clear( S5_SETTED ); 
        else if(FLAG.set == 1 && (SET.priority == 2 || SET.priority == 3))
            CURE_ExitSet_SaveNone(POS_S5);
    }    
}
static void CURE_Key_SET(void)      //15
{
    if(PRO_dev.JZ & (1 << POS_SET))          //SET��
    {
        if(FLAG.set == 0)             //ԭ���رգ��Ϳ���
        {
            BEEP_dev.start(1,150,300);
            CURE_SetEnter_Peripheral_Stop();
            /****************************************************/
            //2016-3-29
            switch(LOCK.lock)
            {
                case 0:
                    if(SENSOR.sensor == 0){//���ڴ���������������²��ܽ��н�һ���ļ���������ֻ�ܽ���һ�������
                        SET.start = 1;      //������ʼ��־      
                        SET.prio_pre = 0;
                    }
                    else BEEP_dev.start(1,150,300);                    
                break;
                case 1:
                    BEEP_dev.start(1,150,300);
                break;
                
                case 2:
                break;
            }
            /****************************************************/

            
            SET.priority = 1;   //�ս�������״̬������Ϊ���ȼ�1
            FLAG.set = 1;       //SET��־          
            PRO_dev.DZ |= 1 << POS_SET;
        }
        else if(FLAG.set == 1)      //ԭ���������͹ر�
        {
            switch(SET.priority)
            {
                case 1:                             //���ȼ���1�����ƻ�������
                    BEEP_dev.start(1,150,300);
                    PRO_dev.DZ &= ~(1 << POS_SET);
                    FLAG.set = 0;
                    CURE_SET_Sure(SET.priority);    //ȷ������
                    SET.priority = 0;   
                    if(SENSOR.sensor == 0){
                        SET.stage_out = 1;              //�˳�
                    }
                break;
                case 2:                             //�ֶ��������ã���Ҫ������SET���˳�
//                    BEEP_dev.start(1,150,300);
                    switch(SET.manual)
                    {
                        case 0:
//                            BEEP_dev.start(1,150,300);
                            SET.manual = 1;
                            SET.stage_in = 1;
                        break;
                        case 1:
//                            BEEP_dev.start(1,150,300);
                            PRO_dev.DZ &= ~(1 << POS_SET);
                            FLAG.set = 0;
                            SET.manual =0;
                            SET.priority = 0;
                            SET.stage_out = 1;
                        break;
                    }
                break;
//                 case 3:                             //�Զ���������״̬,�ڴ�״̬������SET�����˳���������
// //                    BEEP_dev.start(1,150,300);
//                     PRO_dev.DZ &= ~(1 << POS_SET);
//                     FLAG.set = 0;
//                     SET.priority = 0;
//                     SET.stage_out = 6;//�������˳�
//                 break;    
                
                 /***********************************************************************************/
                 //�¼Ӵ��룺Ϊ�Լ�����   2015-11-26
                case 3://�Զ���������״̬���ڴ�״̬������SET��������Լ�״̬��ͬʱSETҲ�˳�������
                    PRO_dev.DZ &= ~(1 << POS_SET);
                    FLAG.set = 0;
                    SET.priority = 0;
                    SET.stage_out = 0;//��ʼ�����Լ�״̬
                    CHECKSELF.stage_in = 1;
                    CHECKSELF.cmdpre = CHECKSELF_FUNC_START;
                break;
                /*************************************************************/
                default:break;
            }
        }
    }
    else
    {
        if(FLAG.set == 0)
        {PRO_dev.DZ &= ~(1 << POS_SET);FLAG.set = 0;}
        else if(FLAG.set == 1)
        {
            if(SET.start)//���ڼ�ʱ��ֹͣ��ʱ
            {SET.start = 0;SET.priority = SET.prio_pre;}
            SET.count[0] = 0;
            SET.count[1] = 0; 
            SET.stage_in = 0;
            SET.prio_pre = 0;
            PRO_dev.DZ |= 1 << POS_SET;
            FLAG.set = 1;
        }         
    }    
}
static void CURE_Key_RESET(void)    //16
{
    if(PRO_dev.JZ & (1 << POS_RESET))
    {   
        if(FLAG.reset ==0 )
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1)//��SET״̬ʱ���˳�SET״̬��������
            CURE_ExitSet_SaveNone(POS_RESET);
        FLAG.reset = 1;
    }
    else
    {FLAG.reset = 0;}
}
static void CURE_Key_LP(void)       //17
{
    if(PRO_dev.JZ & (1 << POS_LP))
    { 
        if(FLAG.lp == 0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1)//��SET״̬ʱ���˳�SET״̬��������
            CURE_ExitSet_SaveNone(POS_LP);
        FLAG.lp = 1;
    }
    else
    {
        FLAG.lp = 0;
    }
}
static void CURE_Key_SP(void)       //18
{
    if(PRO_dev.JZ & (1 << POS_SP))
    { 
        if(FLAG.spite == 0 && SENSOR.sensor ==0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1)//��SET״̬ʱ���˳�SET״̬��������
            CURE_ExitSet_SaveNone(POS_SP);
        FLAG.spite = 1;
    }
    else
    {
        FLAG.spite = 0;
    }

}
static void CURE_Key_POWER(void)    //19
{
    if(PRO_dev.JZ & (1 << POS_POWER))
    {   
        if(FLAG.power == 0)
            BEEP_dev.start(1,150,300);
        if(FLAG.set == 1)//��SET״̬ʱ���˳�SET״̬��������
            CURE_ExitSet_SaveNone(POS_POWER);
        FLAG.power = 1;
    }
    else
    {FLAG.power = 0;}
}
static void CURE_Key_A(void)        //20
{

}
static void CURE_Key_B(void)        //21
{

}
static void CURE_Key_C(void)        //22
{

}
//D�����ڿ���DC 24V 36V��ͨ��
static void CURE_Key_D(void)        //23
{
    if(PRO_dev.JZ & (1 << POS_D))                   //D��
    {
        BEEP_dev.start(1,150,300);
        if(FLAG.set == 1 && SET.priority == 1)      //��������״̬
        {
            DC24V36V.setted = 1;                    //DC24V36V.setted = 1ʱ��������ʼ����
            DC24V36V.timecount[0] = 0;              //���ü���������
            PRO_dev.DZ |= 1 << POS_D;
            CURE_Set_Clear( DC24V36V_SETTED );      //���CUP�Ѿ������ù�
        }
        else  //û�д�������״̬
        {
            if(FLAG.dc24v36v == 0)                  //ԭ���رգ��Ϳ���
            {PRO_dev.DZ |= 1 << POS_D;FLAG.dc24v36v = 1;}
            else{PRO_dev.DZ &= ~(1 << POS_D);FLAG.dc24v36v = 0;  DC24V36V.timecount[1] = 0;}
        }
    }  
    else
    {
        if(FLAG.set == 1 && SET.priority == 1) //24V 36Vʱ������ֹͣ
        {
            DC24V36V.setted = 0;//����ֹͣ
            PRO_dev.DZ &= ~(1 << POS_D);
        }
        else if(FLAG.dc24v36v == 1){PRO_dev.DZ |= 1 << POS_D;FLAG.dc24v36v = 1;}
        else if(FLAG.dc24v36v == 0){PRO_dev.DZ &= ~(1 << POS_D);}
    }    
}
static void CURE_Key_E(void)        //24
{

}
static void CURE_Key_LOCK_GAS(void) //25,��Դ����
{
    if(PRO_dev.JZ & (1 << POS_LOCK_GAS)){
        switch(FLAG.rec_board)
        {
            case 1:FLAG.lock_gas1 = 1;break;//
            case 2:FLAG.lock_gas2 = 1;break; 
            case 3:break;//����3�޶���
        }
    }
    else{
        switch(FLAG.rec_board)
        {
            case 1:FLAG.lock_gas1 = 0;break;
            case 2:FLAG.lock_gas2 = 0;break;   
            case 3:break;//����3�޻�������
        }
    }
    if(FLAG.lock_gas1 == 1 || FLAG.lock_gas2 == 1) FLAG.lock_gas = 1;
    else FLAG.lock_gas = 0;
}
static void CURE_Key_LOCK_CC(void)  //26�����λ���
{
    if(PRO_dev.JZ & (1 << POS_LOCK_CC)){
        switch(FLAG.rec_board){
            case 1:FLAG.lock_cc1 = 1;break;
            case 2:FLAG.lock_cc2 = 1;break;            
        }
    }
    else{
        switch(FLAG.rec_board){
            case 1:FLAG.lock_cc1 = 0;break;
            case 2:FLAG.lock_cc2 = 0;break;            
        }
    }
    if(FLAG.lock_cc1 == 1 || FLAG.lock_cc2 == 1) FLAG.lock_cc = 1;
    else FLAG.lock_cc = 0;
}

/**********************************************************************/
void CURE_LOCK_Detect(void)
{
    u8 temp = 0;
    switch(FLAG.lock_gas)
    {
        case 0:
            if(FLAG.lock_cc == 0)temp = 0;      //û�л���
            else if(FLAG.lock_cc == 1)temp = 1; //���λ���
        break;
        case 1:                                 //��Դ����
            LOCK.gased = 1;
            if(FLAG.lock_cc == 0)temp = 2;
            else if(FLAG.lock_cc == 1)temp = 3;        
        break;
    }
    if(LOCK.unlock == 1){                       //�ѽ⿪��
        switch(temp)
        {
            case 0:LOCK.lock = 0;break;
            case 1:LOCK.lock = 1;break;
            case 2:LOCK.lock = 0;break;         //ֻ��Դ�������ѽ�
            case 3:LOCK.lock = 1;break;         //������������������Դ�����ѽ⣬ֻʣ�»��λ���
        }
    }
    else if(LOCK.unlock == 0){                  //δ�⿪��
        switch(temp)
        {
            case 0:LOCK.lock = 0;if(LOCK.gased) LOCK.unlock = 1;break;
            case 1:LOCK.lock = 1;if(LOCK.gased) LOCK.unlock = 1;break;
            case 2:LOCK.lock = 2;break;
            case 3:LOCK.lock = 2;break;
        }
    }
}
void CURE_LOCK_Deal(void)
{
    if(LOCK.start == 2)//�Ѽ���10S,����
    {
        BEEP_dev.start(1,150,300);
        LOCK.start = 0;
        LOCK.unlock = 1;
        CURE_LOCK_Detect();
    }
}
void CURE_LOCK_Mix(u32 *temp)
{
    *temp &= ~(3 << POS_LOCK_GAS);
    if(FLAG.lock_gas && LOCK.unlock == 0)*temp |= 1 << POS_LOCK_GAS;
    if(FLAG.lock_cc)*temp |= 1 << POS_LOCK_CC;
}
    //�жϸ��ϵ�ʱ��Դ�Ƿ��ڻ���״̬
void CURE_LOCK_First_Detect(void)
{
    if(LOCK.effective < 4){
        LOCK.effective ++;
        if(FLAG.lock_gas) LOCK.effective_count ++;
        if(LOCK.effective >= 4){
            if(LOCK.effective_count >= 2)LOCK.unlock = 0;
            else LOCK.unlock = 1;
            CURE_LOCK_Detect();
            CURE_LOCK_Mix(&CHAIR.JZ);
        }
    }  
}
//��Ҫ�Ƕ����Ӱ巢�͹����ĵ�ֵ���д���
void LIGHT_Deal(void)
{
    /*****************************************/
    //�����ε������ƻ��岻��ص�λ
    PRO_dev.DZ &= ~(0X1F<<0);//bit4~bit0
    PRO_dev.DZ &= ~(0X1F<<10);
    PRO_dev.DZ &= ~(0X1FF<<16);
    PRO_dev.DZ &= 0X0FFFFFFF;
    /*****************************************/
    
    /*****************************************/
    //���ε������Ӱ岻��ص�λ
    CHAIR.DZ &= ~(0X1F<<5);
    CHAIR.DZ &= ~(1<<15);
    /*****************************************/
    if((CHAIR.DZ & 0XF) && LOCK.lock == 0)
        BEEP_dev.start(1,150,300); 
    PRO_dev.DZ |= CHAIR.DZ;//�����������Ӱ�ĵ�ֵ
}
/**********************************************************************/
// #define CUP_SETTED          1
// #define WATER_SETTED        2
// #define LIGHT_SETTED        3
// #define S1_SETTED           4
// #define S2_SETTED           5
// #define S3_SETTED           6
// #define S4_SETTED           7
// #define S5_SETTED           8
// #define UP_SETTED           9
// #define DN_SETTED           10
// #define BU_SETTED           11
// #define BD_SETTED           12
// //_______________________________________
// #define PRG_SETTED          13
// #define LP_SETTED           14
// #define SP_SETTED           15
// #define A_SETTED            16
// #define B_SETTED            17
// #define C_SETTED            18
// #define D_SETTED            19
// #define E_SETTED            20
//���SET����Ӧ��ֵ��exceptΪ���������
//������0==ȫ�����������
void CURE_Set_Clear(u8 except)
{
    FLAG.cupsetted = 0;
    FLAG.watersetted = 0;
    FLAG.s1setted = 0;
    FLAG.s2setted = 0;
    FLAG.s3setted = 0;
    FLAG.s4setted = 0;
    FLAG.s5setted = 0;  
    FLAG.dc24v36vsetted = 0;
    if(except){
        switch(except)
        {
            case 1:FLAG.cupsetted = 1;break;
            case 2:FLAG.watersetted = 1;break;
            case 4:FLAG.s1setted = 1;break;
            case 5:FLAG.s2setted = 1;break;
            case 6:FLAG.s3setted = 1;break;
            case 7:FLAG.s4setted = 1;break;
            case 8:FLAG.s5setted = 1;break;
            case 9:FLAG.dc24v36vsetted = 1;break;
        }
    }
}
//�������Ƿ����
//HEATED.light_interval��0��������˸״̬��1������˸״̬������2������˸״̬����
void CURE_Heat_Detect(void)
{
    if(FLAG.heat == 1 ){     //�ѿ��Ƽ���
        if(HEATED.heated){HEATED.control = 0;HEATED.light_interval = 1; HEATED.count = 0; }//���������
        else HEATED.control = 1;             //���ڼ�����              
    }
    else{
        HEATED.control = 0;HEATED.delaycount = 0;HEATED.delayfinish = 0;
        HEATED.light_interval = 0; HEATED.count = 0;
    }
    /********************************************/
    
    /********************************************/
    if(HEATED.control)  //���ƵƵ�����,��ʼ��ʱ
    {
        switch(HEATED.count)
        {
            case 0://������ʱ������
                HEATED.delaycount = 0;
                HEATED.delayfinish = 0;
                HEATED.light_interval = 1;//����
                HEATED.count ++;
            break;
            case 1://�ȴ�������ʱ����
                if(HEATED.delayfinish)
                {
                    HEATED.delaycount = 0;
                    HEATED.delayfinish = 0;
                    HEATED.light_interval = 0;//����
                    HEATED.count ++; 
                }
            break;
            case 2://�ȴ�������ʱ����
                if(HEATED.delayfinish) HEATED.count = 0; 
            break;
        }
    }
    /********************************************/
    
    /********************************************/
    if(HEATED.light_interval) PRO_dev.DZ |= 1 << 7;
    else PRO_dev.DZ &= ~(1 << 7);
    /********************************************/    
}
//���ڶ�WATER��������ʱ������ʶ��
void CURE_WATER_Scan(void)
{
    if(WATER.start)
    {
        switch(WATER.setttimes)
        {
            case 1:if(WATER.work){BEEP_dev.start(1,50,300);WATER.work = 0;}break;
            case 2:if(WATER.work){BEEP_dev.start(2,50,100);WATER.work = 0;}break;
            case 3:if(WATER.work){BEEP_dev.start(3,150,300);WATER.work = 0;}break;
        }
    }
}
void CURE_WATER_start(void)
{
    FLAG.water = 1;
    WATER.watercount[1] = 0;
    PRO_dev.DZ |= 1 << POS_WATER;
}
//WATER���¿�ʼ��ʱ�������ԭ���ļ�ʱ�����㿪ʼ��ʱ
void CURE_WATER_ReCount(void)
{
    WATER.watercount[1] = 0;
}
void CURE_WATER_Close(void)
{
    FLAG.water = 0;
    PRO_dev.DZ &= ~(1 << POS_WATER);
}
//�˳�����״̬������
//PositionΪRESET �� SPITE(SP)
void CURE_ExitSet_SaveNone(u8 Position)
{
    if(SET.priority)
    {
        if(SET.priority == 1)//����ͨ��������£�����RESET��POS_SP��POS_LP�˳�����
        {
#ifndef	VERSION_JAPAN		
            if(Position == POS_RESET || Position == POS_SP || Position == POS_LP )
#else 
			if(Position == POS_RESET || Position == POS_SP || Position == POS_LP || Position == POS_POWER)
#endif	
            {
                /*****************************************************************************/
                CHAIR.JZ &= ~(1 << Position);   //����������״̬ʱ���Ͳ���Ҫ�ٷ��ͼ�ֵ�����Ӱ壬���������ƻ������ض��Ĵ���
                PRO_dev.DZ &= ~(1 << POS_SET);  //�ر�SET�ĵ�ֵ
                /*******************************************************************************/
                //���ƻ��ϵ�WATER,CUP����
                FLAG.set=0;
                TIME_dev.cupcount[0]=0;
                TIME_dev.cupcount[1]=0;
                FLAG.cupsetted=0;
                //WATER
                switch(WATER.type)
                {
                    case 0:
                        WATER.watercount[0]=0;WATER.watercount[1]=0;FLAG.watersetted=0;
                        WATER.watersettimes=0;FLAG.waterset=0;            
                    break;
                    case 1:
                        WATER.min = 0;WATER.sec = 0;WATER.setttimes = 0;WATER.work = 0;
                        WATER.watercount[0]=0;WATER.watercount[1]=0;FLAG.watersetted=0;
                        WATER.watersettimes=0;FLAG.waterset=0; 
                    break;       
                }
                WATER.start = 0;WATER.unlimit = 0;
                //LIGHT
                BEEP_dev.beeptimescounter=0;
                /***********************************************************************************/
            }
        }
        else if(SET.priority == 2)
        {
#ifndef	VERSION_JAPAN	
            if(Position == POS_RESET || Position == POS_SP || Position == POS_LP 
                || Position == POS_S1 || Position == POS_S2 || Position == POS_S3 
                || Position == POS_S4 || Position == POS_S5)
#else 
            if(Position == POS_RESET || Position == POS_SP || Position == POS_LP || Position == POS_POWER 
                || Position == POS_S1 || Position == POS_S2 || Position == POS_S3 
                || Position == POS_S4 || Position == POS_S5)
#endif			
            {
                CHAIR.JZ &= ~(1 << Position);
                PRO_dev.DZ &= ~(1 << POS_SET);
            }
            else if(Position == POS_LOCK_CC)
            {
                PRO_dev.DZ &= ~(1 << POS_SET);
            }
        }
        else if(SET.priority == 3)
        {
#ifndef	VERSION_JAPAN
            if(Position == POS_RESET || Position == POS_SP || Position == POS_LP 
                || Position == POS_S1 || Position == POS_S2 || Position == POS_S3 || Position == POS_S4 || Position == POS_S5
                || Position == POS_UP || Position == POS_DN || Position == POS_BU || Position == POS_BD
                || Position == POS_PRG || Position == POS_CUP || Position == POS_WATER || Position == POS_HEAT
                || Position == POS_WATCH || Position == POS_LIGHT || Position == POS_A || Position == POS_B
                || Position == POS_C || Position == POS_D || Position == POS_POWER)
#else
            if(Position == POS_RESET || Position == POS_SP || Position == POS_LP || Position == POS_POWER
                || Position == POS_S1 || Position == POS_S2 || Position == POS_S3 || Position == POS_S4 || Position == POS_S5
                || Position == POS_UP || Position == POS_DN || Position == POS_BU || Position == POS_BD
                || Position == POS_PRG || Position == POS_CUP || Position == POS_WATER || Position == POS_HEAT
                || Position == POS_WATCH || Position == POS_LIGHT || Position == POS_A || Position == POS_B
                || Position == POS_C || Position == POS_D || Position == POS_POWER)			
#endif
            {
                CHAIR.JZ &= ~(1 << Position);
                PRO_dev.DZ &= ~(1 << POS_SET);
            }
            else if(Position == POS_LOCK_CC)
            {
                PRO_dev.DZ &= ~(1 << POS_SET);
            }
        }
        /***********************************************************************/
        //��Ҫ���͵����ƻ�����Ϣ
        BEEP_dev.start(1,150,300);
        SET.start = 0;SET.chairprio = 0;SET.count[0] = 0;SET.count[1] = 0;
        SET.fail = 0;SET.manual = 0;SET.prio_pre = 0;SET.stage_in = 0;
        PRO_dev.DZ &= ~(1 << POS_SET);
        FLAG.set = 0;
        SET.priority = 0;
        SET.stage_out = 6;//�������˳�
        /***********************************************************************/
    }
}
//����Ƿ���յ����ݣ���ִ��
void CURE_Work_Run(void)
{
    if(FLAG.rec_board)
    {
        switch(FLAG.rec_board)
        {
            case 1:KEY_deal();CURE_LOCK_Mix(&CHAIR.JZ);break;//���̰�1
            case 2:KEY_deal();CURE_LOCK_Mix(&CHAIR.JZ);break;//���̰�2
            case 3:KEY_deal();//CURE_LOCK_Mix();
            break;
            case 4:LIGHT_Deal();break;//���Ӱ壬������Ӱ巢�͹����ĵ�ֵ���д���            
        }
        FLAG.rec_board = 0;//������ձ�־
    }
}

u8 CURE_Communication_Recieved(void)
{
    u32 temp = 0; u16 len= 0,i = 0;
    u8 settemp = 0,ret = 0;
    if( USART_RX_STA & 0x8000 )
    {
        len=USART_RX_STA & 0x3FFF;
        for(i=0;i<len;i++)
            PRO_dev.RS485REV[i] =  USART_RX_BUF[i];
        USART_RX_STA = 0;
        if((PRO_dev.RS485REV[0] == 0xAA) && (PRO_dev.RS485REV[1] == 0x0F))
        {
            if(PRO_dev.RS485REV[8] == (PRO_dev.RS485REV[3] ^ PRO_dev.RS485REV[4] ^ PRO_dev.RS485REV[5] ^ PRO_dev.RS485REV[6] ^ PRO_dev.RS485REV[7])
               && (PRO_dev.RS485REV[2] == 0x01 || PRO_dev.RS485REV[2] == 0x02 || PRO_dev.RS485REV[2] == 0x03 || PRO_dev.RS485REV[2] == 0x04)
            )
            {  
                len=USART_RX_STA & 0x3FFF;
                temp=0;
                temp|=PRO_dev.RS485REV[4] & 0XFF;
                temp|=(PRO_dev.RS485REV[5] & 0XFF)<<8;
                temp|=(PRO_dev.RS485REV[6] & 0XFF)<<16;
                temp|=(PRO_dev.RS485REV[7] & 0XFF)<<24;
                FLAG.rec_board = PRO_dev.RS485REV[2];
                PRO_dev.addr_err &= ~(1 << (FLAG.rec_board - 1));//��������־
                switch(FLAG.rec_board)
                {
                    case 1://�������̷��͹����ļ���ֵ
                        if(CURE_KeyNum_Detect(KEYBOARD_1,&KEYBOARD,temp))
                        {
                            if(CURE_KeyBoard_ValueDetect(KEYBOARD_1,&KEYBOARD))
                            {
                                if(PRO_dev.JZ != temp)//����һ�ε�ֵ���� 
                                {
                                    if(CURE_CheckSelf_Quit_Key(&CHECKSELF,temp) == 0){
                                        if(S1TOS5.state == FIGURE_FINISH || S1TOS5.state == FIGURE_CLEAR){
                                            CURE_AutoRunStop_PressSet(&temp);
                                            PRO_dev.JZ = temp;CHAIR.JZ = PRO_dev.JZ;
                                        }
                                    }
                                    else FLAG.rec_board = 0;
                                }  
                                else ret = 1;
                            }
                            else ret = 1;
                        }
                        else ret = 1;
                    break;
                    case 2://�������̷��͹����ļ���ֵ
                        if(CURE_KeyNum_Detect(KEYBOARD_2,&KEYBOARD,temp))
                        {
                            if(CURE_KeyBoard_ValueDetect(KEYBOARD_2,&KEYBOARD))
                            {
                                if(PRO_dev.JZ != temp)//����һ�ε�ֵ���� 
                                {
                                    if(CURE_CheckSelf_Quit_Key(&CHECKSELF,temp) == 0){
                                        if(S1TOS5.state == FIGURE_FINISH || S1TOS5.state == FIGURE_CLEAR){
                                            CURE_AutoRunStop_PressSet(&temp);
                                            PRO_dev.JZ = temp;CHAIR.JZ = PRO_dev.JZ;
                                            CURE_LOCK_Mix(&CHAIR.JZ);//�Լ���1��2��LOCK��ֵ�����ۺ�
                                        }
                                    }
                                    else FLAG.rec_board = 0;
                                } 
                                else ret = 1;
                            }
                            else ret = 1;
                        }
                        else ret = 1;
                    break;
                    case 3:
                        if(CURE_KeyNum_Detect(KEYBOARD_3,&KEYBOARD,temp))
                        {
                            if(CURE_KeyBoard_ValueDetect(KEYBOARD_3,&KEYBOARD))
                            {
                                if(PRO_dev.JZ != temp)//����һ�ε�ֵ���� 
                                {
                                    if(CURE_CheckSelf_Quit_Key(&CHECKSELF,temp) == 0){
                                        if(S1TOS5.state == FIGURE_FINISH || S1TOS5.state == FIGURE_CLEAR){
                                            CURE_AutoRunStop_PressSet(&temp);
                                            CURE_LOCK_Mix(&temp);
                                            PRO_dev.JZ = temp;CHAIR.JZ = PRO_dev.JZ;
                                        }
                                    }
                                    else FLAG.rec_board = 0;
                                } 
                                else ret = 1;
                            }
                            else ret = 1;
                        }
                        else ret = 1;                        
                    break;
                    
                    case 4://���ӿ��ư巢�͹����ĵ�ֵ                        
                        if(CHAIR.DZ != temp)
                            CHAIR.DZ = temp;
                        settemp = (PRO_dev.RS485REV[3] >> 3) & 0X1F;//����λ
                        /*******************************************/
                        //SET�Ĺ���
                        if(settemp <= 7)
                            CURE_SET_InOut_ACK(settemp); //���յ�������
                        /*******************************************/
                        //SPITE����,8,9,10,11
                        else if(settemp >= 8 && settemp <= 12){
                            CURE_SPITE_Set(settemp);
                        }
                        /*******************************************/
                        //�Լ칦��,13,14,15,16
                        else if(settemp >= 13 && settemp <= 16){
                            CURE_CheckSelf_CmdSet(settemp);//��ȡ�����Ӱ巢�͹�����Ӧ���źŻ�����
                        }
                        /*******************************************/
                        //���������
                        CURE_Sensor_Detect(&SENSOR,&CHAIR.DZ);
                        //S1TOS5������̼��
                        CURE_S1TOS5_Figure_Detect(&CHAIR.DZ);
                        /*******************************************/
                    break;                        
                }
                if(ret){FLAG.rec_board = 0;}
                return 1;//������ȷ���ɹ�
            }
            else PRO_dev.addr_err |= 1 << (PRO_dev.addr_count-1);    //��¼��������ĵ�ַ
            return 2;
        }
        else PRO_dev.addr_err |= 1 << (PRO_dev.addr_count-1);        //��¼��������ĵ�ַ
        return 2;
    }
    return 0;
}
void CURE_Communication_Send(u8 addr)
{
    u32 temp = 0;
    switch(addr)
    {
        case 1:temp = PRO_dev.DZ;CURE_CheckSelf_LightControl(&CHECKSELF,&temp);CURE_Sensor_Send(&SENSOR,&temp);break;//���͵����̵��ǵ�ֵ
        case 2:temp = PRO_dev.DZ;CURE_CheckSelf_LightControl(&CHECKSELF,&temp);CURE_Sensor_Send(&SENSOR,&temp);break;//���͵����̵��ǵ�ֵ
        case 3:temp = PRO_dev.DZ;CURE_CheckSelf_LightControl(&CHECKSELF,&temp);CURE_Sensor_Send(&SENSOR,&temp);break;//���͵����̵��ǵ�ֵ
        case 4://���͵����Ӱ���Ǽ�ֵ
            temp = CHAIR.JZ;
            if(SENSOR.sensor)//����������ʱ����S1-S5 SP POWER ABCDE
            {
                temp &= ~(0X1F << POS_S1);//S1~S5
				/**********************************************************/
#ifdef VERSION_JAPAN
				temp &= ~(0X1 << POS_SP);//SP
                temp &= ~(0X7 << POS_A);//ABC
				/**********************************************************/				
#else
                temp &= ~(0X1F << POS_SP);//SP POWER ABC
				/**********************************************************/				
#endif
				

                temp &= ~(1 << POS_E);//E
            }
            if(LOCK.lock == 1) temp &= ~(0X1F << POS_S1);
        break;
    }
    PRO_dev.RS485SEND[0] = 0XAA;
    PRO_dev.RS485SEND[1] = 0X0F;
    PRO_dev.RS485SEND[2] = addr;
    /***************************���ܺ�����*********************************/
    //���ԭ���ķ������޴��󣬵���λ
    if(PRO_dev.addr_err){
        if(PRO_dev.addr_err & (1 << (addr - 1)))
        {PRO_dev.RS485SEND[3] = 1;PRO_dev.addr_err &= ~(1 << (addr - 1));}
        else PRO_dev.RS485SEND[3] = 0;  
    }else PRO_dev.RS485SEND[3] = 0;
    //����λΪ��SET�йص�����
    if(addr == 4){
        if(SET.stage_in || SET.stage_out) CURE_SET_InOut_Send();
        else if(CHECKSELF.stage_in || CHECKSELF.stage_out) CURE_CheckSelf_Send(&PRO_dev.RS485SEND[3]);
    }
    /************************************************************/
    PRO_dev.RS485SEND[4] = temp & 0xFF;
    PRO_dev.RS485SEND[5] = (temp >> 8) & 0xFF;
    PRO_dev.RS485SEND[6] = (temp >> 16) & 0xFF;
    PRO_dev.RS485SEND[7] = (temp >> 24) & 0xFF;	
    PRO_dev.RS485SEND[8] = PRO_dev.RS485SEND[3] ^ PRO_dev.RS485SEND[4] ^ PRO_dev.RS485SEND[5] ^ PRO_dev.RS485SEND[6] ^ PRO_dev.RS485SEND[7];
    RS485_Send(PRO_dev.RS485SEND,9);//���͵��ǵ�ֵ        
}
u8 CURE_Communication_Rec_Sample(void)
{
    u32 temp = 0; u16 len= 0,i = 0;
    if( USART_RX_STA & 0x8000 )
    {
        len=USART_RX_STA & 0x3FFF;
        for(i=0;i<len;i++)
            PRO_dev.RS485REV[i] =  USART_RX_BUF[i];
        USART_RX_STA = 0;
        if((PRO_dev.RS485REV[0] == 0xAA) && (PRO_dev.RS485REV[1] == 0x0F))
        {
            if(PRO_dev.RS485REV[8] == (PRO_dev.RS485REV[3] ^ PRO_dev.RS485REV[4] ^ PRO_dev.RS485REV[5] ^ PRO_dev.RS485REV[6] ^ PRO_dev.RS485REV[7])
               && (PRO_dev.RS485REV[2] == 0x01 || PRO_dev.RS485REV[2] == 0x02 || PRO_dev.RS485REV[2] == 0x03 || PRO_dev.RS485REV[2] == 0x04) 
            )
            {
                temp|=PRO_dev.RS485REV[4] & 0XFF;
                temp|=(PRO_dev.RS485REV[5] & 0XFF) << 8;
                temp|=(PRO_dev.RS485REV[6] & 0XFF) << 16;
                temp|=(PRO_dev.RS485REV[7] & 0XFF) << 24;
                FLAG.rec_board = PRO_dev.RS485REV[2];
                PRO_dev.JZ = temp;
                CURE_Key_LOCK_GAS();
                CURE_Key_LOCK_CC(); 
                CURE_LOCK_First_Detect();
                return 1;
            }
            else return 2;
        }
        else return 3;
    }return 0;
}
/**********************************************************************/
//�����̷��͹���������1�ĸ����Ƿ�Ϊ1
u8 CURE_KeyNum_Detect(u8 board,Typedef_KEYBOARD *keyboard,u32 jz)
{
    u32 temp = jz;    //JZΪ���β�ѯʱ��õ��ۺ�ֵ��JZ10Ϊ��һ�ε��ۺ�ֵ
    u8 low4bit = 0,templow = 0;
    u8 t = 0,sum = 0,ret = 0,keydown = 0;
    low4bit = temp & 0X0F;
    if(low4bit){
        switch(low4bit){
            case 0X1:case 0X2:case 0X4:case 0X8:templow = 1;break;//һ��
            case 0X5:case 0X6:case 0X9:case 0XA:templow = 2;break;//����      
            default:templow = 3;break;//�����������Ч
        }
    }
    else {low4bit = 0;templow = 0;}
    switch(templow){
        case 0://����λ�޼�����
            temp >>= 4;
            if((temp & 0X001FFFFF) == 0) {ret = 1;keydown = 0;break;}
            for(t = 0;t < 21;t ++)   //ͳ����ĸ���
            {if(temp & 0x01) sum ++;temp >>= 1;}		
            if(sum != 1) {ret = 0;keydown = 1;break;}           //���ܵ���������ĸ�����Ϊ1����Чֱ�ӷ���
            ret = 1;keydown = 1;
        break;
        case 1:case 2://����λ1������
            temp >>= 4;
            if((temp & 0X001FFFFF) == 0) {ret = 1;keydown = 1;break;}
            else {ret = 0;keydown = 1;}
        break;
//         case 2://����λ2������
//             
//         break;
        case 3:ret = 0;keydown = 1;break;//����λ�������
    }
    switch(board){
        case KEYBOARD_1:keyboard->KEYBOARD1.keydown = keydown; break;
        case KEYBOARD_2:keyboard->KEYBOARD2.keydown = keydown; break;
        case KEYBOARD_3:keyboard->KEYBOARD3.keydown = keydown; break;
    }
    return ret;
}
//2016-3-30:���̰�֮��ļ�ֵ�Ƚ�
u8 CURE_KeyBoard_ValueDetect(u8 board,Typedef_KEYBOARD *keyboard)
{
    u8 ret = 1;
    if(board){
        switch(board){
            case KEYBOARD_1:if(keyboard->KEYBOARD2.keydown || keyboard->KEYBOARD3.keydown) ret = 0;else ret = 1;break;
            case KEYBOARD_2:if(keyboard->KEYBOARD1.keydown || keyboard->KEYBOARD3.keydown) ret = 0;else ret = 1;break;
            case KEYBOARD_3:if(keyboard->KEYBOARD1.keydown || keyboard->KEYBOARD2.keydown) ret = 0;else ret = 1;break;
        }}
    return ret;
}
/**********************************************************************/
//LIGHT 1��������
void CURE_1RANK_Deal(u8 keyupdn)
{
    switch(keyupdn)
    {
        case KEY_UP://������
            if(FLAG.light == 1)//һ���LIGHT
            {PRO_dev.DZ |= 1 << POS_LIGHT;FLAG.light = 1;}
            else if(FLAG.light==0)
                PRO_dev.DZ &= ~(1 << POS_LIGHT);        
        break;
        case KEY_DOWN://������
            BEEP_dev.start(1,150,300);
            if(FLAG.light == 0)         //ԭ���رգ��Ϳ���
            {PRO_dev.DZ |= 1 << POS_LIGHT;FLAG.light = 1;} 
            else if(FLAG.light == 1)   //ԭ���������͹رգ���ʱֹͣ
            {PRO_dev.DZ &= ~(1 << POS_LIGHT);FLAG.light = 0;}               
        break;
    }
}
//LIGHT 3��������
void CURE_3RANK_Deal(TypeDef_CURELIGHT *light,u8 keyupdn)
{
    switch(keyupdn)
    {
        case KEY_UP://������
            if(light->keylightflag == 1){//�ж��Ƿ��ڼ�����״̬
                light->keylightflag = 0;
                light->timecounter = 0;//ֹͣ����
                light->rank ++;
                if(light->rank > 3)
                    light->rank = 1;
                /*___________________________________*/
                //�̵�����λ�л�ʱ��ʱ0.5S:2016-3-17
                light->RANK3_SWITCHDELAY.flag = 1;
                light->RANK3_SWITCHDELAY.step = 2;
                /*___________________________________*/
            }
            else if(light->keylightflag == 0){//û�д��ڼ�����״̬
                if(FLAG.light == 1)//һ���LIGHT
                {PRO_dev.DZ |= 1 << POS_LIGHT;FLAG.light = 1;}
                else if(FLAG.light==0)
                    PRO_dev.DZ &= ~(1 << POS_LIGHT); 
            }
        break;
        
        case KEY_DOWN://������
            BEEP_dev.start(1,150,300);
            if(FLAG.light == 0)//ԭ��δ����,����
            {PRO_dev.DZ |= 1 << POS_LIGHT;FLAG.light = 1;light->ranklast = light->rank;light->RANK3_SWITCHDELAY.step = 1;}
            else if(FLAG.light == 1)//ԭ���Ѿ�����,�˳������������ڵ�ֵ��Ϊ�´�����������
                light->keylightflag = 1;
        break;
    }
}
void CURE_3RANK_ExitDetect(TypeDef_CURELIGHT *light)
{
    if(light->stop){
        if(light->ranklast != light->rank)//�˴ε�ֵ����һ�ε�ֵ����ͬ�ű��棬���򲻱���
            DATAFLASH.save(OFFSET_LIGHT,light->rank,DF_PAGE1);//����˴ε�λ��ֵ
        CURE_3RANK_Clear(&CURELIGHT);
        FLAG.light = 0;//�ƹر�
        PRO_dev.DZ &= ~(1 << POS_LIGHT);}
}
void CURE_3RANK_Clear(TypeDef_CURELIGHT *light)
{
    light->keylightflag = 0;light->temp = 0;light->timecounter = 0;light->ranklast = 0;
    light->stop = 0;light->RANK3_SWITCHDELAY.counter = 0;light->RANK3_SWITCHDELAY.flag = 0;
}
/**********************************************************************/


