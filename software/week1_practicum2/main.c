#include <stdio.h>
#include <system.h>
#include "HAL\inc\sys\alt_irq.h"
#include "drivers\inc\altera_avalon_pio_regs.h"
#include "drivers\inc\altera_avalon_timer_regs.h"

/*Function definitions*/
void write7SegDisplay(char, int);
void counterInterrupt(void* isr_context);
char* dec2ToHex(int);


/*Defines for instantion constitency*/
#define MAX_HEX_SIZE 6

/*Global variable declarations*/
volatile int intCounter = 0;

//Convert a char hexadecimal character to its corresponding int value
int charToInt(char convert)
{
//    int result = convert - 48;
//    if((result <= 9 )&& (result >= 0)){
//    	return result;
//    }else{
//    	return (int)convert-55;	//if char is 'A' == 65 decimal so 65-55 is = 10 decimal is the decimal value of 0xA
//    }

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
//Return the hex display input bits to set a certain number on the display
int hex_to_7_seg (int hex_digit ) {
    if ( hex_digit == 0x0 ) return 0x40 ;
    if ( hex_digit == 0x1 ) return 0x79 ;
    if ( hex_digit == 0x2 ) return 0x24 ;
    if ( hex_digit == 0x3 ) return 0x30 ;
    if ( hex_digit == 0x4 ) return 0x19 ;
    if ( hex_digit == 0x5 ) return 0x12 ;
    if ( hex_digit == 0x6 ) return 0x02 ;
    if ( hex_digit == 0x7 ) return 0x78 ;
    if ( hex_digit == 0x8 ) return 0x00 ;
    if ( hex_digit == 0x9 ) return 0x18 ;
    if ( hex_digit == 0xA ) return 0x08 ;
    if ( hex_digit == 0xB ) return 0x03 ;
    if ( hex_digit == 0xC ) return 0x46 ;
    if ( hex_digit == 0xD ) return 0x21 ;
    if ( hex_digit == 0xE ) return 0x06 ;
    if ( hex_digit == 0xF ) return 0x0E ;
return 0x7F ;
}

volatile int noPara;
volatile int flags;

void init_interrupt(){
		/*Set callback function abnd enable interrupts*/
		//bits 15:0 periodl
		int counter	= 50000;	// 1/(50MHz) x (50000) = 1msec

		//Write counter value to registers
		IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, counter & 0xFFFF);
		IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (counter >> 16) & 0xFFFF);

		void* isrPara = (void *) &noPara;
		void* noFlags = (void *) &flags;
		//uitzoeken wat void isr_context moet zijn en de flags
		if(alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,TIMER_0_IRQ, counterInterrupt, isrPara, noFlags)){
		  printf("Error in ic_isr_register set callback");
		}

		//Set control register bits
		IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, 0x7);

		/* set interrupt mask bits for levels 6 which is the defined interrupt generator for
		* the interval timer hardware
		*/
		NIOS2_WRITE_IENABLE(0x40);

		NIOS2_WRITE_STATUS(1); // enable Nios II interrupts
}

/*Interrupt function */
void counterInterrupt(void* isr_context)
{
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
}

void write7SegDisplay(char displayValue, int displayNumber)
{
    int decNumber = charToInt(displayValue);
    int segInput = hex_to_7_seg(decNumber);

    //Write to the correct adress for a given display
    /* Uitleg
     * Het adres van hexY_X bevat een X aantal bits als voorbeeld hex0_3 = 28 bits
     * De hexto7Seg returned het getal wat nodig is om de goede bits van het hex display aan te sturen
     * Maar je wilt dit naar een specifiek gedeelte van de 28 bits schrijven, de 1e 7 = hex0 daarna hex1 etc. dit kan doormiddel van een mask(0x7F)
     */
    if(displayNumber == 0)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE, (segInput));
    if(displayNumber == 1)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE, ((segInput << 7) | IORD_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE)));
    if(displayNumber == 2)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE, ((segInput << 14) | IORD_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE)));
    if(displayNumber == 3)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE, ((segInput << 21) | IORD_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE)));
    if(displayNumber == 4)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX4_5_BASE, (segInput));
    if(displayNumber == 5)
    	IOWR_ALTERA_AVALON_PIO_DATA(HEX4_5_BASE, ((segInput << 7) | IORD_ALTERA_AVALON_PIO_DATA(HEX4_5_BASE)));
}

char* dec2ToHex(int decimalnum)
{
    int quotient, remainder;
    int i, j = 0;
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
  IOWR_ALTERA_AVALON_PIO_DATA(HEX0_3_BASE,0);
  IOWR_ALTERA_AVALON_PIO_DATA(HEX4_5_BASE,0);
  init_interrupt();
  int SW_value = IORD_ALTERA_AVALON_PIO_DATA ( SWITCHES_BASE ) ;
  IOWR_ALTERA_AVALON_PIO_DATA ( LEDS_BASE , SW_value ) ;
  while(1);
  return 0;
}
