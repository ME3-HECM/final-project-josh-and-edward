#include <xc.h>
#include "LED_buttons.h"

//initDCmotorsPWM(200);

void LED_init(void) //initialize LEDs
{   
    
    // colour click LEDs
    TRISGbits.TRISG1 = 0; // Set TRIS value for red LED (output)
    TRISAbits.TRISA4 = 0; // Set TRIS value for green LED (output)
    TRISFbits.TRISF7 = 0; // Set TRIS value for blue LED (output)
    //turn on LEDs on colour click
    RED_LED=1; // sets RED LED on 
    GREEN_LED=1; // sets green LED on
    BLUE_LED=1; // sets Blue LED on
    
    
    // clicker 2 board LEDs RD7 and RH3
    LATDbits.LATD7=0;   //set initial output state
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)
    LATHbits.LATH3=0;   //set initial output state
    TRISHbits.TRISH3=0; //set TRIS value for pin (output)
    
    
    // Buggie LEDs
    //right signal lights RH0
    Right_Signal=0;   //set initial output state 
    TRISHbits.TRISH0=0; //set TRIS value for pin (output)
    //left signal lights RF0
    Left_Signal=0;   //set initial output state
    TRISFbits.TRISF0=0; //set TRIS value for pin (output)
    // break light RD4
    Break_Light=0;   //set initial output state
    TRISDbits.TRISD4=0; //set TRIS value for pin (output)
    
    //beam light RD3
    Beam_Light=0;   //set initial output state
    TRISDbits.TRISD3=0; //set TRIS value for pin (output)
    
}

void Buttons_init(void) //initialize Buttons
{   
    TRISFbits.TRISF2 = 1; // Set Tris value for RF2 Button (input)
    ANSELFbits.ANSELF2=0;
    TRISFbits.TRISF3 = 1; // Set Tris value for RF3 Button (input)
    ANSELFbits.ANSELF3=0;
}
