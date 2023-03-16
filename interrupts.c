#include <xc.h>
#include "interrupts.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
    INTCONbits.IPEN=0;
    INTCONbits.PEIE=0;
    INTCONbits.GIE=1;//turn on interrupts globally (when this is off, all interrupts are deactivated)
    PIE0bits.TMR0IE=1;
//	// turn on global interrupts, peripheral interrupts and the interrupt source 
//	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.TMR0IF){ // timer overflow interrupt
        TMR0H=0; // reset timer
        TMR0L=0;
        lost_flag=1; // set lost flag to trigger go home function 
        PIR0bits.TMR0IF=0; // reset overflow interrupt
    }

}