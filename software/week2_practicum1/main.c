#include <stdio.h>
#include <system.h>
#include <string.h>
#include "HAL\inc\sys\alt_irq.h"
#include "drivers\inc\altera_avalon_timer_regs.h"
#include "drivers\inc\altera_avalon_jtag_uart_regs.h"

/*Function definitions*/
void write7SegDisplay(char, int);
void counterInterrupt(void* isr_context);
void jtagInterrupt (void* isr_context);
char* dec2ToHex(int);


/*Defines for instantion constitency*/
#define MAX_HEX_SIZE 6

/*Global variable declarations*/
volatile int intCounter = 0;
volatile int noPara;
volatile int flags;
volatile int noPara1;
volatile int flags1;

volatile int runTimer = 0;

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

void init_interrupt(){
		/*Set interrupt for interval timer*/
		int counter	= 50000;	// 1/(50MHz) x (50000) = 1msec
		IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, counter & 0xFFFF);
		IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (counter >> 16) & 0xFFFF);

		//Set control register bits
		IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);
		//Set control register bits, enable Write interrupts
		IOWR_ALTERA_AVALON_JTAG_UART_CONTROL(JTAG_UART_0_BASE, 0x2);

		void* isrPara = (void *) &noPara;
		void* noFlags = (void *) &flags;
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

/*Interrupt function */
void counterInterrupt(void* isr_context)
{
	volatile int * interval_timer_ptr = (int *)TIMER_0_BASE;
	*(interval_timer_ptr) = 0;

	if(runTimer == 1){
		intCounter++;
		if(intCounter > 999999) intCounter = 0; //6 seven segments displays
		char *intCounterHex;
		intCounterHex = dec2ToHex(intCounter);

		//elke charachter uit intCounterHex staat gelijk aan een getal op de hex display
		int i = 0;
		while(i < 6){
			write7SegDisplay(intCounterHex[i], i);
			i++;
		}
	}else if(runTimer == 0){
		intCounter = 0;
	}
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


int main()
{
  init_interrupt();

  while(1);
  return 0;
}
