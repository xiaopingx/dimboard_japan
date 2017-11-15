#include "withmemory.h"
#include "hardware.h"
#include "mini51eeprom.h"
#include "timer.h"
#include "curetype.h"
#include "steplessdimming.h"
typedef struct
{
    u8 doctor;
    
}TypeDef_PRG;
extern TypeDef_PRG PRG;

TypedDef_SPITE SPITE =
{
    0,//cmd
    0,//flag
    0,//original_waterstate
    0,//original_lightstate
    0,//work
};

TypeDef_CHECKSELF CHECKSELF = 
{
    0,//cmd
    0,//cmdpre
    0,//flag
    0,0,//counter
    0,//times
    0,//stage_in
    0,//stage_out
    0,//mode
    0,//light
    0,//lightcouter
    
    
};
TypeDef_SENSOR SENSOR = 
{
    0,//sensor
    0,
    0,//light
    
};
TypedDef_S1TOS5 S1TOS5 =
{
    0,//state
    
};
/*******************************SET��غ���****************************************/
//SET���ȼ���⺯��
u8 CURE_SET_Pripre_detect(u32 times)
{
    if(times < SET_PRIORITY2){
        if(SET.stage_in == 0)SET.stage_in = 1;return 1;
    }
        
    else if(times >= SET_PRIORITY2 && times < SET_PRIORITY3){
        if(SET.stage_in == 3)SET.stage_in = 4;return 2; 
    }
    else if(times >= SET_PRIORITY3){
        if(SET.stage_in == 6)SET.stage_in = 7;return 3;  
    }  
    return 0;
}
//SETȷ�Ϻ���
void CURE_SET_Sure(u8 prio)
{
    if(prio)
    {
        switch(prio)
        {
            case 1:
                if(FLAG.cupsetted)
                {
                    if(TIME_dev.cupcount[0] < 1000)
                        TIME_dev.cupcount[0] = 1000;                
                    DATAFLASH.save(OFFSET_CUP,TIME_dev.cupcount[0],DF_PAGE1);
                    TIMEPARAM.cup = TIME_dev.cupcount[0];
                    TIME_dev.cupcount[0] = 0;
                    TIME_dev.cupcount[1] = 0;
                    FLAG.cupsetted = 0;
                    //ͬʱҪ�������ĵ����������LIGHT��WATER
                    //WATER
                    CURE_Water_Clear();
                    //DC24V36V
                    CURE_DC24V36V_Clear();
                }
                else if(FLAG.watersetted)
                {
                    switch(WATER.type)
                    {
                        case 0:
                            DATAFLASH.save(OFFSET_WATER,WATER.watercount[0],DF_PAGE1);
                            TIMEPARAM.water = WATER.watercount[0];
                            WATER.watercount[0] = 0; WATER.watercount[1] = 0;
                            FLAG.watersetted = 0; WATER.watersettimes = 0;                        
                        break;
                        case 1:
                            if(WATER.setttimes)
                            {
                                switch(WATER.setttimes)
                                {
                                    case 1://ֻ������sec
                                        TIMEPARAM.water = (WATER.sec / 1000) * 5000;
                                        WATER.unlimit = 0;
                                    break;
                                    case 2://������min
                                        TIMEPARAM.water = ((WATER.sec / 1000) * 5000) + ((WATER.min / 1000) * 60000);
                                        WATER.unlimit = 0;
                                    break;
                                    case 3://ʱ��Ϊ���޳�
                                        TIMEPARAM.water = 0XFFFFAABB;
                                        WATER.unlimit = 1;
                                    break;
                                }
                            }
                            DATAFLASH.save(OFFSET_WATER,TIMEPARAM.water,DF_PAGE1);
                            FLAG.watersetted = 0;
                            WATER.sec = 0;
                            WATER.min = 0;
                            WATER.watercount[0] = 0;
                            WATER.watercount[1] = 0;
                            WATER.setttimes = 0;
                        break;
                    }
                    //CUP
                    CURE_Cup_Clear();
                    //DC24V36V
                    CURE_DC24V36V_Clear();
                }
                else if(FLAG.dc24v36vsetted)
                {
                    if(DC24V36V.timecount[0] < 1000)
                        DC24V36V.timecount[0] = 1000;                
                    DATAFLASH.save(OFFSET_DC24V36V,DC24V36V.timecount[0],DF_PAGE1);
                    DC24V36V.time = DC24V36V.timecount[0];
                    DC24V36V.timecount[0] = 0;
                    DC24V36V.timecount[1] = 0;
                    FLAG.dc24v36vsetted = 0;                    
                    //CUP
                    CURE_Cup_Clear();
                    //WATER
                    CURE_Water_Clear();
                }
                else if(FLAG.s1setted || FLAG.s2setted || FLAG.s3setted || FLAG.s4setted ||FLAG.s5setted)
                {
                    //CUP
                    CURE_Cup_Clear();
                    //WATER
                    CURE_Water_Clear();
                    //DC24V36V
                    CURE_DC24V36V_Clear();
                    //���S1~S5
                    FLAG.s1setted = 0;FLAG.s2setted = 0;FLAG.s3setted = 0;FLAG.s4setted = 0;FLAG.s5setted = 0;
                }
            break;
            case 2:
                
            break;
            case 3:
                
            break;
        }

    }
}
void CURE_Cup_Clear(void)
{
    TIME_dev.cupcount[0] = 0;
    TIME_dev.cupcount[1] = 0;
    FLAG.cupsetted = 0;
}
void CURE_Cup_Stop(void)
{
    CURE_Cup_Clear();
    FLAG.cup = 0;
    FLAG.cupset = 0;
}
void CURE_Water_Clear(void)
{
    switch(WATER.type)
    {
        case 0:
            WATER.watercount[0] = 0;WATER.watercount[1] = 0;
            FLAG.watersetted = 0;WATER.watersettimes = 0;
        break;
        case 1:
            FLAG.watersetted = 0;WATER.sec = 0;WATER.min = 0;
            WATER.watercount[0] = 0;WATER.watercount[1] = 0;WATER.setttimes = 0;
        break;
    }
}
void CURE_Water_Stop(void)
{
    CURE_Water_Clear();
    FLAG.water = 0;
}
void CURE_DC24V36V_Clear(void)
{
    FLAG.dc24v36vsetted = 0;
    DC24V36V.setted = 0;
    DC24V36V.timecount[0] = 0;
    DC24V36V.timecount[0] = 1;
}
void CURE_DC24V36V_Stop(void)
{
    CURE_DC24V36V_Clear();
    FLAG.dc24v36v = 0;
}
void CURE_SetEnter_Peripheral_Stop(void)
{
    CURE_Cup_Stop();
    CURE_Water_Stop();
    CURE_DC24V36V_Stop();
}
/*SET.stage_in:
2����ͨ�����ѷ���
5���ֶ������ѷ���
8���Զ������ѷ���
SET.stage_out��
2���˳������ѷ���*/
void CURE_SET_priority_Set(u8 prio)
{
    if( SET.stage_in == 2 || SET.stage_in == 5 || SET.stage_in == 8
        || SET.stage_out == 2 || SET.stage_out == 7 || prio == 7
      )
    {    
        switch(prio)
        {
            /****************************����ģʽ***************************/
            case 1://��ͨ����
                SET.priority = 1;
                SET.chairprio = 1;//���Ӱ崦����ͨ����״̬
                BEEP_dev.start(1,150,300);   
            break;
            case 2://���ȼ�2���ֶ�����
                SET.priority = 2;
                SET.chairprio = 2;
                BEEP_dev.start(1,500,300);
            break;
            case 3://���ȼ�3���Զ�����
                SET.priority = 3;
                SET.chairprio = 3;
                BEEP_dev.start(1,1000,300);
            break;
            case 4://���ȼ�4������������õĹ����������˳�
                BEEP_dev.start(1,150,300);
//                CURE_SET_Sure(SET.priority);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//������
            break;
            case 5:
                BEEP_dev.start(1,150,300);
                SET.priority = 2;
            break;
            case 6://���ȼ�6���������˳�
                BEEP_dev.start(1,150,300);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//������
            break;
            case 7://�����Ӱ��������͹�����ֵ����ʾ���Ӱ���Զ������Ѿ����
                BEEP_dev.start(1,150,300);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//������
            break;
            /*********************************************************************/
        }
    }
}
void CURE_SET_Fail_Detect(void)
{
    if(SET.fail)
    {SET.priority = 1;SET.fail = 0;}
}
void CURE_SET_ClearAll(TypeDef_SET *set)
{
    set->chairprio = 0;
    set->count[0] = 0; set->count[1] = 0;
    set->fail = 0;
    set->manual = 0;
    set->priority = 0;
    set->prio_pre = 0;
    set->stage_in = 0;
    set->stage_out = 0;
    set->start = 0;
    
    PRO_dev.JZ &= ~(1 << POS_SET);
}

//������˳��ķ��ͺ���
/*
���ܺţ�
1����ͨ����
2���ֶ���������
3���Զ���������
4��ȷ���˳�������
6��ȷ���˳���������
*/
void CURE_SET_InOut_Send(void)
{
    if(SET.stage_in == 1 || SET.stage_in == 4 || SET.stage_in == 7 || SET.stage_in == 13)//����
    {                   
        switch(SET.prio_pre){
            case 1://��ͨ����
                PRO_dev.RS485SEND[3] |= (1 << 3);SET.stage_in = 2;break;
            case 2://�ֶ�����
                PRO_dev.RS485SEND[3] |= (2 << 3);SET.stage_in = 5;break;
            case 3://�Զ�����
                PRO_dev.RS485SEND[3] |= (3 << 3);SET.stage_in = 8;break;
        }
        if(SET.stage_in == 1 && SET.manual == 1)//�ֶ����õĵ�һ�ΰ���
        {PRO_dev.RS485SEND[3] |= (5 << 3);SET.stage_in = 2;}
    }
    else if(SET.stage_out == 1 || SET.stage_out == 6)//�˳�:1,�����˳���6���������˳�
    {
        if(SET.stage_out == 1)//1,�����˳���
        {
            PRO_dev.RS485SEND[3] |= (4 << 3);//������ɹ���
            SET.stage_out = 2;
        }
        else if(SET.stage_out == 6)//6���������˳�
        {PRO_dev.RS485SEND[3] |= (6 << 3);SET.stage_out = 7;}
    }
}
//��������Ӱ��з����źŵ���Ϣ
/*
    SET.stage_in:
    2����ͨ�����ѷ���
    5���ֶ������ѷ���
    8���Զ������ѷ���
    SET.stage_out��
    2���˳������ѷ���
*/
void CURE_SET_InOut_ACK(u8 temp)
{
    if(temp)
    {
        if(SET.stage_in == 2 || SET.stage_in == 5 || SET.stage_in == 8|| SET.stage_out == 2 || SET.stage_out == 7 || temp == 7){
            if(temp){
                CURE_SET_priority_Set( temp );
                switch(SET.stage_in){
                    case 2:SET.stage_in = 3;break;
                    case 5:SET.stage_in = 6;break;    
                    case 8:SET.stage_in = 0;break;
                }
            }
        }
    }
}
/***************************************************************************************/
//8://��ʼ��ǰ�˶�
//9://�˶�����ǰ���ޣ���ͻ��ֹͣ
//10://�ѻص���ԭ�㣬��ͻ��ֹͣ
void CURE_SPITE_Set(u8 temp)
{
    if(temp >= 8 && temp <=  12){
        SPITE.cmd = temp;SPITE.flag = 1;
    }
}
//SPITE������
//original_lightstate:��¼Lightԭ����״̬
//original_waterstate:��¼waterԭ����״̬
//flag:ϵͳ����SPITE״̬
//cmd:ָ�8��9��10��11
void CURE_SPITE_Deal(u8 *cmd,u8 *flag)
{
    u32 remain = 0;
    if(*flag)
    {
        switch(*cmd)
        {
            /**********************************************************/
            case SPITE_FUNC_STARTHEAD://��ʼ��ǰ�˶�
                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        if(FLAG.light == 1)  //ԭ���������͹ر�
                        {SPITE.original_lightstate = 1;PRO_dev.DZ &= ~(1 << POS_LIGHT);FLAG.light = 0;} 
                        if(FLAG.water == 1){//ԭ���ѿ���,WATER���¿�ʼ��ʱ
                            if(WATER.unlimit == 0)//ԭ����������״̬����������״̬����Ҫ���¼�ʱ
                                CURE_WATER_ReCount();
                        }
                        else//ԭ��û�п���
                            CURE_WATER_start();                        
                    break;
                    case CUREBOARD_DIM:
                        SPITE.work = 1;//PRO_dev.DZ &= ~(1 << POS_LIGHT);
                        if(STEPLESSDIMMING.flag)
                        {
                            SPITE.original_lightstate = 1;CURE_Dimming_Stop(&STEPLESSDIMMING);
                        } 
                    break;
                }
                if(FLAG.water == 1){//ԭ���ѿ���,WATER���¿�ʼ��ʱ
                    if(WATER.unlimit == 0)//ԭ����������״̬����������״̬����Ҫ���¼�ʱ
                        CURE_WATER_ReCount();
                }
                else//ԭ��û�п���
                    CURE_WATER_start();      
            break;
            /**********************************************************/
            case SPITE_FUNC_HEADFINISH://���Ｋ��
            break;
            /**********************************************************/
            case SPITE_FUNC_STARTBACK://��ʼ�����˶�
                if(WATER.unlimit == 0){//ԭ����������״̬����������״̬����Ҫ���¼�ʱ
                    if(FLAG.water){//����Ȼ����
                        remain = TIMEPARAM.water - WATER.watercount[1];
                        //���ж���ԭ����ʱ�Ƿ����15S,������15s���ӳ���15S(ֱ���޸Ĳ���watercount[1])
                        if(remain < 15000 )//15S
                            WATER.watercount[1] = TIMEPARAM.water - SPITE_WATERDELAY;
                    }
                }
            break;
            /**********************************************************/
            case SPITE_FUNC_BACKFINISH://�ص�ԭ��

                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        if(SPITE.original_lightstate){//�ظ�����ǰ��״̬��Light
                            SPITE.original_lightstate = 0;
                            if(FLAG.light == 0){
                                PRO_dev.DZ |= (1 << POS_LIGHT);FLAG.light = 1;}                    
                        }
                        //water���ûָ�                        
                    break;
                    case CUREBOARD_DIM:
                        SPITE.work = 0;//PRO_dev.DZ |= (1 << POS_LIGHT);
                        if(SPITE.original_lightstate)
                        {
                            if(STEPLESSDIMMING.flag == 0)
                                CURE_Dimming_Start(&STEPLESSDIMMING);
                            STEPLESSDIMMING.SPITEFUNC.spite = 0;
                        }
                    break;  
                }
                CURE_SPITE_Clear();//���SPITE��صĲ���
            break;
            /**********************************************************/    
            case SPITE_FUNC_BUSTQUIT://�˳�
                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        //light�ָ���֮ǰ��״̬
                        if(SPITE.original_lightstate){//�ظ�����ǰ��״̬��Light
                            SPITE.original_lightstate = 0;
                            if(FLAG.light == 0){
                                PRO_dev.DZ |= (1 << POS_LIGHT);FLAG.light = 1;}                    
                        }
                        //water���ûָ�                        
                    break;
                    case CUREBOARD_DIM:
                        SPITE.work = 0;//PRO_dev.DZ |= (1 << POS_LIGHT);
                        if(SPITE.original_lightstate)
                        {
                            if(STEPLESSDIMMING.flag == 0)
                                CURE_Dimming_Start(&STEPLESSDIMMING);
                            STEPLESSDIMMING.SPITEFUNC.spite = 0;
                        }
                    break;
                }
                CURE_SPITE_Clear();//���SPITE��صĲ���
            break;
        }
        *flag = 0;
    }
}
void CURE_SPITE_Clear(void)
{
    SPITE.cmd = 0;SPITE.flag = 0;SPITE.work = 0;
    SPITE.original_lightstate = 0;SPITE.original_waterstate = 0;
}
/*************************************�Լ����****************************************/
void CURE_CheckSelf_Send(u8 *send)
{
    if(CHECKSELF.stage_in == 1){
        switch(CHECKSELF.cmdpre)
        {
            case CHECKSELF_FUNC_START://�Լ쿪ʼ
                *send |= (CHECKSELF_FUNC_START << 3);CHECKSELF.stage_in = 2;break;
//             case CHECKSELF_FUNC_DNBU://�������£���ǰ
//                 *send |= (CHECKSELF.cmd << 3);CHECKSELF.stage_in = 2;break;
//             break;
//             case CHECKSELF_FUNC_UPBD://�������ϡ�����  
//             break;
//             case CHECKSELF_FUNC_QUIT://�˳�
//             break;
        }
    }
    else if(CHECKSELF.stage_out == 1){
        *send |= (CHECKSELF.cmd << 3);CHECKSELF.stage_out = 2; 
    }
}
//cmd������ȷ��
void CURE_CheckSelf_CmdSet(u8 cmd)
{
    if(CHECKSELF.stage_in == 2 || CHECKSELF.stage_in == 0 || CHECKSELF.stage_out == 2 || cmd == 16)
    {
        switch(cmd)
        {
            /*******************************�Լ�ģʽ*********************************/
            case CHECKSELF_FUNC_START://�����Լ�ģʽ��    ���ƻ� --> ����
                CHECKSELF.cmd = CHECKSELF_FUNC_START;
                CHECKSELF.cmdpre = CHECKSELF_FUNC_CLEAR;
				CHECKSELF.stage_in = CHECKSELF_FUNC_CLEAR;
				CURE_SET_ClearAll(&SET);	
                
                BEEP_dev.start(1,150,300); 
                CHECKSELF.flag = 1;//�Լ��־λ
                CHECKSELF.mode = CHECKSELF_MODE_DNBU_RUN;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_DNBU://�Լ�ģʽ�Ѵﵽ���£���ǰ   ���� --> ���ƻ�
                CHECKSELF.mode = CHECKSELF_MODE_DNBU_DELAY;//��ʼ������ʱģʽ
                CHECKSELF.times = 1;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_UPBD://�Լ�ģʽ�Ѵﵽ���ϣ����   ���� --> ���ƻ�
                CHECKSELF.mode = CHECKSELF_MODE_UPBD_DELAY;//��ʼ������ʱģʽ
                CHECKSELF.times = 1;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_QUIT://�Լ�ģʽ�˳���    ���ƻ� --> ����
                BEEP_dev.start(1,150,300); 
                CURE_CheckSelf_Clear();
            break;
            /*********************************************************************/
        }
    }
}
/*
CHECKSELF.mode:
1:���£���ǰ�˶�״̬
2:���£���ǰ��ʱ״̬
3:���ϣ������˶�״̬
4:���ϣ������ʱ״̬
0:�κ�״̬������
*/
void CURE_CheckSelf_Deal(void)
{
    if(CHECKSELF.flag)//�Ƿ����Լ�״̬
    {
        switch(CHECKSELF.mode)
        {
            /******************************************************/
            case 1:break;//���£���ǰ�˶�״̬ 
            /******************************************************/
            case 2://���£���ǰ��ʱ״̬
                if(CHECKSELF.times == 1 || CHECKSELF.times == 3){//������light,watch
                    FLAG.light = 1;PRO_dev.DZ |= (1 << POS_LIGHT);
                    FLAG.watch = 1;PRO_dev.DZ |= (1 << POS_WATCH);
                    FLAG.dc24v36v = 1;PRO_dev.DZ |= (1 << POS_D);
                }
                else if(CHECKSELF.times == 2 || CHECKSELF.times == 4){
                    FLAG.light = 0;PRO_dev.DZ &= ~(1 << POS_LIGHT);
                    FLAG.watch = 0;PRO_dev.DZ &= ~(1 << POS_WATCH);
                    FLAG.dc24v36v = 0;PRO_dev.DZ &= ~(1 << POS_D);
                }
                else if(CHECKSELF.times == 0)//��ʱֹͣ
                    CHECKSELF.mode = 3;
            break;
            /******************************************************/
            case 3:break;//���ϣ������˶�״̬
            /******************************************************/
            case 4://���ϣ������ʱ״̬
                if(CHECKSELF.times == 1 || CHECKSELF.times == 3){//������CUP,WATER
                    FLAG.cup = 1;PRO_dev.DZ |= (1 << POS_CUP);
                    FLAG.water = 1;PRO_dev.DZ |= (1 << POS_WATER);
                }
                else if(CHECKSELF.times == 2 || CHECKSELF.times == 4){
                    FLAG.cup = 0;PRO_dev.DZ &= ~(1 << POS_CUP);
                    FLAG.water = 0;PRO_dev.DZ &= ~(1 << POS_WATER);
                }
                else if(CHECKSELF.times == 0)
                    CHECKSELF.mode = 1;
            break;
            /******************************************************/
        }
    }
}
//��Ҫ�����ڶ�ʱ���е���
u8 CURE_CheckSelf_Delay_Detect(u8 *type,u32 *couter,u8 *times)
{
    if(*type == CHECKSELF_MODE_DNBU_DELAY || *type == CHECKSELF_MODE_UPBD_DELAY)
    {
        switch(*type)
        {
            /****************************************************/
            case CHECKSELF_MODE_DNBU_DELAY:    
                if(*couter == CHECKSELF_DNBU_DIV_VALUE) *times = 2;   
                else if(*couter == CHECKSELF_DNBU_DIV_VALUE*2)*times = 3;
                else if(*couter == CHECKSELF_DNBU_DIV_VALUE*3)*times = 4;
                else if(*couter == CHECKSELF_DNBU_DIV_VALUE*4){*times = 0;*couter = 0;*type = 3;}
            break;
            /****************************************************/
            case CHECKSELF_MODE_UPBD_DELAY:
                if(*couter == CHECKSELF_UPBD_DIV_VALUE){*times = 2;} 
                else if(*couter == CHECKSELF_UPBD_DIV_VALUE*2){*times = 3;} 
                else if(*couter == CHECKSELF_UPBD_DIV_VALUE*3){*times = 4;}   
                else if(*couter == CHECKSELF_UPBD_DIV_VALUE*4){
                    *times = 0;
                    *couter = 0;
                    *type = 1;
                }
            break;  
            /****************************************************/
        }        
    }
    return 0;
}
//�����������
void CURE_CheckSelf_Clear(void)
{
    CHECKSELF.cmd = 0;CHECKSELF.cmdpre = 0;CHECKSELF.counter[0] = 0;CHECKSELF.counter[1] = 0;
    CHECKSELF.flag = 0;CHECKSELF.stage_in = 0;CHECKSELF.stage_out = 0;CHECKSELF.times = 0;
    CHECKSELF.mode = 0;CHECKSELF.light = 0;CHECKSELF.lightcouter = 0;
    //ͬʱֹͣwater,cup,watch,light�Ĳ���
    FLAG.light = 0;PRO_dev.DZ &= ~(1 << POS_LIGHT);
    switch(CURETYPE.board)
    {
        case CUREBOARD_1RANK:break;
        case CUREBOARD_3RANK:CURE_3RANK_Clear(&CURELIGHT);break;
        case CUREBOARD_DIM:CURE_Dimming_Clear(&STEPLESSDIMMING);break;
    }
    FLAG.watch = 0;PRO_dev.DZ &= ~(1 << POS_WATCH);
    FLAG.cup = 0;PRO_dev.DZ &= ~(1 << POS_CUP);
    FLAG.water = 0;PRO_dev.DZ &= ~(1 << POS_WATER);
    FLAG.dc24v36v = 0;PRO_dev.DZ &= ~(1 << POS_D);
}
//�Լ�״̬�ƵĿ���,s1-s5��˸
u8 CURE_CheckSelf_LightControl(TypeDef_CHECKSELF *self,u32 *value)
{
    if(self->flag){
        switch(self->light){
            case 0:*value &= ~(0X3FF << POS_S1);break;//��
            case 1:*value |= (0X3FF << POS_S1);break;//��
        } 
    }
    return 0;
}
//�Լ�״̬���κ�һ�������˳�
u8 CURE_CheckSelf_Quit_Key(TypeDef_CHECKSELF *self,u32 key)
{
    if(self->flag){
        if(key){
            self->cmd = CHECKSELF_FUNC_QUIT;//�˳�ָ��
            self->stage_out = 1;            //�Ĵ�����
        }
        return 1;
    }
    return 0;
}
/********************************������������*****************************************/
s8 CURE_Sensor_Detect(TypeDef_SENSOR *sensor,u32 *dz)
{
    u8 i = 0,count = 0;
    u32 temp = *dz;
    if(temp & (0x7 << POS_S1)){
        temp >>= POS_S1;
        for(i = 0;i < 3;i ++){
            if(temp&0x01)
                count ++;
            temp >>= 1;
        }
    }else{
        sensor->sensor &= ~(3 << 0);
        CURE_Sensor_Clear(sensor);
        return count;
    }
    if(count == 1){
        sensor->sensor &= ~(3 << 0);
        CURE_Sensor_Clear(sensor);
        return count;
    } 
    temp = *dz;
    if(count > 1 && (temp & (1 << POS_S3))){
        if(sensor->sensor == 0){
            CURE_Sensor_StopAction();
        }
        switch(count){
            case 2:
                if(temp & (1 << POS_S1)){
                    sensor->sensor |= 1 << 0;
                    sensor->sensor &= ~(1 << 1);
                }
                else if(temp & (1 << POS_S2)){
                    sensor->sensor |= 1 << 1;
                    sensor->sensor &= ~(1 << 0);
                }
            break;
            case 3:sensor->sensor |= 3 << 0;break;
        }
    }
    return count;
}
void CURE_Sensor_Send(TypeDef_SENSOR *sensor,u32 *value)
{
    if(sensor->sensor){
        *value &= ~(0x7 << POS_S1);        
        switch(sensor->sensor){
            case ORIENT_UPDN:
                if(sensor->light) *value |= 1 << POS_S1;
                else *value &= ~(1 << POS_S1);
            break;
            case ORIENT_BUBD:
                if(sensor->light) *value |= 1 << POS_S2;
                else *value &= ~(1 << POS_S2);
            break;
            case ORIENT_BOTH:
                if(sensor->light) *value |= 3 << POS_S1;
                else *value &= ~(3 << POS_S1);
            break;
        }
    }
}
void CURE_Sensor_Clear(TypeDef_SENSOR *sensor)
{
    sensor->counter = 0;
    sensor->light = 0;
    sensor->sensor = 0;
}
void CURE_Sensor_StopAction(void)
{
    CURE_SPITE_Clear();
    CURE_CheckSelf_Clear();
    PRO_dev.DZ &= ~(0X1F << 10);        //��� S1 S2 S3 S4 S5,��bit14~bit10
    if(SET.priority > 1)
        PRO_dev.DZ &= ~(0X1 << 15);
    PRO_dev.DZ &= ~(0X1 << 16);         //��� RESET
    PRO_dev.DZ &= ~(0X1 << 17);         //��� LP 
    PRO_dev.DZ &= ~(0X1 << 18);         //��� SP
    PRO_dev.DZ &= ~(0XF << 0);          //���ֹͣ 
}
void CURE_S1TOS5_Figure_Detect(u32 *dz)
{
    u8 i = 0,count = 0;
    u32 temp = *dz;
    if(temp & (0x7 << POS_S3)){
        temp >>= POS_S3;
        for(i = 0;i < 3;i ++){
            if(temp&0x01)
                count ++;
            temp >>= 1;
        }
    }
    else{S1TOS5.state = FIGURE_CLEAR;return;}
    if(count <= 1)
    {S1TOS5.state = FIGURE_CLEAR;return;}
    temp = *dz;
    if(temp & (1 << POS_S3)){
        switch(count)
        {
            case 2:
                if(temp & (1 << POS_S4)){
                    S1TOS5.state = FIGURE_START;
                    PRO_dev.DZ &= ~(7 << POS_S3);
                    *dz &= ~(7 << POS_S3);
                }
                else if(temp & (1 << POS_S5)){
                    S1TOS5.state = FIGURE_FINISH;
                    PRO_dev.DZ &= ~(7 << POS_S3);
                    *dz &= ~(7 << POS_S3);
                }
            break;
            case 3:break;
        }        
    }
}
/***************************************************************************************/
//2016-3-29
//����SET�Ժ��Զ�����ֹͣ���Զ����а���(S1~S5,SP,RESET,LP)
void CURE_AutoRunStop_PressSet(u32 *temp)
{
    if(*temp & (1 << POS_SET)){
        if(((CHAIR.DZ & (0X1F << 10)) != 0) || ((CHAIR.DZ & (0X7 << 16)) != 0)){
            /**************************************************/
            if((CHAIR.DZ & (0X1F << 10)) != 0)//s1~s5
                if(SENSOR.sensor == 0)//�����Կ��ư�
                    {*temp &= ~(1 << POS_SET);*temp |= 1 << POS_AUTOSTOP_SET;}
            /**************************************************/
            if((CHAIR.DZ & (0x3 << 16)) != 0)//reset lp
            {*temp &= ~(1 << POS_SET);*temp |= 1 << POS_AUTOSTOP_SET;}
            else{*temp |= 1 << POS_AUTOSTOP_SET;}
            /**************************************************/
            if((CHAIR.DZ & (0x1 << 18)) != 0)//sp
            {*temp &= ~(1 << POS_SET);*temp |= 1 << POS_AUTOSTOP_SET;}
            /**************************************************/
        }
        else *temp &= ~(1 << POS_AUTOSTOP_SET);  
    }
    else *temp &= ~(1 << POS_AUTOSTOP_SET);  
}
/***************************************************************************************/
//2016-3-29
//��SET�ĸ߼�״̬ʱ�������ֻ��λ��������˳�SET״̬����ֹͣ����
void CURE_SetAdvanceStop_Lock(void)
{
    if(LOCK.lock == 1 && (SET.priority == 2 || SET.priority == 3))
    {
        CURE_ExitSet_SaveNone(POS_LOCK_CC);
    }
}
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/
/***************************************************************************************/


/***************************************************************************************/

