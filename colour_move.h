#ifndef COLOUR_MOVE_H
#define	COLOUR_MOVE_H

#include <xc.h> // include processor files - each processor file is guarded.  

int lost_count=0; // variable to detect whether car has lost in maze

void colour_move (int recognized_colour); // function to give instruction to car based on recognized colour, reset timer, store card colour

#endif	/* XC_HEADER_TEMPLATE_H */

