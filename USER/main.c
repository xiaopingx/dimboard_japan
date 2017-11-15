/*
                治疗机控制板
程序编写：杨向萍 胡小平  时间：2015-8-7
2015-08-08: 1、修改两个从机的从机上电气源互锁的状态
            2、加入WATER新的设置方式
2015-11-24：加入SPITE的补充功能
2015-11-26: 加入自检功能
2015-12-1 : 加入传感器检测功能
2016-1-5  : 修改串口通讯规则结束符中的0X0D                                                      By Shoopping
2016-1-6  : 加入DC 24V 36V通断的控制P5.1,由D键控制其通断                                        By Shoopping
2016-1-7  : 1、修改当传感器无法工作时，只执行简易型的功能                                       By Shoopping
            2、新加入地址为0X03的从机，即新型脚控功能                                           
            3、原椅子板重新设置为0x04号从机                                                     
2016-1-20 ：解决治疗机刚开始启动时，键盘板闪烁的问题                                            By Shoopping
2016-1-21 ：解决第三个键盘加入后的机椅互锁和起源互锁问题                                        By Shoopping
2016-1-23 : 1、修改在SET状态时，CUP闭合时也应打开继电器                                             
            2、在进入设置时，CUP,WATER,DC24V36V应关闭                                           
2016-1-30 : 在机椅互锁下，S1~S5按下时，蜂鸣器响，但不执行                                       By Shoopping
2016-3-5  : LIGHT为三档                                                                         By Shoopping
2016-3-9  : LIGHT为无极调光方式                                                                 By Shoopping
2016-3-28 : 修改在自检条件下，按SET键出现异常的情况                                             By Shoopping
2016-3-29 : 1、修改在S1~S5,RESET,LP,SP自动运行条件下按动SET键后直接进入SET状态                  By Shoopping
            2、修改在机椅互锁条件下，只能进入治疗机的SET状态,即手动、自动、自检状态不能进入     
            3、在SET高级状态(手动、自动、自检状态)，若出现机椅互锁，则退出SET状态，并停止运行   
2016-3-30 : 1、修改多键同时按下时，只有一个键有效                                               By Shoopping
            2、修改三块键盘不同键同时按下时，只有一块键盘的一个键有效                           
/___________________________________________________________________________________________________________/
2016-7-11 : 日本牙科椅


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
    u32 time=0;//控制发送信息到各从机的间隔
    MINI51_Clock_Init();
    delay_init(22); 
	TIM0_init(22-1,1000-1);             //TIMER0初始化
	CURE_Stepless_Dimming_Init();	
    DATAFLASH_init(DF_STARTADDR);       //芯片内部EEPROM初始化
    WATCHDOG_init(WDT_2POW16,IN_10K);   //打开看门狗，设置看门狗时钟为10KHz,最少时间为1.638S

    while(1)
    { 
        Japan_Dim_Start_ParamterVariable();
         CURE_Peripheral_Carryout();

		
         CURE_Dimming_Detect(&STEPLESSDIMMING);

        //主程序循环控制部分
        time++;
        if(time > 1000)
        {
            time=0;
            WATCHDOG_feeddog(); //喂看门狗
        }
        
    }//while结束   
}//main()结束
