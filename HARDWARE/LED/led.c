#include "sys.h"
#include "clk.h"
#include "led.h"
void LED_init(void)
{
    P3->PMD&=~(3<<12);
    P3->PMD|=1<<12;
    P3->DOUT=1<<6;   
    
    
    
//     P5->PMD&=~(3<<10);
//     P5->PMD|=1<<10;
//     P5->DOUT=1<<5;
}
