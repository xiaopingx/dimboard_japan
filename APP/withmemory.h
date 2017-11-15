#ifndef _WITHMEMORY_H_
#define _WITHMEMORY_H_
#include "sys.h"
#include "hardware.h"
/****************************************************/
typedef struct
{
    u8 cmd;
    u8 flag;
    u8 original_waterstate;
    u8 original_lightstate;
    u8 work;
}TypedDef_SPITE;
extern TypedDef_SPITE SPITE;

typedef struct
{
    u8 cmd;
    u8 cmdpre;
    u8 flag;
    u32 counter[2];
    u8 times;
    u8 stage_in;
    u8 stage_out;
    u8 mode;
    u8 light;
    u32 lightcouter;
}TypeDef_CHECKSELF;
extern TypeDef_CHECKSELF CHECKSELF;

typedef struct
{
    
    u8 sensor;//bit0：UPDN,bit1:BUBD
    u32 counter;
    u8 light;
    
}TypeDef_SENSOR;
extern TypeDef_SENSOR SENSOR;
typedef struct
{
    u8 state;
}TypedDef_S1TOS5;
extern TypedDef_S1TOS5 S1TOS5;
//设置键的时间设置
//#define SET_PRIORITY2   5000    //手动5s
//#define SET_PRIORITY3   10000   //自动10s

#define SET_PRIORITY2   1000*5    //手动60s
#define SET_PRIORITY3   1000*10   //自动120s
/****************************************************/
#define SET_STATE_CLEAR             0
#define SET_STATE_GENERAL_SEND      1
#define SET_STATE_GENERAL_SENDED    2
#define SET_STATE_GENERAL_ACK       3
#define SET_STATE_MANUAL_SEND       4
#define SET_STATE_MANUAL_SENDED     5
#define SET_STATE_MANUAL_ACK        6
#define SET_STATE_AUTO_SEND         7
#define SET_STATE_AUTO_SENDED       8
#define SET_STATE_AUTO_ACK          9
/****************************************************/
#define SPITE_CLEAR                 0
#define SPITE_FUNC_STARTHEAD        8
#define SPITE_FUNC_HEADFINISH       9
#define SPITE_FUNC_STARTBACK        10
#define SPITE_FUNC_BACKFINISH       11
#define SPITE_FUNC_BUSTQUIT         12
#define SPITE_WATERDELAY            15000
/***************************************************/
#define CHECKSELF_FUNC_CLEAR        0
#define CHECKSELF_FUNC_START        13
#define CHECKSELF_FUNC_DNBU         14
#define CHECKSELF_FUNC_UPBD         15
#define CHECKSELF_FUNC_QUIT         16
#define CHECKSELF_DNBU30S           9200
#define CHECKSELF_UPBD30S           9200
#define CHECKSELF_MODE_DNBU_RUN         1
#define CHECKSELF_MODE_DNBU_DELAY       2
#define CHECKSELF_MODE_UPBD_RUN         3
#define CHECKSELF_MODE_UPBD_DELAY       4
#define CHECKSELF_DNBU_DELAY_DIV        (2*2)
#define CHECKSELF_UPBD_DELAY_DIV        (2*2)
#define CHECKSELF_DNBU_DIV_VALUE        (CHECKSELF_DNBU30S/CHECKSELF_DNBU_DELAY_DIV)
#define CHECKSELF_UPBD_DIV_VALUE        (CHECKSELF_UPBD30S/CHECKSELF_UPBD_DELAY_DIV)
/***************************************************/
#define ORIENT_NONE                 0
#define ORIENT_UPDN                 1
#define ORIENT_BUBD                 2
#define ORIENT_BOTH                 3
/***************************************************/
#define FIGURE_CLEAR                0
#define FIGURE_START                1
#define FIGURE_FINISH               2
/***************************************************/



u8 CURE_SET_Pripre_detect(u32 times);
void CURE_SET_Sure(u8 prio);
void CURE_SET_priority_Set(u8 prio);
void CURE_SET_InOut_Send(void);
void CURE_SET_InOut_ACK(u8 temp);
void CURE_SET_ClearAll(TypeDef_SET *set);
void CURE_SPITE_Set(u8 temp);
void CURE_SPITE_Deal(u8 *cmd,u8 *flag);
void CURE_SPITE_Clear(void);
void CURE_CheckSelf_Send(u8 *send);
void CURE_CheckSelf_ACK(void);
void CURE_CheckSelf_CmdSet(u8 cmd);
u8 CURE_CheckSelf_Delay_Detect(u8 *type,u32 *couter,u8 *times);
void CURE_CheckSelf_Deal(void);
void CURE_CheckSelf_Clear(void);
u8 CURE_CheckSelf_LightControl(TypeDef_CHECKSELF *self,u32 *value);
u8 CURE_CheckSelf_Quit_Key(TypeDef_CHECKSELF *self,u32 key);
s8 CURE_Sensor_Detect(TypeDef_SENSOR *sensor,u32 *dz);
void CURE_Sensor_Send(TypeDef_SENSOR *sensor,u32 *value);
void CURE_Sensor_Clear(TypeDef_SENSOR *sensor);
void CURE_Sensor_StopAction(void);
void CURE_Cup_Clear(void);
void CURE_Cup_Stop(void);
void CURE_Water_Clear(void);
void CURE_Water_Stop(void);
void CURE_DC24V36V_Clear(void);
void CURE_DC24V36V_Stop(void);
void CURE_SetEnter_Peripheral_Stop(void);
void CURE_S1TOS5_Figure_Detect(u32 *dz);
void CURE_AutoRunStop_PressSet(u32 *temp);
void CURE_SetAdvanceStop_Lock(void);
#endif

