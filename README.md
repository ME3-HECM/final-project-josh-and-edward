# Course project - Mine navigation search and rescue

## Overview of the ".c/.h" files

#### main.c: Sets up initialisation functions and initialises motor structure, calls calibration procedures before going into main operating loop

#### color.c/.h: Contains functions to configure the color click module, functions that recognize colour from colour click reading

#### interrupts.c/.h: Functions to initialise interrupts sources and a high priority interrupt service routine for the timer overflow

#### i2c.c/.h: Set up of the I2C serial communication interface in order to read RGBC values from the TCS3471 color click module during testing

#### serial.c/.h:

#### timers.c/.h: Set up of the Timer0 source, function to read timer value

#### dc_motor.c/.h: Define motor structure, configuration of dc motor, functions for specific basic moves e.g. turn left 45 and a calibration routine depending on the surface, functions for moves corresponding to each colour recognition

#### Memory.c/.h: Create arrays to store route information, function to let the buggie return to base by calling arrays


## Main Operating Procedure
	Main Loop:
	

## Motor calibration
The motor calibration routine is run on start and does a 180 degree turn after this it then waits until either RF2 or RF3 button is pressed and. If the buggy is short of 180 degrees the RF3 button is pressed and held until the red light comes on and off. This increases the timing of the right motor so it turns longer while RF2 button makes it turn shorter with the same associated buttons. 

After both buttons are pressed and held both red lights come on and wait until the headlights and left signal lights turn off. This exists the calibration routine and the buggy now waits for the RF2 button to be pressed to start the maze. 

	void turnCalibration (struct DC_motor *mL,struct DC_motor *mR){
    LATFbits.LATF0=1;  // turn on left signal
    __delay_ms(1000);
    
    while (!(RF2_button && RF3_button)){
        LATDbits.LATD3=1; // turn on beam light
        // turn 180 degrees
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        turnLeft45(&motorL, &motorR);
        
        while (!(RF2_button || RF3_button)){
              LATDbits.LATD4=1; // turn on break light
            __delay_ms(2000);
            if(RF3_button && RF2_button){
                LATHbits.LATH3=1;
                LATDbits.LATD7=1;
                __delay_ms(1000);
                LATHbits.LATH3=0;
                LATDbits.LATD7=0;
            }
            
            else if (RF3_button){
                Turn45Delay+=10;
                LATHbits.LATH3=1;
                __delay_ms(1000);
                LATHbits.LATH3=0;
            }

            else if (RF2_button){
                Turn45Delay-=10;
                LATDbits.LATD7=1;
                __delay_ms(1000);
                LATDbits.LATD7=0;
            }
        }
        LATDbits.LATD4=0; // turn off break light
        __delay_ms(2000);
    }
    LATDbits.LATD3=0; // turn off beam light
    __delay_ms(2000);
    }

## dc_motor.c
This file has the associated motor functions that turn left, right and go forward. The most critical commands being used are turn left 45 , turn right 45 , fullspeedAhead and timed_forward. 

Turn left and right 45 activate the right turn function for a specified ammount of time that is calibrated in the motor calibration routine: 

	void turnRight45(struct DC_motor *mL,struct DC_motor *mR){
	    turnRight(mL,mR);
	    TurnDelay(Turn45Delay);
	    stop(&motorL, &motorR);
	    __delay_ms(50); 
	}

The fullspeedAhead function is the function that is run in the main code and will ramp up the speed of the car until the specified power at which point the car will move continuouslly forward until another instruction is received. 

	void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
	{

    mL->direction =1;
    mR->direction =1;
    while (mL->power < power && mR->power < power){

        mL->power+=10;
        mR->power+=10;

        setMotorPWM(mL);
        setMotorPWM(mR);
        
        
        __delay_ms(20);
    }

	}
	
Finally the timed_forward function does a similar role as the fullspeedAhead but runs for a specified ammount of time. This is used when returning home to travel forward after a specified ammount of time until a turn is required. 
	

## Distinction of colours
The code hear reads 4 different value (R,G,B,C) standing for Red, Green, Blue, Clear respectively. The values are read in the functiion 
	readColours:
	
	void readColours (struct RGB *vals) {
 	vals ->R = color_read_Red(); // reads colour red and stores it in R location of struct
    	vals ->B = color_read_Blue(); // reads colour Blue and stores it in B location of struct
    	vals ->G = color_read_Green(); // reads colour Green and stores it in G location of struct
   	 vals ->L = color_read_lum();        // reads colour Clear and stores it in L location of struct
	}

These call the functions to read the colours and place them in the struct. There is a second function which calculates the relative values of the colours to make it less dependent on ambient light. 

Finally the colour decider function is the most critical function of this file and returns an associated number to indicate which colour it has seen. It does this by calculating the hue of the colour which equations were obtained from here: https://www.rapidtables.com/convert/color/rgb-to-hsl.html. 
Colour | Instruction
---------|---------
Red | 1
Orange | 2
Yellow | 3
Blue | 4
Green | 5
Light blue | 6
Pink | 7
White | 0
Black/Unknown colour | 10

To achieve this we place the RGB values in an array and itterate through them to find the min and max values. We then execute the equation depending on whether R G or B is found to be the maximum. 


Hue calculation:

	for (i=0; i<3 ;i++){
        	if (RGB_col[i]>Cmax){
            	Cmax=RGB_col[i];
            	Cmax_i=i;
        	}
        	if (RGB_col[i]<Cmin){Cmin=RGB_col[i];}           
    		}
    	// if Cmax and Cmin are equal to each other then the colour is white so return 0
    	if (Cmax-Cmin==0){return 0;}
    	// if the max colour is red calculate the hue based on the formula
    	if (Cmax_i==0){ 
        	Hue = 60 * ((RGB_col[1]-RGB_col[2])/(Cmax-Cmin));
        	// if the hue is negative add 360 as hue is based off a circle angle
        	if (Hue < 0) {Hue += 360;}
    	}
    	// if the max colour is green then calculate the hue based on the formula
    	else if (Cmax_i==1){
       	 	Hue=60*(2+(RGB_col[2]-RGB_col[0])/(Cmax-Cmin));
   	 }
    	// otherwise the max colour must be blue and calculate it off this formula
    	else {
        	Hue=(4+(RGB_col[0]-RGB_col[1])/(Cmax-Cmin))*60;
    	}
After determining the hue it will then go through a series of if statements as each card has a certain hue range carved out for it. For example for red it is in between 330 and 360. 

However there are exceptions with it where certain colours have hues that are very close toghether. Light blue and Green are one pair and White and pink are another pair that cannot be consistently distinguished on hue alone. Thus we rely on the realtive values to distinguish them and if they still cannot distinguish the colour then it returns 10 which stands for an unknown colour. For example for light blue and green:

Light Blue and Green:

	// if the hue is between the found ranges of green return green 
    else if ((60<=Hue)&(Hue<=77)){
        if ((0.12<=rel->B)&(0.21>rel->G)){
            return 6; // light blue
        }
        else if ((0.12>rel->B)&(0.21<rel->G)){
            return 5; // green
        }
        else{
            return 10; // return unknown colour
        }
        
    }


## Timers

##  Memory operation
The go home function is in charge of returning the buggy home once called. It does this through 2 arrays, Time_forward and WayBack. The first array stores a series of time values and the second array stores the array of turns taken. It first turns the buggy arround through executing the blue command and stops the timer. After this it runs through a for loop itterating backwards starting at the most recent move_count index and working backwards. 

It first accesses the current time and runs the timed_forward function to run the buggy forward for that speciefied ammount of time. After this it then accesses the WayBack array at the index move_count-1. This is because the forward command is always reccorded first and then its associated turn command is executed. The forward array will always be one greater than then the WayBack array because the white card is not stored in the WayBack array. 

After identifing the WayBack array it then executes the reverse move of the colour listed below

Colour | Reverse instruction
---------|---------
Red | Green
Orange | Light Blue
Yellow | Reverse Yellow
Blue | Blue
Green | Red
Light blue | Orange
Pink | Reverse Pink

After it has run through the for loop it then stops the motor and turns the run flag off to prevent accidental restarts. 

	    int i;
    BlueMove(&motorL, &motorR);
    T0CON0bits.T0EN=0;
    for (i = move_count; i >= 0; i--){
        timed_forward(&motorL, &motorR,Time_forward[i]);
            
        if (WayBack[i-1]==1){
            reverseDetect(&motorL, &motorR);
            GreenMove(&motorL, &motorR); // opposite of red move
        }
        else if (WayBack[i-1]==2){
            reverseDetect(&motorL, &motorR);
            LightBlueMove(&motorL, &motorR); // opposite of orange move
        }
        else if (WayBack[i-1]==3){
            reverseDetect(&motorL, &motorR);
            ReverseYellow(&motorL, &motorR); // opposite of yellow move
        }
        else if (WayBack[i-1]==4){
            reverseDetect(&motorL, &motorR);
            BlueMove(&motorL, &motorR); // 180 degrees same either way
        }
        else if (WayBack[i-1]==5){
            reverseDetect(&motorL, &motorR);
            RedMove(&motorL, &motorR); //opposite of green move
        }
        else if (WayBack[i-1]==6){
            reverseDetect(&motorL, &motorR);
            OrangeMove(&motorL, &motorR); // opposite of light blue move
        }
        else if (WayBack[i-1]==7){
            reverseDetect(&motorL, &motorR);
            ReversePink(&motorL, &motorR);
        }
        stop(&motorL, &motorR);
        run_flag=0;


  	} 
	}

## Exceptions

## Performance


## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## "Mine" environment specification

A "mine" is contstructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Colour | Instruction
---------|---------
Red | Turn Right 90
Green | Turn Left 90
Blue | Turn 180
Yellow | Reverse 1 square and turn right 90
Pink | Reverse 1 square and turn left 90
Orange | Turn Right 135
Light blue | Turn Left 135 
White | Finish (return home)
Black | Maze wall colour

Mine courses will vary in difficulty, with the simplest requiring 4 basic moves to navigate. More advanced courses may require 10 or moves to navigate. The mines may have features such as dead ends but colour cards will always direct you to the end of the maze. Once the end of the maze has been reached, you must return to the starting position. An example course to navigate is shown below. You do not know in advance which colours will be in the course or how many.

![Navi Diagram](gifs/maze.gif)

## Resources and project submission

To develop your solution you have your Clicker 2 board, buggy and colour click add on board. You must not use any hardware that is not provided as part of this course and you must use the XC8 compiler to compile your C code. 

Please use this GitHub repo to manage your software development and submit your project code. 

Final testing will take place in the CAGB foyer and testing areas will be provided around the mechatronics lab. You will not know the exact layout of the maze(s) in advance. You will also be asked to give a short presentation on the testing day.

## Supplementary technical information

### Additional buggy features

In addition to the motor drives we explored in lab 6, the buggy contains some additional features that may be of use during the project. The first feature is additional LEDs, controlled through the pins labelled **H.LAMPS**, **M.BEAM**, **BRAKE**, **TURN-L** and **TURN-R**. H.LAMPS turns on the front white LEDs and rear red LEDs, at a reduced brightness. M.BEAM and BRAKE enable you to turn these LEDs on at full brightness. The turn signals have not hardware based brightness control. These LEDs give you a method to provide feedback for debugging, in addition of the LEDs on the clicker board.

![Buggy pinout](gifs/buggy_pins.png)

A further feature of the buggy is **BAT-VSENSE** pin which allows you to monitor the batter voltage via an analogue input pin. The battery is connected across a voltage divider, as shown in the diagram below:

![Bat sense](gifs/bat_vsense.png)

The voltage at BAT-VSENSE will always be one third of that at the battery. Measuring this value can be useful for determining if your battery needs charging. You could also use it to calibrate your robot to perform well at different charge levels. 

### Colour click

The Colour click board contains 2 devices to help with navigation, a tri-colour LED for illumination and a 4 channel RGBC photodiode sensor. This combination of two devices (an illumination source and a sensor) allow you a make measurements of the reflected colour of objects near the sensor. The circuit diagram for the Colour click is shown below:

![Color Cick](gifs/color_click.png)

The tri-colour LED is the simpler of the two devices to control. Three separate pins control the red, green and blue LEDs individually, despite them being in a single package. Control via these pins is simple digital on/off control and if any brightness control was required, the user would need program the microcontroller to generate a PWM signal to achieve this.  

The second device on the Colour click is the TCS3471 colour light-to-digital converter. The sensor contains a grid of 4x4 photodiodes, 4 are sensitive to red light, 4 green light, 4 blue light and 4 "clear" light (that is, a range of wavelengths, see datasheet for exact spectral response). When light falls on the photodiode the photons are absorbed and current is generated. This signal is then integrated over time using integrators and sampled by 16 bit on board ADCs. Communication with the device is achieved using an I2C interface. This enables configuration of the device to customise sampling of the data (i.e. integration time, gain, etc.) and to read the 16 bit digital values for each of the RGBC channels. The relative magnitude of these values gives you information about the colour of light that is falling on the sensor. The device can also be configured to send an interrupt signal to the PIC when signal reaches a preset value.

### I2C

The I2C interface widely used in industry for communication between microcontrollers and peripheral integrated circuits (other chips) over short distances. I2C is serial communication bus that enables communication between many devices over a simple 2 wire interface. One wire is the data line (SDA) and is used for both transmission and receiving. The second wire (SCL) is used for a clock signal to ensure all devices are synchronous. To ensure communication of data occurs without problem a message protocol must be followed and understood by all devices on the bus. Devices are termed master and slave devices, with master devices initiation communication to a slave device via unique address for that device. The general sequence of communication between a master/slave over the I2C interface is a follows:

1. Send a Start bit
1. Send the slave address, usually 7 bits
1. Send a Read (1) or Write (0) bit to define what type of transaction it is
1. Wait for an Acknowledge bit
1. Send a data or command byte (8 bits)
1. Wait for Acknowledge bit
1. Send the Stop bit

This is shown pictorial in the figure below:

![I2C](gifs/i2c.png)

Although it is possible to program an entirely software based I2C interface, luckily for us our PIC chip has a module dedicated to generating and receiving I2C signals: the Master Synchronous Serial Port Module, or MSSP (see chapter 28 of the PIC datasheet). This module provides methods for sending start/stop/acknowledge bits and allows us to focus on sending/receiving data.

The included i2c.c/h files contain functions to help you get started with I2C communication. The first function below sets up the MSSP module as an I2C master device and configures the necessary pins.

	void I2C_2_Master_Init(void)
	{
		//i2c config  
		SSP2CON1bits.SSPM= 0b1000;    // i2c master mode
		SSP2CON1bits.SSPEN = 1;       //enable i2c
		SSP2ADD = (_XTAL_FREQ/(4*_I2C_CLOCK))-1; //Baud rate divider bits (in master mode)
  
		//pin configuration for i2c  
		TRISDbits.TRISD5 = 1;                   //Disable output driver
		TRISDbits.TRISD6 = 1;                   //Disable output driver
		ANSELDbits.ANSELD5=0;					// disable analogue on pins
		ANSELDbits.ANSELD6=0;					// disable analogue on pins
		SSP2DATPPS=0x1D;      //pin RD5
		SSP2CLKPPS=0x1E;      //pin RD6
		RD5PPS=0x1C;      // data output
		RD6PPS=0x1B;      //clock output
	}
	
Bits with the SSP2CON2 register are set to send the individual start/stop/acknowledge bits used in the protocol. These must only be set when the bus is idle (nothing being sent/received). The I2C_2_Master_Start(), I2C_2_Master_Stop() and I2C_2_Master_RepStart() functions allow you add the necessary bits as defined in the protocol above. Data is sent on the bus using the SSP2BUF register:

	void I2C_2_Master_Write(unsigned char data_byte)
	{
		I2C_2_Master_Idle();
		SSP2BUF = data_byte;         //Write data to SSPBUF
	}

Data is also read using the same SSP2BUF registers. However, to receive data we first need to switch the module into receiver mode. We also need to start the acknowledge sequence to let the slave device know what we have received the data OK. The following function achieves this:

	unsigned char I2C_2_Master_Read(unsigned char ack)
	{
		unsigned char tmp;
		I2C_2_Master_Idle();
		SSP2CON2bits.RCEN = 1;        // put the module into receive mode
		I2C_2_Master_Idle();
		tmp = SSP2BUF;                //Read data from SS2PBUF
		I2C_2_Master_Idle();
		SSP2CON2bits.ACKDT = !ack;     // 0 turns on acknowledge data bit
		SSP2CON2bits.ACKEN = 1;        //start acknowledge sequence
		return tmp;
	}

The functions described so form the basics required for I2C communication with the PIC. To communicate with the TCS3471 onboard the Colour click we first need to know its address. This is listed in the data sheet as 0x29. To send our first byte over the I2C we need to send this address combined with the transaction type (read or write) as defined in the protocol diagram above. This lets the TCS3471 know the message is intended for it and not some other device on the interface. Next we send a byte which is a combination of command type and the register address in the TCS3471 that we want to write to. Finally we the value that we want to write to that register. This sequence is shown in the function below:

	void color_writetoaddr(char address, char value){
		I2C_2_Master_Start();         		//Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write (0) mode (note 0x52=0x29<<1)
		I2C_2_Master_Write(0x80 | address);    //command + register address
		I2C_2_Master_Write(value);    			//value to store in the register
		I2C_2_Master_Stop();          //Stop condition
	}

We then call the function to, for example, turn the device on:

	color_writetoaddr(0x00, 0x01); // write 1 to the PON bit in the device enable register
	
There are additional commands that must be set to initialise the device and many registers that be configured to obtain optimal performance for the sensor in your conditions. It is up to you to carefully read the TCS3471 datasheet and experiment with this.

To read values from the TCS3471 you need to a similar sequence to above but you first need to tell the device which register you want to deal with, before telling the device you want read from it. The example below uses the combined read format to read multiple bytes in sequence. The auto-increment is set so that instead of reading the same register over and over again, it automatically advances to the next one. The example starts at the Red channel low byte address and then automatically advances and reads the associated high byte.

	unsigned int color_read_Red(void)
	{
		unsigned int tmp;
		I2C_2_Master_Start();         //Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
		I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
		I2C_2_Master_RepStart();
		I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
		tmp=I2C_2_Master_Read(1);			// read the Red LSB
		tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
		I2C_2_Master_Stop();          //Stop condition
		return tmp;
	}

Instead of separate functions for each channel you may want to create a structure to store all the values together, and pass a pointer to the function so that all values in the structure can be updated in one operation. An example structure might look like this:

	//definition of RGB structure
	struct RGB_val { 
		unsigned int R;
		unsigned int G;
		unsigned int B;
	};

This concludes the basics of I2C and communication with the colour sensor. Best of luck! 
