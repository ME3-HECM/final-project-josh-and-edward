#ifndef MEMORY_H
#define	MEMORY_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


char WayBack [50]; // create list to store sequence of colour card
int Time_forward[50]; // create list to store straight line times
extern volatile unsigned int move_count=-1; // global variable for list index
int run_flag=1; // define variable for main loop

void go_Home (char *WayBack, int *Time_forward); // function to carry out return home instruction based on lists

#endif


