#include <xc.h>
#include "colour_move.h"
#include "dc_motor.h"
#include "Memory.h"
#include "serial.h"
#include "color.h"

void colour_move (int recognized_colour) 
{

    //give move instruction based on returned colour
    if (recognized_colour==1){ //red
        RedMove(&motorL, &motorR); 
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=1; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==2){ //orange
        OrangeMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=2; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==3){ //yellow
        YellowMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=3; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==4){ //blue
        BlueMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=4; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==5){ //green
        GreenMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=5; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==6){ //light blue
        LightBlueMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=6; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if(recognized_colour==7){ //pink
        PinkMove(&motorL, &motorR);
        TMR0H=0; // reset timer values
        TMR0L=0;
        WayBack[move_count]=7; // store card colour in list
        lost_count=0; // recognized colour so reset lost count
    }
    else if (recognized_colour==10){// undecided colour
        lost_count++; // lost count increment when cannot read colour
        if (lost_count>=3){ 
            go_Home(WayBack, Time_forward); // return to starting point
        }
        else{
            RetryMove(&motorL, &motorR);
            TMR0H=0; // reset timer values
            TMR0L=0;
        }
    }
    else if (recognized_colour==0){ // recognized white card
        go_Home(WayBack, Time_forward); // go home
    }
}
