#include "Memory.h"
#include "dc_motor.h"
#include "timers.h"
void go_Home (char *WayBack, int *Time_forward){
    int i;
    BlueMove(&motorL, &motorR);
    T0CON0bits.T0EN=0;
    for (i = move_count; i >= 0; i--){
        timed_forward(&motorL, &motorR,Time_forward[i]);
            
        if (WayBack[i-1]==1){
            //homeReverse(&motorL, &motorR);
            GreenMove(&motorL, &motorR); // opposite of red move
            Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==2){
            //homeReverse(&motorL, &motorR);
            ReverseOrangeMove(&motorL, &motorR); // opposite of orange move
            Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==3){
            //homeReverse(&motorL, &motorR);
            ReverseYellow(&motorL, &motorR); // opposite of yellow move
            Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==4){
            homeReverse(&motorL, &motorR);
            BlueMove(&motorL, &motorR); // 180 degrees same either way
            //Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==5){
            //homeReverse(&motorL, &motorR);
            RedMove(&motorL, &motorR); //opposite of green move
            Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==6){
            //homeReverse(&motorL, &motorR);
            ReverseLightBlueMove(&motorL, &motorR); // opposite of light blue move
            Backhalfblock(&motorL, &motorR);
        }
        else if (WayBack[i-1]==7){
            //homeReverse(&motorL, &motorR);
            ReversePink(&motorL, &motorR);
            Backhalfblock(&motorL, &motorR);
        }
        
    } 
    stop(&motorL, &motorR); // stop car
    run_flag=0; // set run flat to 0 when returned home
}