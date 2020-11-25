/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The * 
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/

#include <stdio.h>
#include <system.h>
#include <string.h>
#include "includes.h"
#include "HAL\inc\sys\alt_irq.h"
#include "drivers\inc\altera_avalon_timer_regs.h"
#include "drivers\inc\altera_avalon_jtag_uart_regs.h"




/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    taskWriteHex0_3_stk[TASK_STACKSIZE];
OS_STK    taskWriteHex4_5_stk[TASK_STACKSIZE];

/*Defines of Task priorities*/
#define taskWriteHex0_3Priority 1
#define taskWriteHex4_5Priority 2

/*Function definitions*/
void write7SegDisplay(char, int);
void counterInterrupt(void* isr_context);
void jtagInterrupt (void* isr_context);
void checkStatus();
void taskWriteHex0_3(void* pdata);
void taskWriteHex4_5(void* pdata);
char* dec2ToHex(int);

/*Defines for instantion constitency*/
#define MAX_HEX_SIZE 6

/*Global variable declarations*/
volatile int intCounter = 0;
volatile int int1sCounter = 0;
volatile int noPara;
volatile int flags;
volatile int noPara1;
volatile int flags1;

volatile int hex0_3Counter;
volatile int hex4_5Counter;

volatile int runTimer = 1;	// AANPASSEN

//Convert a char hexadecimal character to its corresponding int value
int charToInt(char convert)
{
	switch(convert){
        case '0' : return 0; break;
        case '1' : return 1; break;
        case '2' : return 2; break;
        case '3' : return 3; break;
        case '4' : return 4; break;
        case '5' : return 5; break;
        case '6' : return 6; break;
        case '7' : return 7; break;
        case '8' : return 8; break;
        case '9' : return 9; break;
        case 'A' : return 10; break;
        case 'B' : return 11; break;
        case 'C' : return 12; break;
        case 'D' : return 13; break;
        case 'E' : return 14; break;
        case 'F' : return 15; break;
        default  : return 0;
    }
}
char* dec2ToHex(int decimalnum)
{
    int quotient, remainder;
    int j = 0;
    static char hexadecimalnum[MAX_HEX_SIZE];

    for(int i = 0; i < MAX_HEX_SIZE; i++)
        hexadecimalnum[i] = '0';

    quotient = decimalnum;

    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (remainder < 10)
            hexadecimalnum[j++] = 48 + remainder;
        else
            hexadecimalnum[j++] = 55 + remainder;
        quotient = quotient / 16;
    }
    return hexadecimalnum;
}

void init_interrupt(){
		/*Set interrupt for interval timer*/
		int counter	= 50000;	// 1/(50MHz) x (50000) = 1msec
		IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, counter & 0xFFFF);
		IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (counter >> 16) & 0xFFFF);

		//Set control register bits
		IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);
		//Set control register bits, enable Write interrupts
		IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, 0x2);

		void* isrPara  = (void *) &noPara;
		void* noFlags  = (void *) &flags;
		void* isrPara1 = (void *) &noPara1;
		void* noFlags1 = (void *) &flags1;
		//uitzoeken wat void isr_context moet zijn en de flags
		if(alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,TIMER_0_IRQ, counterInterrupt, isrPara, noFlags) != 0){
			printf("Error in initializing interval timer interrupt");
		}
		/*Set jtag_uart interrupt */
		if(alt_ic_isr_register(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,JTAG_UART_0_IRQ, jtagInterrupt, isrPara1, noFlags1) != 0){
			printf("Error in initializing jtag_uart interrupt");
		}

		alt_irq_cpu_enable_interrupts();
		alt_ic_irq_enable(JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID,jtagInterrupt);
		alt_ic_irq_enable(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, counterInterrupt);
}

void counterInterrupt(void* pdata){
	//checkStatus();
	if(runTimer == 1){
		if(((intCounter % 1000) == 0) && intCounter != 0){
			int1sCounter++;
			intCounter++;
			OSTaskSuspend(taskWriteHex0_3Priority);
			OSTaskResume(taskWriteHex4_5Priority);
		}else{
			intCounter++;
			OSTaskSuspend(taskWriteHex4_5Priority);
			OSTaskResume(taskWriteHex0_3Priority);
		}
	}else{
//		//sleep the threads so they cant increase counter;
//		OSTaskSuspend(taskWriteHex0_3Priority);
//		OSTaskSuspend(taskWriteHex4_5Priority);
	}
}



/* Prints "Hello World" and sleeps for three seconds */
void taskWriteHex0_3(void* pdata){
	int displaySelector = 0;
	while(1){
		displaySelector = 0;
		if(runTimer == 1){
			char *intCounterHex;
			volatile int * interval_timer_ptr = (int *)TIMER_0_BASE;
			*(interval_timer_ptr) = 0;

			if(intCounter > 65535) intCounter = 0;
			hex0_3Counter = intCounter;
			intCounterHex = dec2ToHex(intCounter);


			for(int i = 0; i < 4; i++){
				write7SegDisplay(intCounterHex[i], displaySelector);
				displaySelector++;
			}
		}
	}
	OSTaskSuspend(OS_PRIO_SELF);
}
void taskWriteHex4_5(void* pdata){
	int displaySelector = 4;
	while(1){
		displaySelector = 4;
		if(runTimer == 1){
			char *intCounterHex;
			volatile int * interval_timer_ptr = (int *)TIMER_0_BASE;
			*(interval_timer_ptr) = 0;

			if(int1sCounter > 255) int1sCounter = 0;
			intCounterHex = dec2ToHex(int1sCounter);

			for(int i = 0; i < 2; i++){
				write7SegDisplay(intCounterHex[i], displaySelector);
				displaySelector++;
			}
		}
	}
	OSTaskSuspend(OS_PRIO_SELF);
}

void jtagInterrupt(void* isr_context){
	char data;
	char completeData[10] = "";

	do{
		data = IORD_ALTERA_AVALON_JTAG_UART_DATA(JTAG_UART_0_BASE);
		if(data == '\n')
			break;
		strncat(completeData,&data,1);
	}while(data != '\n');

	if(!(strcmp (completeData,"start"))) //strcmp returns zero on succes
		runTimer = 1;
	if(!(strcmp (completeData,"stop")))
		runTimer = 0;
}

void write7SegDisplay(char displayValue, int displayNumber)
{
    int decNumber = charToInt(displayValue);

    volatile int* hexDisplay = (int *) REG32_AVALON_INTERFACE_0_AVALON_SLAVE_0_1_BASE;
    volatile int* hexDisplay4_5 = (int *) REG32_AVALON_INTERFACE_0_AVALON_SLAVE_0_2_BASE;

    //Write to the correct adress for a given display
    if(displayNumber == 0)
    	*(hexDisplay) = decNumber;
    if(displayNumber == 1)
    	*(hexDisplay) = (decNumber << 4) | *(hexDisplay);
    if(displayNumber == 2)
    	*(hexDisplay) = (decNumber << 8) | *(hexDisplay);
    if(displayNumber == 3)
    	*(hexDisplay) = (decNumber << 12) | *(hexDisplay);
    if(displayNumber == 4)
    	*(hexDisplay4_5) = (decNumber);
    if(displayNumber == 5)
    	*(hexDisplay4_5) = (decNumber << 4) | *(hexDisplay4_5);
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
  OSTaskCreateExt(taskWriteHex0_3,
		  	  	  NULL,
				  (void* ) &taskWriteHex0_3_stk[TASK_STACKSIZE-1],
				  taskWriteHex0_3Priority,
				  taskWriteHex0_3Priority,
				  &taskWriteHex0_3_stk[0],
				  TASK_STACKSIZE,
				  NULL,
				  OS_TASK_OPT_STK_CHK);
  OSTaskCreateExt(taskWriteHex4_5,
  		  	  	  NULL,
  				  (void* )&taskWriteHex4_5_stk[TASK_STACKSIZE-1],
				  taskWriteHex4_5Priority,
				  taskWriteHex4_5Priority,
				  &taskWriteHex4_5_stk[0],
  				  TASK_STACKSIZE,
  				  NULL,
				  OS_TASK_OPT_STK_CHK);
  OSStart();
  return 0;
}
