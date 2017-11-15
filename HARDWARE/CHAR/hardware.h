#ifndef __HARDWARE_CHAR_H
#define __HARDWARE_CHAR_H
#include "sys.h"
#include "gpio.h"
#define BEEP P06

#define LIGHT_SPITE             (1<<0)
#define LIGHT_XHD               (1<<1)
#define LIGHT_A                 (1<<2)
#define LIGHT_B                 (1<<3)
#define LIGHT_C                 (1<<4)
#define LIGHT_CUP               (1<<8)
#define LIGHT_WATER             (1<<9)
#define LIGHT_HEAT              (1<<10)
#define LIGHT_S0                (1<<11)
#define LIGHT_SET               (1<<12)
#define LIGHT_S1                (1<<16)
#define LIGHT_S2                (1<<17)
#define LIGHT_S3                (1<<18)
#define LIGHT_WATCH             (1<<19)
#define LIGHT_LIGHT             (1<<20)

/***********************************************/
#define WATCH_DRIVER            P55 //��Ƭ��
#define WATER_DRIVER            P52 //����
#define HEAT_DRIVER             P22 //���ȣ����
#define CUP_DRIVER              P23 //��ˮ
//*************************************************
#define LIGHT_L_DRIVER          P07 //���Ƶ͵�
#define LIGHT_M_DRIVER          P26 //�����е�
#define LIGHT_H_DRIVER          P05 //���Ƹߵ�
#define LIGHT_STEPLESS_DIMMING  P05//���� �޼�����
//*************************************************
#define LIGHT_SAMPLE_DRIVER     P07 //ֻ��һ��������

#define WK                      P36 //�¶ȴ�����������
#define DC24V36V_DRIVER         P51 //ֱ��24V 36Vͨ�Ͽ���
/***********************************************/
//��FLASH�е�ƫ��λ��
#define OFFSET_CUP              0X00
#define OFFSET_WATER            0X04
#define OFFSET_LIGHT            0X08
#define OFFSET_ABCDZ            0X0C 
#define OFFSET_DC24V36V         0X10
//�޼�����
#define OFFSET_STEPLESSDIMMING  0X14
//��־
#define OFFSET_FLAG             100
/***********************************************/
#define BEEP_WORKTIME           150
#define BEEP_STOPTIME           300
/***********************************************/
#define CUP_SETTED              1
#define WATER_SETTED            2
#define LIGHT_SETTED            3
#define S1_SETTED               4
#define S2_SETTED               5
#define S3_SETTED               6
#define S4_SETTED               7
#define S5_SETTED               8
#define DC24V36V_SETTED         9
/****************************************************/
// #define CUPMAX                  30000
#define CUPMAX                  120000
#define DC24V36VMAX             120000
/****************************************************/
#define POS_UP                  0
#define POS_DN                  1
#define POS_BU                  2
#define POS_BD                  3
#define POS_PRG                 4
#define POS_CUP                 5
#define POS_WATER               6
#define POS_HEAT                7

#define POS_WATCH               8
#define POS_LIGHT               9
#define POS_S1                  10
#define POS_S2                  11
#define POS_S3                  12
#define POS_S4                  13
#define POS_S5                  14
#define POS_SET                 15

#define POS_RESET               16
#define POS_LP                  17
#define POS_SP                  18
#define POS_POWER               19
#define POS_A                   20
#define POS_B                   21
#define POS_C                   22
#define POS_D                   23

#define POS_E                   24
#define POS_LOCK_GAS            25  //��Դ����
#define POS_LOCK_CC             26  //���λ���
#define POS_AUTOSTOP_SET        27
#define POS_FOOT2_LP            28
#define POS_FOOT2_SP            29
#define POS_FOOT2_CUP           30
#define POS_FOOT2_WATER         31
/****************************************************/
#define KEY_UP                  0
#define KEY_DOWN                1
#define LIGHT_CLEAR             0
#define LIGHT_RANK1             1
#define LIGHT_RANK2             2
#define LIGHT_RANK3             3
/****************************************************/
#define KEYBOARD_1              1
#define KEYBOARD_2              2
#define KEYBOARD_3              3
/****************************************************/
//JZ:��ֵ��Чֵ
//JZ0:��ֵ��ǰֵ
//JZ1:��ֵǰһ��ֵ
typedef struct
{ 
    u32 JZ;
    u32 DZ;
    u8 RS485REV[15];
    u8 RS485SEND[15];
    u8 addr_count;
    u8 addr_err;
    u32 temp_dz;
}TypeDef_PRO;
extern TypeDef_PRO PRO_dev;
/********************************************/
typedef struct
{
    u8 up;
    u8 cup;
    u8 s1;
    u8 spite;
    u8 dn;
    u8 water;
    u8 s2;
    /************************/
    u8 bu;
    u8 heat;
    u8 s3;
    u8 bd;
    u8 watch;
    u8 s4;
    u8 s5;
    /************************/
    u8 prg;
    u8 set;
    u8 light;
    /************************/
    u8 cupset;
    u8 cupsetted;
    u8 waterset;
    u8 watersetted;
    /************************/
    u8 s1setted;
    u8 s2setted;
    u8 s3setted;
    u8 s4setted;
    u8 s5setted;
    /************************/
    u8 keyboard1;   //�Ӽ���1���յ����ݵı�־
    u8 keyboard2;   //�Ӽ���2���յ����ݵı�־
    u8 chairboard;  //�����Ӱ���յ����ݵı�־
    u8 rec_board;   //��¼���ĸ����ӷ��͹������������
    /************************/
    u8 lock_gas1;
    u8 lock_gas2;
    u8 lock_cc1;
    u8 lock_cc2;
    u8 lock_gas;
    u8 lock_cc;
    /************************/
    u8 reset;
    u8 lp;
	u8 power;
    /************************/
    u8 dc24v36v;
    u8 dc24v36vsetted;
}TypeDef_FLAG;
extern TypeDef_FLAG FLAG;
/********************************************/
typedef struct
{
    u32 cup;
    u32 water;
}TypeDef_TIMEPARAM;
extern TypeDef_TIMEPARAM TIMEPARAM;
/********************************************/
typedef struct
{
    void (*start)(u8,u32,u32);
    void (*stop)(u8);
    u8 beepflag;            //�������Ƿ�����־������Ϊ��ʱ�ͷ������Ƿ������ж�
    u32 beepcount[2];       //���������� ��λΪms
    u8 beeptimes;           //�������ܹ���Ҫ��Ĵ���
    u8 beeptimescounter;    //��������Ĵ�������
    u8 beepcounterflag;
    u32 worktime;
    u32 stoptime;
}TypeDef_BEEP;
extern TypeDef_BEEP BEEP_dev;
/********************************************/
typedef struct
{
    void (*up)(void);       //0
    void (*dn)(void);       //1  
    void (*bu)(void);       //2
    void (*bd)(void);       //3
    void (*prg)(void);      //4
    void (*cup)(void);      //5
    void (*water)(void);    //6
    void (*heat)(void);     //7
    void (*watch)(void);    //8
    void (*light)(void);    //9
    void (*s1)(void);       //10
    void (*s2)(void);       //11
    void (*s3)(void);       //12
    void (*s4)(void);       //13
    void (*s5)(void);       //14
    void (*set)(void);      //15
    void (*reset)(void);    //16
    void (*lp)(void);       //17
    void (*sp)(void);       //18
    void (*power)(void);    //19
    void (*a)(void);        //20
    void (*b)(void);        //21
    void (*c)(void);        //22
    void (*d)(void);        //23
    void (*e)(void);        //24 
    void (*lock_gas)(void); //25,��Դ����
    void (*lock_cc)(void);  //26�����λ���
}TypeDef_KEYFUNC;
extern TypeDef_KEYFUNC KEYFUNC;
/********************************************/
typedef struct
{
    u32 DZ;//�����Ӱ巢�͹����ĵ�ֵ
    u32 JZ;//�����ƻ���Ҫ���͵����Ӱ�ļ�ֵ
}TypeDef_CHAIR;
extern TypeDef_CHAIR CHAIR;
/********************************************/
typedef struct
{
    u8 heated;              //�����Ƿ����
    u8 light_interval;      //���ȵƼ�϶��˸
    u32 delaycount;         //��ʱ
    u8 delayfinish;         //��ʱ���
    u8 control;             //���ȹ��̵Ŀ��Ʊ�־
    u8 count;
    u16 first;
}TypeDef_HEATED;
extern TypeDef_HEATED HEATED;
/********************************************/
typedef struct
{
    u8 lock;        //��
    u32 counter;    //����
    u8 unlock;      //����
    u8 start;
    u8 effective;
    u8 gased;
    u8 effective_count;
}TypeDef_LOCK;
extern TypeDef_LOCK LOCK;
/********************************************/

/********************************************/
typedef struct
{
    u8 type;//waterʹ�õ�����
    u32 watercount[2];
    u8 watersettimes;
    u8 start;
    u32 min;
    u32 sec;
    u8 setttimes;
    u8 work;
    u8 unlimit;
}TypeDef_WATER;
extern TypeDef_WATER WATER;
/********************************************/
typedef struct
{
    u8 priority;    //�������ȼ�
    u8 start;       //������ʼ
    u32 count[2];   //����
    u8 prio_pre;    //
    u8 manual;      //�ֶ����ü���
    u8 stage_in;
    u8 stage_out;
    u8 fail;
    u8 chairprio;   //���Ӱ���������ȼ�״̬
}TypeDef_SET;
extern TypeDef_SET SET;
/********************************************/
typedef struct
{
    u32 time;
    u32 timecount[2];
    u8 setted;
    
    
}TypeDef_DC24V36V;
extern TypeDef_DC24V36V DC24V36V;

typedef struct
{
    u8 flag;//��ʼ��ʱ
    u16 counter;
    /*
    step
    1:���������
    2:�л���ʱ��
    */
    u8 step;
}TypeDef_LIGHTRANK3_SWITCHDELAY;

typedef struct
{
    u8 rank;
    u8 ranklast;
    u16 timecounter;
    u8 temp;
    u8 keylightflag;
    u8 stop;
    TypeDef_LIGHTRANK3_SWITCHDELAY RANK3_SWITCHDELAY;
}TypeDef_CURELIGHT;
extern TypeDef_CURELIGHT CURELIGHT;
/********************************************************/

typedef struct
{
    u8 keydown;
}Typedef_KEYBOARDJZ;
typedef struct
{
    Typedef_KEYBOARDJZ KEYBOARD1;
    Typedef_KEYBOARDJZ KEYBOARD2;
    Typedef_KEYBOARDJZ KEYBOARD3;
}Typedef_KEYBOARD;
extern Typedef_KEYBOARD KEYBOARD;
/********************************************************/
void Hardware_init(void);
void CURE_DC24V36V_init(void);
void KEY_deal(void);
void DATAFLASH_init(u32 dfba_startaddr);
void CURE_Peripheral_Carryout(void);
void CUREBOARD_FirstTime(u8 type);
void CUREBOARD_FirstBeep(void);
void CUREBOARD_FirstToChair(void);
void CURE_Heat_Detect(void);
void LIGHT_Deal(void);
void CURE_Work_Run(void);
void CURE_ExitSet_SaveNone(u8 Position);
void CURE_Communication_Send(u8 addr);
u8 CURE_Communication_Recieved(void);
u8 CURE_Communication_Rec_Sample(void);
void CURE_LOCK_Detect(void);
void CURE_LOCK_Deal(void);
void CURE_LOCK_Mix(u32 *temp);
void CURE_WATER_Scan(void);
void CURE_WATER_start(void);
void CURE_WATER_ReCount(void);
void CURE_WATER_Close(void);
u8 CURE_KeyNum_Detect(u8 board,Typedef_KEYBOARD *keyboard,u32 jz);
u8 CURE_KeyBoard_ValueDetect(u8 board,Typedef_KEYBOARD *keyboard);
void CURE_1RANK_Deal(u8 keyupdn);
void CURE_3TYPE_LIGHT_Init(void);
void CURE_3RANK_Deal(TypeDef_CURELIGHT *light,u8 keyupdn);
void CURE_3RANK_Clear(TypeDef_CURELIGHT *light);
void CURE_3RANK_ExitDetect(TypeDef_CURELIGHT *light);
#endif

