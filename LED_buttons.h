#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

// function to initialize LED
void LED_init(void);

// function to initialize Buttons
void Buttons_init(void);

// define name for LED on the colour click
#define RED_LED LATGbits.LATG1 
#define GREEN_LED LATAbits.LATA4
#define BLUE_LED LATFbits.LATF7


// define name for buttons on the clicker 2 board
#define RF2_button !PORTFbits.RF2
#define RF3_button !PORTFbits.RF3


// define name for LED on Buggie
#define Right_Signal LATHbits.LATH0
#define Left_Signal LATFbits.LATF0
#define Break_Light LATDbits.LATD4
#define Beam_Light LATDbits.LATD3


#endif