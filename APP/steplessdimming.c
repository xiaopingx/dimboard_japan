#include "steplessdimming.h"
#include "mini51eeprom.h"
#include "timer.h"
#include "string.h"
static void CURE_Stepless_IntFun_Open(void);
static void CURE_Stepless_IntFun_Close(void);
static void TIM1_Open(void);
static void TIM1_Close(void);
Typedef_STEPLESSDIMMING STEPLESSDIMMING =
{
    {
        0,
        CURE_Stepless_IntFun_Open,
        CURE_Stepless_IntFun_Close,
        TIM1_Open,
        TIM1_Close,
    },
    {0},
    0,//param
    0,//param_last
    0,//param_count
    0,//param_flag
    0,//param_finish
    0,//close

    0,//func_running
    0,//flag
    
};
//无极调光初始化
void CURE_Stepless_Dimming_Init(void)
{
    /*****************************************************************/
    //中断模式P0.7
    P0->IMD &= ~(1 << 7);           //边沿触发终端
    P0->IEN &= ~(1 << (7 + 16));    //关闭中断
    /*****************************/
    P0->DBEN |= 1 << 7;             //允许防抖
    GPIO->DBNCECON |= 1 << 5;       
    GPIO->DBNCECON &= ~(1 << 4);
    GPIO->DBNCECON &= ~(0XF << 0);
    GPIO->DBNCECON |= 5 << 0;        
    MY_NVIC_Init(GPIO01_IRQn,1);    // 
    /*****************************************************************/
    //采用定时器1进行计数
//     TIM1_init(22-1,(1000*10)-1);
    TIM1_init(22-1,10-1);
    /***************************************/
    //P3.4控制光耦的通断
	P3->PMD &= ~(3 << 4*2);
	P3->PMD |= 1 << 4*2;
	P3->DOUT &= ~(1 << 4);
	/***************************************/
	//四根控制脚
/*
		治疗机     无极调光单片机             代号
		 P25:       U13-6 (P32)     ==>		DIM_OUTPUT
		 P23:       U13-4 (P15)		==> 	DIM_BIT2
		 P22:		U13-3 (P14)		==> 	DIM_BIT1
		 P21:		U13-2 (P13)		==> 	DIM_BIT0
		DIM_BIT2   DIM_BIT1   DIM_BIT0
		   0		  0			 0			==>  关闭
		   0          0          1			==>  开启保持
		   0          1          0			==>  亮度变量
		   0		  1			 1			==>  亮度变暗
		   1		  1			 1			==>  开关量打开

*/
	P1->PMD &= ~(0X3F << 3*2);
	P3->PMD &= ~(0X3 << 2*2);
	P3->PMD |= 1 << 2*2;
	P3->DOUT &= ~(1 << 2);
	/****************************************/
	//LED灯指示P1.2
	P1->PMD &= ~(3 << 2*2);
	P1->PMD |= 1 << 2*2;
	P1->DOUT |= 1 << 2;
	/***************************************/
	//BEEP
	P3->PMD &= ~(3 << 2*5);
	P3->PMD |= 1 << 2*5;
	P3->DOUT &= ~(1 << 5);
    
}
static void CURE_Stepless_IntFun_Open(void)
{
    P0->IEN |= 1 << (7 + 16);//打开中断,上升沿触发
}
static void CURE_Stepless_IntFun_Close(void)
{
    P0->IEN &= ~(1 << (7 + 16));//关闭中断,上升沿触发
}
//中断处理函数
void GPIO01_IRQHandler(void)
{
    if(P0->ISRC & (1 << 7))
    {
        
        STEPLESSDIMMING.PLUS.plusEn = 1;
        CURE_Dimming_PlusRead(&STEPLESSDIMMING);
        
    }
    P0->ISRC |= 1 << 7;//清除标志位
}
void CURE_Peripheral_Carryout(void)
{
	if(FLAG.driverEn)
		CONTROL_DRIVER = 1;
	else
		CONTROL_DRIVER = 0;
	
	Japan_Dim_Perpher_OutPut();
}
//斩波脉冲读函数
void CURE_Dimming_PlusRead(Typedef_STEPLESSDIMMING *dim)
{
    if(dim->PLUS.plusEn == 1){
        dim->PLUS.plusEn = 0;
        dim->param_flag = 1;//可以开始参数的运行,即检测到脉冲
        if(dim->param != 0)
            FLAG.driverEn = 0;     //关闭，不导通
        else FLAG.driverEn = 1;
        CURE_Peripheral_Carryout();	//即打开控制端
    }
}
void CURE_Dimming_Start(Typedef_STEPLESSDIMMING *dim,u8 genral)
{
    dim->flag = 1;
	if(genral == TYPE_GENERAL){
		dim->param = 0;
		dim->param_last = dim->param;
	}
    else dim->param_last = dim->param;       //保存刚开始的值
    dim->PLUS.int_open();               //打开中断，可以读取脉冲
    dim->PLUS.timer_open();             //打开定时器1开始计时
    CURE_Dimming_ParamStart_Init(dim);
	

	BITREAD.start.flag = 1;
	BITREAD.start.count = 0;
	BITREAD.start.var_param = 700;
}
void Japan_Dim_Start_ParamterVariable(void)
{
// 	if(BITREAD.start.flag){
// 		BITREAD.start.count ++;
// 		if(BITREAD.start.count >= 40)
// 		{
// 			BITREAD.start.count = 0;
// 			BITREAD.start.var_param --;
// 			if(BITREAD.start.var_param < 2){
// 				BITREAD.start.var_param = 0;
// 				BITREAD.start.flag = 0;
// 			}
// 			STEPLESSDIMMING.param_count = 0;
// 		}

// 	}
}
u8 Japan_Dim_Start_Detect(void)
{
	u8 ret = 0;
	if(FLAG.light == 0)
		ret= 1;
	return ret;
}
void CURE_Dimming_Stop(Typedef_STEPLESSDIMMING *dim)
{
    //保存此次的参数
    if(dim->param != dim->param_last)//参数与刚开启时的不同，则保存参数
        DATAFLASH.save(OFFSET_STEPLESSDIMMING,dim->param,DF_PAGE1);
    CURE_Dimming_Clear(dim);                    
    FLAG.driverEn = 0;     //关闭输出
	BITREAD.sended = 0;
	memset(&BITREAD.start,0,sizeof(BITREAD.start));
}
u8 Japan_Dim_Stop_Detect(void)
{
	u8 ret = 0;
	if(FLAG.light == 1)
		ret = 1;
	return ret;
}
void CURE_Dimming_ParamStart_Init(Typedef_STEPLESSDIMMING *dim)
{
    dim->param_count = 0;dim->param_finish = 0;
}
void CURE_Dimming_ParamDetect(Typedef_STEPLESSDIMMING *dim)
{
    if(dim->param_finish == 1)
    {FLAG.driverEn = 1;dim->param_finish = 0;}
}
void CURE_Dimming_Detect(Typedef_STEPLESSDIMMING *dim)
{
    s32 temp = dim->param;
    static u16 add_cnt = 0,decent_cnt = 0;
    CURE_Dimming_ParamDetect(dim);
	switch(BITREAD.state)
	{
		case DIM_STATE_SHUTDOWN:
			if(Japan_Dim_Stop_Detect())
			{
				FLAG.light = 0;
				CURE_Dimming_Stop(&STEPLESSDIMMING);
				BITREAD.sended = 0;
			}
			
		break;
		case DIM_STATE_START_HOLD:
			if(Japan_Dim_Start_Detect())
			{
				FLAG.light = 1;
				CURE_Dimming_Start(&STEPLESSDIMMING,TYPE_DIM);
			}
			add_cnt = 0;decent_cnt = 0;
			BITREAD.sended = 0;
		break;
		case DIM_STATE_ADD:
			if(Japan_Dim_Start_Detect())
			{
				FLAG.light = 1;
				CURE_Dimming_Start(&STEPLESSDIMMING,TYPE_DIM);
			}
			add_cnt  ++;decent_cnt = 0;
			if(add_cnt >= 250){
				add_cnt = 0;temp --;//时间减少，逐渐变亮
				if(temp <= 50)
					temp -= 3;
				if(temp <= 0){temp = 0;BITREAD.sended = 1;}
				else BITREAD.sended = 0;
				dim->param = temp;
			}
				
		break;
		
		case DIM_STATE_SUB:
			if(Japan_Dim_Start_Detect())
			{
				FLAG.light = 1;
				CURE_Dimming_Start(&STEPLESSDIMMING,TYPE_DIM);
			}
			decent_cnt ++;add_cnt = 0;
			if(decent_cnt >= 250){
				decent_cnt = 0;temp ++;//时间增加，逐渐变暗
				if(temp <= 100)
					temp += 3;
				if(temp >= 700){temp = 700;BITREAD.sended = 1;}
				else BITREAD.sended = 0;
				dim->param = temp;
			}
		break;
		
		case DIM_STATE_GENERAL_ON:
			if(Japan_Dim_Start_Detect())
			{
				FLAG.light = 1;
				CURE_Dimming_Start(&STEPLESSDIMMING,TYPE_GENERAL);
				BITREAD.sended = 0;
			}
			
		break;
	}
}
void CURE_Dimming_TimerCount(Typedef_STEPLESSDIMMING *dim)
{
    //对亮度进行操作
    if(dim->param_flag == 1){               
        dim->param_count ++;
		if(dim->param_count >= dim->param){
			dim->param_count = 0;
			dim->param_flag = 0;
			dim->param_finish = 1;
		}
    }
}
void CURE_Dimming_Clear(Typedef_STEPLESSDIMMING *dim)
{
    dim->close = 0;
	dim->param_count = 0;
    dim->param_flag = 0;dim->param_last = 0;
	dim->func_running = 0;
    dim->flag = 0;dim->PLUS.plusEn = 0;dim->PLUS.int_close();dim->param_finish = 0;
    dim->PLUS.timer_close();
}

void TIM1_init(u8 pre,u32 tcmp)
{
    CLK->CLKSEL1 &= ~(7 << 12);     //清除定时器时钟源设置
    CLK->CLKSEL1 |= 2 << 12;        //把定时器1时钟源设置为HCLK      
    CLK->APBCLK |= 1 << 3;          //打开定时器1时钟
    TIMER1->TCSR &= ~(3 << 27);
    TIMER1->TCSR |= 1 << 27;        //周期模式   
    TIMER1->TCSR |= 1 << 29;        //开启中断    
    TIMER1->TCSR |= pre;            //预分频器设置
    TIMER1->TCMPR = tcmp;           //计数周期
    MY_NVIC_Init(TMR1_IRQn,0);      //设置中断优先级
}
static void TIM1_Open(void)
{
    TIMER1->TCSR |= 1 << 30; 
}
static void TIM1_Close(void)
{
    TIMER1->TCSR &= ~(1 << 30); 
}
void TMR1_IRQHandler(void)
{
    if(TIMER1->TISR & ( 1 << 0))
    {
        CURE_Dimming_TimerCount(&STEPLESSDIMMING);
        CURE_Dimming_ParamDetect(&STEPLESSDIMMING);
    }
    TIMER1->TISR |= 1 << 0;//清除标志位
}
/*************************************************************************************/
Typedef_BITREAD BITREAD = 
{
	{{{0},0},{{0},0},{{0},0},},
	0,
};
Typedef_FLAG FLAG = 
{
	0,
	
};
void Japan_Dim_Count(s8 i,u8 updn)
{
	if(updn == BIT_LOWLEVEL){
		BITREAD.bit[i].count[0] ++;
		BITREAD.bit[i].count[1] = 0;
		if(BITREAD.bit[i].count[0] >= 100){
			BITREAD.bit[i].count[0] = 0;
			BITREAD.bit[i].flag = 1;
		}
	}else{
		BITREAD.bit[i].count[1] ++;
		BITREAD.bit[i].count[0] = 0;
		if(BITREAD.bit[i].count[1] >= 100){
			BITREAD.bit[i].count[1] = 0;
			BITREAD.bit[i].flag = 0;
		}	
	}
}
void Japan_Dim_ReadBit(void)
{
	s8 i = 0;
	for(i = 0;i < 3;i ++){
		switch(i){
			case 2:
				BIT2_INPUT = 1;
				if(BIT2_INPUT == 0)
					Japan_Dim_Count(i,BIT_LOWLEVEL);
				else Japan_Dim_Count(i,BIT_HIGHLEVEL);
			break;
			case 1:
				BIT1_INPUT = 1;
				if(BIT1_INPUT == 0)
					Japan_Dim_Count(i,BIT_LOWLEVEL);
				else Japan_Dim_Count(i,BIT_HIGHLEVEL);
			break;
			case 0:
				BIT0_INPUT = 1;
				if(BIT0_INPUT == 0)
					Japan_Dim_Count(i,BIT_LOWLEVEL);
				else Japan_Dim_Count(i,BIT_HIGHLEVEL);
			break;
		}
	}
	for(i = 0;i < 3;i ++){
		if(BITREAD.bit[i].flag)
			BITREAD.state |= 1 << i;
		else
			BITREAD.state &= ~(1 << i);	
	}	
}
void Japan_Dim_Perpher_OutPut(void)
{
	if(BITREAD.sended)
		BIT3_OUTPUT = 1;
	else BIT3_OUTPUT = 0;
}
void DATAFLASH_init(u32 dfba_startaddr)
{
    DATAFLASH.init(dfba_startaddr);
    if(0XAC != DATAFLASH.read(OFFSET_FLAG,DF_PAGE1))
    {
        STEPLESSDIMMING.param = 10;
        DATAFLASH.erase(DF_PAGE1);
		DATAFLASH.write(DF_PAGE1+OFFSET_FLAG,0XAC);     
        DATAFLASH.write(DF_PAGE1+OFFSET_STEPLESSDIMMING,STEPLESSDIMMING.param);
    }
    else
    {
        STEPLESSDIMMING.param = DATAFLASH.read(OFFSET_STEPLESSDIMMING,DF_PAGE1);
    }
}
