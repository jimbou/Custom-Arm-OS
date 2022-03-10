#include "utils.h"
#include "clib/printk.h"
#include "sched.h"
#include "peripherals/timer.h"

const unsigned int interval = 2000000;
unsigned int curVal = 0;

void timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void ) 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	printk("\t\tTimer interupt recieved:\tContext switch\n");
	timer_tick();
}
void disable_timer_irq( void ) 
{
	curVal = 0;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	printk("\nDisable timer interrupt\n");
}
