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
/*******************************SET相关函数****************************************/
//SET优先级检测函数
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
//SET确认函数
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
                    //同时要把其他的的设置清除，LIGHT和WATER
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
                                    case 1://只设置了sec
                                        TIMEPARAM.water = (WATER.sec / 1000) * 5000;
                                        WATER.unlimit = 0;
                                    break;
                                    case 2://设置了min
                                        TIMEPARAM.water = ((WATER.sec / 1000) * 5000) + ((WATER.min / 1000) * 60000);
                                        WATER.unlimit = 0;
                                    break;
                                    case 3://时间为无限长
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
                    //清除S1~S5
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
2：普通设置已发送
5：手动设置已发送
8：自动设置已发送
SET.stage_out：
2：退出设置已发送*/
void CURE_SET_priority_Set(u8 prio)
{
    if( SET.stage_in == 2 || SET.stage_in == 5 || SET.stage_in == 8
        || SET.stage_out == 2 || SET.stage_out == 7 || prio == 7
      )
    {    
        switch(prio)
        {
            /****************************设置模式***************************/
            case 1://普通设置
                SET.priority = 1;
                SET.chairprio = 1;//椅子板处于普通设置状态
                BEEP_dev.start(1,150,300);   
            break;
            case 2://优先级2，手动设置
                SET.priority = 2;
                SET.chairprio = 2;
                BEEP_dev.start(1,500,300);
            break;
            case 3://优先级3，自动设置
                SET.priority = 3;
                SET.chairprio = 3;
                BEEP_dev.start(1,1000,300);
            break;
            case 4://优先级4，即完成了设置的工作，保存退出
                BEEP_dev.start(1,150,300);
//                CURE_SET_Sure(SET.priority);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//清除完成
            break;
            case 5:
                BEEP_dev.start(1,150,300);
                SET.priority = 2;
            break;
            case 6://优先级6，不保存退出
                BEEP_dev.start(1,150,300);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//清除完成
            break;
            case 7://由椅子板主动发送过来的值，表示椅子板的自动设置已经完成
                BEEP_dev.start(1,150,300);
                PRO_dev.DZ &= ~(1 << 15);
                FLAG.set = 0;
                SET.priority = 0;
                SET.stage_out = 0;//清除完成
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

//进入或退出的发送函数
/*
功能号：
1：普通设置
2：手动极限设置
3：自动极限设置
4：确认退出，保存
6：确认退出，不保存
*/
void CURE_SET_InOut_Send(void)
{
    if(SET.stage_in == 1 || SET.stage_in == 4 || SET.stage_in == 7 || SET.stage_in == 13)//进入
    {                   
        switch(SET.prio_pre){
            case 1://普通设置
                PRO_dev.RS485SEND[3] |= (1 << 3);SET.stage_in = 2;break;
            case 2://手动设置
                PRO_dev.RS485SEND[3] |= (2 << 3);SET.stage_in = 5;break;
            case 3://自动设置
                PRO_dev.RS485SEND[3] |= (3 << 3);SET.stage_in = 8;break;
        }
        if(SET.stage_in == 1 && SET.manual == 1)//手动设置的第一次按键
        {PRO_dev.RS485SEND[3] |= (5 << 3);SET.stage_in = 2;}
    }
    else if(SET.stage_out == 1 || SET.stage_out == 6)//退出:1,保存退出，6，不保存退出
    {
        if(SET.stage_out == 1)//1,保存退出，
        {
            PRO_dev.RS485SEND[3] |= (4 << 3);//设置完成功能
            SET.stage_out = 2;
        }
        else if(SET.stage_out == 6)//6，不保存退出
        {PRO_dev.RS485SEND[3] |= (6 << 3);SET.stage_out = 7;}
    }
}
//处理从椅子板中返回信号的信息
/*
    SET.stage_in:
    2：普通设置已发送
    5：手动设置已发送
    8：自动设置已发送
    SET.stage_out：
    2：退出设置已发送
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
//8://开始向前运动
//9://运动到了前极限，或突发停止
//10://已回到了原点，或突发停止
void CURE_SPITE_Set(u8 temp)
{
    if(temp >= 8 && temp <=  12){
        SPITE.cmd = temp;SPITE.flag = 1;
    }
}
//SPITE处理函数
//original_lightstate:记录Light原来的状态
//original_waterstate:记录water原来的状态
//flag:系统处于SPITE状态
//cmd:指令，8，9，10，11
void CURE_SPITE_Deal(u8 *cmd,u8 *flag)
{
    u32 remain = 0;
    if(*flag)
    {
        switch(*cmd)
        {
            /**********************************************************/
            case SPITE_FUNC_STARTHEAD://开始向前运动
                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        if(FLAG.light == 1)  //原来开启，就关闭
                        {SPITE.original_lightstate = 1;PRO_dev.DZ &= ~(1 << POS_LIGHT);FLAG.light = 0;} 
                        if(FLAG.water == 1){//原来已开启,WATER重新开始计时
                            if(WATER.unlimit == 0)//原来不是无限状态，若是无限状态则不需要重新计时
                                CURE_WATER_ReCount();
                        }
                        else//原来没有开启
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
                if(FLAG.water == 1){//原来已开启,WATER重新开始计时
                    if(WATER.unlimit == 0)//原来不是无限状态，若是无限状态则不需要重新计时
                        CURE_WATER_ReCount();
                }
                else//原来没有开启
                    CURE_WATER_start();      
            break;
            /**********************************************************/
            case SPITE_FUNC_HEADFINISH://到达极限
            break;
            /**********************************************************/
            case SPITE_FUNC_STARTBACK://开始往后运动
                if(WATER.unlimit == 0){//原来不是无限状态，若是无限状态则不需要重新计时
                    if(FLAG.water){//还仍然开着
                        remain = TIMEPARAM.water - WATER.watercount[1];
                        //先判断其原开启时是否低于15S,若低于15s则延长到15S(直接修改参数watercount[1])
                        if(remain < 15000 )//15S
                            WATER.watercount[1] = TIMEPARAM.water - SPITE_WATERDELAY;
                    }
                }
            break;
            /**********************************************************/
            case SPITE_FUNC_BACKFINISH://回到原点

                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        if(SPITE.original_lightstate){//回复到以前的状态，Light
                            SPITE.original_lightstate = 0;
                            if(FLAG.light == 0){
                                PRO_dev.DZ |= (1 << POS_LIGHT);FLAG.light = 1;}                    
                        }
                        //water不用恢复                        
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
                CURE_SPITE_Clear();//清除SPITE相关的参数
            break;
            /**********************************************************/    
            case SPITE_FUNC_BUSTQUIT://退出
                switch(CURETYPE.board)
                {
                    case CUREBOARD_1RANK:case CUREBOARD_3RANK:
                        //light恢复到之前的状态
                        if(SPITE.original_lightstate){//回复到以前的状态，Light
                            SPITE.original_lightstate = 0;
                            if(FLAG.light == 0){
                                PRO_dev.DZ |= (1 << POS_LIGHT);FLAG.light = 1;}                    
                        }
                        //water不用恢复                        
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
                CURE_SPITE_Clear();//清除SPITE相关的参数
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
/*************************************自检操作****************************************/
void CURE_CheckSelf_Send(u8 *send)
{
    if(CHECKSELF.stage_in == 1){
        switch(CHECKSELF.cmdpre)
        {
            case CHECKSELF_FUNC_START://自检开始
                *send |= (CHECKSELF_FUNC_START << 3);CHECKSELF.stage_in = 2;break;
//             case CHECKSELF_FUNC_DNBU://到达最下，最前
//                 *send |= (CHECKSELF.cmd << 3);CHECKSELF.stage_in = 2;break;
//             break;
//             case CHECKSELF_FUNC_UPBD://到达最上、到后  
//             break;
//             case CHECKSELF_FUNC_QUIT://退出
//             break;
        }
    }
    else if(CHECKSELF.stage_out == 1){
        *send |= (CHECKSELF.cmd << 3);CHECKSELF.stage_out = 2; 
    }
}
//cmd的最终确认
void CURE_CheckSelf_CmdSet(u8 cmd)
{
    if(CHECKSELF.stage_in == 2 || CHECKSELF.stage_in == 0 || CHECKSELF.stage_out == 2 || cmd == 16)
    {
        switch(cmd)
        {
            /*******************************自检模式*********************************/
            case CHECKSELF_FUNC_START://进入自检模式，    治疗机 --> 椅子
                CHECKSELF.cmd = CHECKSELF_FUNC_START;
                CHECKSELF.cmdpre = CHECKSELF_FUNC_CLEAR;
				CHECKSELF.stage_in = CHECKSELF_FUNC_CLEAR;
				CURE_SET_ClearAll(&SET);	
                
                BEEP_dev.start(1,150,300); 
                CHECKSELF.flag = 1;//自检标志位
                CHECKSELF.mode = CHECKSELF_MODE_DNBU_RUN;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_DNBU://自检模式已达到最下，最前   椅子 --> 治疗机
                CHECKSELF.mode = CHECKSELF_MODE_DNBU_DELAY;//开始进入延时模式
                CHECKSELF.times = 1;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_UPBD://自检模式已达到最上，最后   椅子 --> 治疗机
                CHECKSELF.mode = CHECKSELF_MODE_UPBD_DELAY;//开始进入延时模式
                CHECKSELF.times = 1;
            break;
            /***********************************************************************/
            case CHECKSELF_FUNC_QUIT://自检模式退出，    治疗机 --> 椅子
                BEEP_dev.start(1,150,300); 
                CURE_CheckSelf_Clear();
            break;
            /*********************************************************************/
        }
    }
}
/*
CHECKSELF.mode:
1:往下，往前运动状态
2:最下，最前延时状态
3:往上，往后运动状态
4:最上，最后延时状态
0:任何状态都不是
*/
void CURE_CheckSelf_Deal(void)
{
    if(CHECKSELF.flag)//是否处于自检状态
    {
        switch(CHECKSELF.mode)
        {
            /******************************************************/
            case 1:break;//往下，往前运动状态 
            /******************************************************/
            case 2://最下，最前延时状态
                if(CHECKSELF.times == 1 || CHECKSELF.times == 3){//开启，light,watch
                    FLAG.light = 1;PRO_dev.DZ |= (1 << POS_LIGHT);
                    FLAG.watch = 1;PRO_dev.DZ |= (1 << POS_WATCH);
                    FLAG.dc24v36v = 1;PRO_dev.DZ |= (1 << POS_D);
                }
                else if(CHECKSELF.times == 2 || CHECKSELF.times == 4){
                    FLAG.light = 0;PRO_dev.DZ &= ~(1 << POS_LIGHT);
                    FLAG.watch = 0;PRO_dev.DZ &= ~(1 << POS_WATCH);
                    FLAG.dc24v36v = 0;PRO_dev.DZ &= ~(1 << POS_D);
                }
                else if(CHECKSELF.times == 0)//计时停止
                    CHECKSELF.mode = 3;
            break;
            /******************************************************/
            case 3:break;//往上，往后运动状态
            /******************************************************/
            case 4://最上，最后延时状态
                if(CHECKSELF.times == 1 || CHECKSELF.times == 3){//开启，CUP,WATER
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
//主要用于在定时器中调用
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
//参数清除函数
void CURE_CheckSelf_Clear(void)
{
    CHECKSELF.cmd = 0;CHECKSELF.cmdpre = 0;CHECKSELF.counter[0] = 0;CHECKSELF.counter[1] = 0;
    CHECKSELF.flag = 0;CHECKSELF.stage_in = 0;CHECKSELF.stage_out = 0;CHECKSELF.times = 0;
    CHECKSELF.mode = 0;CHECKSELF.light = 0;CHECKSELF.lightcouter = 0;
    //同时停止water,cup,watch,light的操作
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
//自检状态灯的控制,s1-s5闪烁
u8 CURE_CheckSelf_LightControl(TypeDef_CHECKSELF *self,u32 *value)
{
    if(self->flag){
        switch(self->light){
            case 0:*value &= ~(0X3FF << POS_S1);break;//灭
            case 1:*value |= (0X3FF << POS_S1);break;//亮
        } 
    }
    return 0;
}
//自检状态按任何一个按键退出
u8 CURE_CheckSelf_Quit_Key(TypeDef_CHECKSELF *self,u32 key)
{
    if(self->flag){
        if(key){
            self->cmd = CHECKSELF_FUNC_QUIT;//退出指令
            self->stage_out = 1;            //四次握手
        }
        return 1;
    }
    return 0;
}
/********************************传感器检测操作*****************************************/
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
    PRO_dev.DZ &= ~(0X1F << 10);        //清除 S1 S2 S3 S4 S5,即bit14~bit10
    if(SET.priority > 1)
        PRO_dev.DZ &= ~(0X1 << 15);
    PRO_dev.DZ &= ~(0X1 << 16);         //清除 RESET
    PRO_dev.DZ &= ~(0X1 << 17);         //清除 LP 
    PRO_dev.DZ &= ~(0X1 << 18);         //清除 SP
    PRO_dev.DZ &= ~(0XF << 0);          //电机停止 
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
//按下SET以后，自动运行停止，自动运行包括(S1~S5,SP,RESET,LP)
void CURE_AutoRunStop_PressSet(u32 *temp)
{
    if(*temp & (1 << POS_SET)){
        if(((CHAIR.DZ & (0X1F << 10)) != 0) || ((CHAIR.DZ & (0X7 << 16)) != 0)){
            /**************************************************/
            if((CHAIR.DZ & (0X1F << 10)) != 0)//s1~s5
                if(SENSOR.sensor == 0)//记忆性控制板
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
//在SET的高级状态时，若出现机椅互锁，则退出SET状态，并停止运行
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

