#include <xc.h>
#include "dc_motor.h"
#include "timers.h"
#include "color.h"
#include "LED_buttons.h"
// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6
    
    //configure tris register for output
    TRISEbits.TRISE2=0; //set TRIS value for pin (output)
    TRISEbits.TRISE4=0; //set TRIS value for pin (output)
    TRISCbits.TRISC7=0; //set TRIS value for pin (output)
    TRISGbits.TRISG6=0; //set TRIS value for pin (output)
    

    // timer 2 config
    T2CONbits.CKPS=0b011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=PWMperiod; //Period reg 10kHz base period // timer reset when count equals 200 to have period register of 10^-4
    T2CONbits.ON=1; //turn on timer
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
    
    


}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) { //changed original code to make car go forward when direction=1
        
        *(m->posDutyHighByte)=posDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=negDuty;
  
    } else {
        *(m->posDutyHighByte)=negDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=posDuty;     
    }
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL,struct DC_motor *mR)
{
    while (mL->power >0 || mR->power >0){ // loop to gradually increase power of motor

        mL->power-=10; // power change in each loop
        mR->power-=10;

        setMotorPWM(mL); // send PWM signal to motor
        setMotorPWM(mR);
        
        __delay_ms(20);
    }
}

//function to make the robot turn left // with gradual start
void turnLeft(struct DC_motor *mL,struct DC_motor *mR)
{
    mL->direction =0; // set direction of motor
    mR->direction =1;
    
    while(mL->power<power && mR->power<power){ // loop to gradually increase power of motor

        
        mL->power+=10; // power change in each loop
        mR->power+=10;
        
        setMotorPWM(mL);// send PWM signal to motor
        setMotorPWM(mR);
        
        __delay_ms(20);
    }
}

//function to make the robot turn right 
void turnRight(struct DC_motor *mL,struct DC_motor *mR)
{
    mL->direction =1; // set direction of motor
    mR->direction =0;
    
    while(mL->power<power && mR->power<power){
        
        mL->power+=10; // power change in each loop
        mR->power+=10;
        
        setMotorPWM(mL); // send PWM signal to motor
        setMotorPWM(mR);
        
        __delay_ms(20);
    }
}

//function to make the robot go straight forward
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{

    mL->direction =1; // set direction of motor
    mR->direction =1;
    while (mL->power < power && mR->power < power){

        mL->power+=10; // power change in each loop
        mR->power+=10;

        setMotorPWM(mL); // send PWM signal to motor
        setMotorPWM(mR);
        
        
        __delay_ms(20);
    }

}
void timed_forward(struct DC_motor *mL, struct DC_motor *mR, int time)
{

    mL->direction =1; // set direction of motor
    mR->direction =1;
    while (mL->power < power && mR->power < power){

        mL->power+=10; // power change in each loop
        mR->power+=10;

        setMotorPWM(mL); // send PWM signal to motor
        setMotorPWM(mR);
        
        __delay_ms(20);
    }
    delayed_ms(time);
}

//function to make the robot go straight backward
void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR)
{

    mL->direction =0; // set direction of motor
    mR->direction =0;
    while (mL->power < power && mR->power < power){

        mL->power+=10; // power change in each loop
        mR->power+=10;

        setMotorPWM(mL); // send PWM signal to motor
        setMotorPWM(mR);
        
        __delay_ms(20);
    }
}

// custom delay function to allow changing delay time to change turn angles
void TurnDelay(Turn45Delay){
    while (Turn45Delay>0){
        __delay_ms(1); 
        Turn45Delay--; // decrease delay variable
    }
}

// function to let car turn right 45 degrees
void turnRight45(struct DC_motor *mL,struct DC_motor *mR){
    turnRight(mL,mR);
    TurnDelay(Turn45Delay);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car turn left 45 degrees
void turnLeft45(struct DC_motor *mL,struct DC_motor *mR){
    turnLeft(mL,mR);
    TurnDelay(Turn45Delay);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50);
}

// function to let car reverse after reading card
void reverseDetect(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedBack(mL,mR); // let car run backward
    __delay_ms(200);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}


void homeReverse(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedBack(mL,mR); // let car run backward
    __delay_ms(10);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car reverse one block
void reverseOneBlock(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedBack(mL,mR); // let car run forward
    __delay_ms(RunOneBlockTime); // reverse time period needs to be calibrated
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car forward one block
void ForwardOneBlock(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedAhead(mL,mR);  // let car run forward
    __delay_ms(RunOneBlockTime); // reverse time period needs to be calibrated
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car forward half block
void Forwardhalfblock(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedAhead(mL,mR); // let car run forward
    __delay_ms(RunOneBlockTime/2); 
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car forward 1/4 block
void quaterForward(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedAhead(mL,mR); // let car run forward
    __delay_ms(300);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}

// function to let car reverse half one block
void Backhalfblock(struct DC_motor *mL,struct DC_motor *mR){
    fullSpeedBack(mL,mR); // let car run backward
    __delay_ms(RunOneBlockTime/2);
    stop(&motorL, &motorR); // stop motors
    __delay_ms(50); 
}


// one line instruction for each colour ********************************************

// red move instruction Turn Right 90
void RedMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn right 90 degrees
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
}

// green move instruction Turn Left 90
void GreenMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn left 90 degree
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
}

// blue move instruction Turn 180
void BlueMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn 180 degree
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    
    Backhalfblock(&motorL, &motorR); // reverse against wall for alignment
}

// yellow move instruction Reverse 1 square and turn right 90
void YellowMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    //reverse one square
    reverseOneBlock(&motorL, &motorR);
    // turn right 90 degree
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
}

// pink move instruction Reverse 1 square and turn left 90
void PinkMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    //reverse one square
    reverseOneBlock(&motorL, &motorR);
    // turn left 90 degree
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
}

// orange move instruction Turn Right 135
void OrangeMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn right 135 degree
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    //move forward 1/4 block
    quaterForward(&motorL, &motorR);
}

// light blue move instruction 	Turn Left 135
void LightBlueMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn left 135 degree
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    //move forward 1/4 block
    quaterForward(&motorL, &motorR);
}

void RetryMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR); // reverse time period needs to be calibrated
    stop(&motorL, &motorR);
    fullSpeedAhead(mL,mR);
    __delay_ms(500); // reverse time period needs to be calibrated
    stop(&motorL, &motorR);
}

// reversed yellow card movement for go home function
void ReverseYellow(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn right 90 degrees
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    // go forward one block
    ForwardOneBlock(&motorL, &motorR);
    reverseDetect(&motorL, &motorR);
    // turn 180 degrees
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
}

// reversed pink card movement for go home function
void ReversePink(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    ForwardOneBlock(&motorL, &motorR);
    reverseDetect(&motorL, &motorR);
    // turn 180 degrees
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
}

// reversed orange  card movement for go home function
void ReverseOrangeMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn right 135 degree
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
    turnLeft45(&motorL, &motorR);
}

// reversed light blue card movement for go home function
void ReverseLightBlueMove(struct DC_motor *mL,struct DC_motor *mR){
    reverseDetect(&motorL, &motorR);
    // turn left 135 degree
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
    turnRight45(&motorL, &motorR);
}

void turnCalibration (struct DC_motor *mL,struct DC_motor *mR){
    Left_Signal=1;  // turn on left signal
    __delay_ms(1000);
    
    while (!(RF2_button && RF3_button)){ // stay in loop unless both buttons pressed at same time

        Beam_Light=1; // turn on beam light
        // turn 180 degrees
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        
        while (!(RF2_button || RF3_button)){ // stay in loop unless any button pressed 

              Break_Light=1; // turn on break light to show entered while loop
            __delay_ms(2000);
            if(RF3_button && RF2_button){
                LATHbits.LATH3=1; // turn on LED to show button pressed
                LATDbits.LATD7=1; // turn on LED to show button pressed
                __delay_ms(1000);
                LATHbits.LATH3=0; // turn off LED
                LATDbits.LATD7=0;
            }
            
            else if (RF3_button){
                Turn45Delay+=10; // increase turn angle
                LATHbits.LATH3=1; // turn on LED to show button pressed
                __delay_ms(1000);
                LATHbits.LATH3=0; // turn off LED
            }

            else if (RF2_button){
                Turn45Delay-=10; // decrease turn angle
                LATDbits.LATD7=1;// turn on LED to show button pressed
                __delay_ms(1000);
                LATDbits.LATD7=0; // turn off LED
            }
        }
        Break_Light=0; // turn off break light
        __delay_ms(2000);
    }
    Beam_Light=0; // turn off beam light
    __delay_ms(2000);
} 