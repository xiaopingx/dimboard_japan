#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
struct TIM_Type
{
    void (*init)(u8,u32);
    u32 time0count; //��ʱ���ļ���ֻ��ÿ�ж�һ�ξͼ���һ��
    u32 cupcount[2];//��ˮ����
    u32 heatcount[2];
};
extern struct TIM_Type TIME_dev;
void TIM0_init(u8 pre,u32 tcmp);

#endif

