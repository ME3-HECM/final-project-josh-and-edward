// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "serial.h"
#include "color.h"
#include "i2c.h"
#include "interrupts.h"
#include "dc_motor.h"
#include "Memory.h"
#include "timers.h"
#include "LED_buttons.h"
#include "colour_move.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

struct RGB_rel rel;
struct RGB vals;
volatile unsigned int move_count;  // variable to keep track of list index
void main(void) {
    initUSART4(); 
    Interrupts_init();
    color_click_init();
    I2C_2_Master_Init();
    LED_init(); // initialize LEDs
    Buttons_init(); // initialize buttons
    initDCmotorsPWM(200);
    Timer0_init();
    
    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=200; 			//store PWMperiod for motor (value of T2PR in this case)
    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    motorR.PWMperiod=200; 			//store PWMperiod for motor (value of T2PR in this case)
    
    char buf[100];
    
    turnCalibration(&motorL,&motorR);
    
    Left_Signal=0;  // turn off left signal to show exit calibration code
    __delay_ms(1000);
    
    while (!RF2_button); // PORTFbits.RF2
    __delay_ms(1000);
    
    T0CON0bits.T0EN=1; // start timer
    while (run_flag)
    {
        consecuitive=0;
        
        fullSpeedAhead(&motorL,&motorR);
        // read the colours and store it in the struct vals
        readColours(&vals);

        // obtain the relative RGB values and store it in the struct RGB_rel vals

        // if the clear value is greater than 2500 (value obtained from lowest clear value card which was blue) then it has hit a wall so detect what colour it sees
        if (vals.L>=500){
            move_count++; // increment index of move and timer arrays
            getTMR0val();// place time moving forward in time array
            if (move_count>98){
                go_Home(WayBack, Time_forward);
            }
            else{
                Forwardhalfblock(&motorL,&motorR);
                while (consecuitive<20){
                    __delay_ms(100);
                    readColours(&vals);
                    colour_rel(&vals, &rel);
                    int colour = Colour_decider(&vals, &rel);
                    if (colour==prev_colour){
                        consecuitive++;
                    }
                    else{
                        consecuitive=0;
                    }
                    prev_colour=colour;
                }

                // serial communication for testing
                sprintf(buf,"red=%f green=%f blue=%f lum=%d actual_colour=%d \r\n",rel.R, rel.G,rel.B,vals.L, prev_colour);
                sendStringSerial4(buf);

                colour_move (prev_colour); // give buggie move instruction based on recognized colour
            }
        }else if (lost_flag){
            move_count++; // increment index of move and timer arrays
            Time_forward[move_count]=65535; // as timer overflow ammount so need to retravel this ammount in a straight line to go home
            go_Home(WayBack, Time_forward);

        }
    }
}

