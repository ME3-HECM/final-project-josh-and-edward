// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

// include header files
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

struct RGB_rel rel; // structure for colour click readings
struct RGB vals; // structure for colour click readings
volatile unsigned int move_count;  // variable to keep track of list index
void main(void) {
    initUSART4(); 
    Interrupts_init(); // initialize timer interrupt
    color_click_init(); // initialize colour click
    I2C_2_Master_Init(); // initialize serial communication
    LED_init(); // initialize LEDs
    Buttons_init(); // initialize buttons
    initDCmotorsPWM(200); // initialize dc motors
    Timer0_init(); // initialize timer
    
    
    // define structure for dc motor parameters
    // left motors
    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=200; 			//store PWMperiod for motor (value of T2PR in this case)
    
    // right motors
    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    motorR.PWMperiod=200; 			//store PWMperiod for motor (value of T2PR in this case)
    
    char buf[100];
    
    turnCalibration(&motorL,&motorR); // function to enter the motor calibration procedure
    
    Left_Signal=0;  // turn off left signal to show successfully exit calibration code
    __delay_ms(1000);
    
    while (!RF2_button); // wait for RF2 button press to start the maze
    __delay_ms(1000);
    
    T0CON0bits.T0EN=1; // start timer
    while (run_flag) // loop for exploring maze
    {
        consecuitive=0;
        
        fullSpeedAhead(&motorL,&motorR);
        // read the colours and store it in the struct vals
        readColours(&vals);

        // obtain the relative RGB values and store it in the struct RGB_rel vals

        // if the clear value is greater than 2500 (value obtained from lowest clear value card which was blue) then it has hit a wall so detect what colour it sees
        if (vals.L>=500){ // when detected a card
            move_count++; // increment index of move and timer arrays
            getTMR0val(); // place time moving forward in time array
            if (move_count>98){ // let buggie go home when too many steps
                go_Home(WayBack, Time_forward);
            }
            else{
                Forwardhalfblock(&motorL,&motorR); // go forward to make sure colour click against card
                while (consecuitive<20){ // loop to get 20 same consecutive colour recognition before confirm card colour
                    __delay_ms(100);
                    readColours(&vals); // get readings from colour click
                    colour_rel(&vals, &rel); // calculate relative value of readings
                    int colour = Colour_decider(&vals, &rel); // store current colour reading in variable colour
                    if (colour==prev_colour){
                        consecuitive++; // variable increment for same consecutive readings
                    }
                    else{
                        consecuitive=0; // reset consecutive count when get different reading
                    }
                    prev_colour=colour; //confirm card colour
                }

                // serial communication for testing
                sprintf(buf,"red=%f green=%f blue=%f lum=%d actual_colour=%d \r\n",rel.R, rel.G,rel.B,vals.L, prev_colour);
                sendStringSerial4(buf); // send variable to serial port

                colour_move (prev_colour); // give buggie move instruction based on recognized colour
            }
        
        }else if (lost_flag){ // loop for when timer overflows, meaning car is lost
            move_count++; // increment index of move and timer arrays
            Time_forward[move_count]=65535; // as timer overflow amount so need to retravel this ammount in a straight line to go home
            go_Home(WayBack, Time_forward); // go back to starting point in maze

        }
    }
}

