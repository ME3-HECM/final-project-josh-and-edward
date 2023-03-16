#ifndef _interrupts_H
#define _interrupts_H


#include <xc.h>

#define _XTAL_FREQ 64000000

void Interrupts_init(void); // initialize interrupt
void __interrupt(high_priority) HighISR(); // High priority interrupt service routine

int lost_flag=0; // define variable for showing when buggie is lost

#endif
