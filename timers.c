#include <xc.h>
#include "timers.h"
#include "interrupts.h"
#include "Memory.h"
/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1101; // 1:8192 // 0.512ms interval
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // it's a good idea to initialise the timer registers so we know we are at 0
    TMR0H=0;            //write High reg first, update happens when low reg is written to
    TMR0L=0; 
    T0CON0bits.T0EN=0;	//start the timer
}

/************************************
 * Function to return the full 8 most significant bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
void getTMR0val(void)
{
    unsigned int temp= TMR0L;
    //disregard the TMR0L
    int moving=(TMR0H<<8)|(temp&0xff);
    if (moving>500){ // minus 250 ms to avoid hitting into the wall
        moving=moving-500;
    }
    
    Time_forward[move_count]=moving;
    // function to input TMR0H into the array
    
	//add your code to get the full 16 bit timer value here
}

// custom delay function to use timer value
void delayed_ms(int time){
    for(unsigned int i=0;i<time;i++){
        __delay_us(500);
    }
}
