#include <clib/string.h>
#include <clib/stdlib.h>
#include "utils.h"
#include <clib/printk.h>
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"
#include <sched.h>
#include <mini_uart.h>
#include "irq.h"
#include "fork.h"
#include "mm.h"
#include "sys.h"



/* String array, for the message of each process */
static char proc_args[NR_TASKS][10];

void cat(char *array)
{
  call_sys_cat(2);
}

void low_priority_process(char *array)
{
	char buf[2] = {0};
  sys_change_prior(LOW_PRIORITY);
  int count = 0;
	while (count < 5){
		for (int i = 0; i < 5; i++){
			buf[0] = array[i];
			call_sys_write(buf);
			delay(1000000);
		}
    count++;
	}
	call_sys_exit();
}

void middle_priority_process(char *array)
{
  char buf[2] = {0};
  sys_change_prior(MIDDLE_PRIORITY);

  int count = 0;
	while (count < 5){
		for (int i = 0; i < 5; i++){
			buf[0] = array[i];
			call_sys_write(buf);
			delay(1000000);
		}
    count++;
	}
	call_sys_exit();
}

void high_priority_process(char *array)
{
  printk("\nHigh priority process\n");

  char buf[2] = {0};
  sys_change_prior(HIGH_PRIORITY);
  int count = 0;
	while (count < 5){
		for (int i = 0; i < 5; i++){
			buf[0] = array[i];
			call_sys_write(buf);
			delay(1000000);
		}
    count++;
	}
  printk("\nEnd of high priority process\n");
	call_sys_exit();
}

void kill_processes()
{
	preempt_disable();

	struct task_struct *p;

	for (int i = 0; i < NR_TASKS; i++) {
		p = task[i];
		if (p != 0 && i != 0) {
            free_page((unsigned long) p);
			nr_tasks--;
			task[i] = 0;

		}
	}

}
void user_process(){
	
	printk("\nUser processes started \n");
	unsigned long stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 3\n\r");
		return;
	}
	int err = call_sys_clone((unsigned long)&low_priority_process, (unsigned long)"12345", stack);
	if (err < 0){
		printk("Error while clonning process 2\n\r");
		return;
	} 

  stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
  err = call_sys_clone((unsigned long)&high_priority_process, (unsigned long)"zqrty", stack);
	if (err < 0){
		printk("Error while clonning process 1\n\r");
		return;
	} 
	
  stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
	err = call_sys_clone((unsigned long)&middle_priority_process, (unsigned long)"abcde", stack);
	if (err < 0){
		printk("Error while clonning process 2\n\r");
		return;
	} 
  stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
	err = call_sys_clone((unsigned long)&middle_priority_process, (unsigned long)"rtyui", stack);
	if (err < 0){
		printk("Error while clonning process 2\n\r");
		return;
	} 
	call_sys_exit();
}

void kernel_process(){
	printk("\nKernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((unsigned long)&user_process);
	if (err < 0){
		printk("Error while moving process to user mode\n\r");
  }
}

void user_process_cat(){
	
	printk("\nUset cat process started \n");
	unsigned long stack = call_sys_malloc();
	if (stack < 0) {
		printk("Error while allocating stack for process 1\n\r");
		return;
	}
	int err = call_sys_clone((unsigned long)&cat, (unsigned long)"12345", stack);
	if (err < 0){
		printk("Error while clonning process 1\n\r");
		return;
	} 
	call_sys_exit();
}

void kernel_process_cat(){
	printk("\nKernel cat process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((unsigned long)&user_process_cat);
	if (err < 0){
		printk("Error while moving process to user mode\n\r");
  }
}
