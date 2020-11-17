
#include "exception_handler.c"
#include <stdio.h>
 
/*NOTE TO MAX AND MESELF
* HIJ DEED ALLEEN LEDS0tm8 want #define was een char* en een char is 8 bits
* veranderdt naar een int in simulatie resulteerde in alle leds werkend
*/

/*Hardware addresses defines*/
#define INTERVAL_TIMER (char *) 0x0002000
#define hex4_5 (int *) 0x0002020    //int is big enough for 28 bits, int is 32
#define hex0_3 (int *) 0x0002030
#define switches (volatile int *) 0x0002040
#define leds (int *) 0x0002050

/*Defines for instantion constitency*/
#define MAX_HEX_SIZE 6

/*Function definitions*/
void write7SegDisplay(char, int);
char* dec2ToHex(int);

/*Global variable declarations*/
volatile int intCounter = 0;

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
return 0x40 ;
}

void init_interrupt()
{
    volatile int * interval_timer_ptr = (int *)INTERVAL_TIMER;                    // interal timer base address

    /* set the interval timer period for scrolling the LED lights 
	/*Uitleg
	* Je pakt eerst de 16 meest rechter bits van de counter waarde en zet dat in register period_1 == offset 0x2
	* Daarna wil je in regsiter periodl de andere bovenste 16 bits van de counter erin zetten dus bitshift met 16
	* dit resulteert in een waarde van 25000000 totaal beide registers gecombineerd
	*/
	int counter					= 50000;	// 1/(50MHz) x (50000) = 1msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);			//This is the timer register bits 15:0
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;     //This is the timer register bits 31:16

    /* 	start interval timer, enable its interrupts
	/* 	start 	= 1 , enable interval timer
	*  	cont 	= 1 , enable continous running of the timer until the stop bit is set
	*	ito 	= 1 , enable interrupts for the timer
    */
   *(interval_timer_ptr + 1) = 0x7; // STOP = 0, START = 1, CONT = 1, ITO = 1

    /* set interrupt mask bits for levels 6 which is the defined interrupt generator for
    * the interval timer hardware
    */
    NIOS2_WRITE_IENABLE(0x40);
    NIOS2_WRITE_STATUS(1); // enable Nios II interrupts
    return;
}



//Zie testprogramma voor meer details
//Interrupt triggered by the interval timer
void counterInterrupt(void)
{
    volatile int * interval_timer_ptr = (int *)INTERVAL_TIMER;
    *(interval_timer_ptr) = 0;

    intCounter++;
    if(intCounter > 999999) intCounter = 0; //6 seven segments displays
    char *intCounterHex;
    intCounterHex = dec2ToHex(intCounter);

    //elke charachter uit intCounterHex staat gelijk aan een getal op de hex display
    int i = 0;
    //|| i > MAX_HEX_SIZE-1
    while(i < 6){
        write7SegDisplay(intCounterHex[i], i);
        i++;
    }
    return;
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
        *hex0_3 = (segInput);
    if(displayNumber == 1)
        *hex0_3 = ((segInput << 7) | *hex0_3);  
    if(displayNumber == 2)
        *hex0_3 = ((segInput << 14) | *hex0_3);
    if(displayNumber == 3)
        *hex0_3 = ((segInput << 21) | *hex0_3);
    if(displayNumber == 4)
        *hex4_5 = (segInput);
    if(displayNumber == 5)
        *hex4_5 = ((segInput << 7) | *hex4_5);
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
    *hex0_3 = 0;
    *hex4_5 = 0;
	init_interrupt();
    while(1);
}