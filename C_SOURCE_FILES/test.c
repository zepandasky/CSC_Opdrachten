#include <stdio.h>


/*  TESTPROGRAMMA 
*   CONVERTEERT DECIMAAL NAAR HEXADECIMAAL EN PRINT DIT UIT
*   IN WSL GETEST MET GCC 9.3.0
*/

#define MAX_HEX_SIZE 100

int intCounter = 10;

char* dec2ToHex(int);

void counterInterrupt()
{
    intCounter++;
    if(intCounter > 999999) intCounter = 0; //6 seven segments displays
    char *intCounterHex;
    intCounterHex = dec2ToHex(intCounter);
    
    for(int i = 99 ;i >= 0 ; i--)
        printf("%c", intCounterHex[i]);
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

void main()
{ 
	counterInterrupt();
}