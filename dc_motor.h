#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
};
//create structure for dc motors

struct DC_motor motorL, motorR; //declare two DC_motor structures 

int power = 50; // forward backward, turning motor power
int Turn45Delay = 60; //time delay in ms for a 45 degree turn
int RunOneBlockTime = 1050; // time delay to run one block

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *mL,struct DC_motor *mR); // function to let left and right motor stop
void turnLeft(struct DC_motor *mL,struct DC_motor *mR); // function to let car turn right continuously
void turnRight(struct DC_motor *mL,struct DC_motor *mR); // function to let car turn left continuously
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR); // function to let car go forward continuously
void timed_forward(struct DC_motor *mL, struct DC_motor *mR, int time); // function to let car go forward for variable amount of time
void fullSpeedBack(struct DC_motor *mL,struct DC_motor *mR); // function to let car go backward continuously

void turnRight45(struct DC_motor *mL,struct DC_motor *mR); // function to let car turn right 45 degrees
void turnLeft45(struct DC_motor *mL,struct DC_motor *mR); // function to let car turn left 45 degrees
void reverseDetect(struct DC_motor *mL,struct DC_motor *mR); // function to let car reverse set amount of time
void homeReverse(struct DC_motor *mL,struct DC_motor *mR); // reverse a little on blue card go home route
void reverseOneBlock(struct DC_motor *mL,struct DC_motor *mR); // function to let car reverse one block
void ForwardOneBlock(struct DC_motor *mL,struct DC_motor *mR); // function to let car forward one block
void Backhalfblock(struct DC_motor *mL,struct DC_motor *mR); // function to let car reverse half block
void Forwardhalfblock(struct DC_motor *mL,struct DC_motor *mR); // function to let car forward half block
void quaterForward(struct DC_motor *mL,struct DC_motor *mR); // function to let car forward 1/4 block

void RedMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to red card
void GreenMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to green card
void BlueMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to blue card
void YellowMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to yellow card
void PinkMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to pink card
void OrangeMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to orange card
void LightBlueMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out movement corresponding to light blue card
void RetryMove(struct DC_motor *mL,struct DC_motor *mR);
void ReverseYellow(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out reverse movement corresponding to yellow card when returning home
void ReversePink(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out reverse movement corresponding to pink card when returning home
void ReverseOrangeMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out reverse movement corresponding to orange card when returning home
void ReverseLightBlueMove(struct DC_motor *mL,struct DC_motor *mR); // function to let car carry out reverse movement corresponding to light blue card when returning home

void turnCalibration(struct DC_motor *mL,struct DC_motor *mR); // function to allow user calibrate turn angle before start exploring maze
void TurnDelay(int Turn45Delay); // custom delay function to allow variable turn delay time
#endif
