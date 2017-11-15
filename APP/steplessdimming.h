#ifndef __STEPLESS_DIMMING_H
#define __STEPLESS_DIMMING_H
#include "sys.h"
#define KEY_UP              0
#define KEY_DOWN            1
#define DIM_CLEAR           0
#define DIM_ADD             1
#define DIM_DECENT          2
#define OFFSET_FLAG					0
#define OFFSET_STEPLESSDIMMING		4
#define FLAGVALUE			0X123890AC
#define LED					P12


#define CONTROL_DRIVER		P34
#define BIT3_OUTPUT			P32
#define BIT2_INPUT			P13
#define BIT1_INPUT			P14
#define BIT0_INPUT			P15
#define BIT_HIGHLEVEL		1
#define BIT_LOWLEVEL		0

#define DIM_CMD_0000					0
#define	DIM_CMD_0001					1
#define	DIM_CMD_0010					2
#define DIM_CMD_0011					3
#define DIM_CMD_0111					7

#define DIM_STATE_SHUTDOWN				DIM_CMD_0000
#define DIM_STATE_START_HOLD			DIM_CMD_0001
#define DIM_STATE_ADD					DIM_CMD_0010
#define DIM_STATE_SUB					DIM_CMD_0011
#define DIM_STATE_GENERAL_ON			DIM_CMD_0111
#define DIM_STATE_GENERAL_OFF			DIM_STATE_SHUTDOWN

#define TYPE_GENERAL					0
#define TYPE_DIM						1

typedef struct
{
	u32 count[2];
	u32 flag;
	
}Typedef_Bit;
typedef struct
{
	Typedef_Bit bit[3];
	u8 state;
	u8 sended;
	struct{
		u8 flag;
		u32 count;
		u32 var_param;
	}start;
}Typedef_BITREAD;
extern Typedef_BITREAD BITREAD;

typedef struct
{
	u8 light;
	u8 driverEn;
}Typedef_FLAG;
extern Typedef_FLAG FLAG;

typedef struct
{
    u8 plusEn;//已检测到脉冲
    void (*int_open)(void);
    void (*int_close)(void);
    void (*timer_open)(void);
    void (*timer_close)(void);
    
}TypeDef_PLUS;
typedef struct
{
    u8 spite;
}TypeDef_SPITEFUNC;

typedef struct
{
    TypeDef_PLUS PLUS;
    TypeDef_SPITEFUNC SPITEFUNC;
    s32 param;//记录需要延迟的参数
    u32 param_last;
    u32 param_count;//实际的计数值
    u8 param_flag;
    u8 param_finish;
    u8 close;//关闭冷光灯
    u8 func_running;//功能正在运行标志,及开始对其进行斩波操作
    u8 flag;
    
    
}Typedef_STEPLESSDIMMING;
extern Typedef_STEPLESSDIMMING STEPLESSDIMMING;


void CURE_Stepless_Dimming_Init(void);
void CURE_Dimming_KeyDeal(Typedef_STEPLESSDIMMING *dim,u8 keyupdn);
void CURE_Dimming_Clear(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_ParamDetect(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_Detect(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_PlusRead(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_ParamStart_Init(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_TimerCount(Typedef_STEPLESSDIMMING *dim);
// void CURE_Dimming_Start(Typedef_STEPLESSDIMMING *dim);
void CURE_Dimming_Start(Typedef_STEPLESSDIMMING *dim,u8 genral);
void CURE_Dimming_Stop(Typedef_STEPLESSDIMMING *dim);

void TIM1_init(u8 pre,u32 tcmp);
void Japan_Dim_Perpher_OutPut(void);
void Japan_Dim_ReadBit(void);
s8 Japan_Dim_state_detect(Typedef_BITREAD *bitread);
void CURE_Peripheral_Carryout(void);
void DATAFLASH_init(u32 dfba_startaddr);
void Japan_Dim_Start_ParamterVariable(void);
#endif

