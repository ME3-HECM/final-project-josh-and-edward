#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "LED_buttons.h"
//initDCmotorsPWM(200);

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master
     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);
    //set integration time
	color_writetoaddr(0x01, 0xD5);
    //Gain =1X
    color_writetoaddr(0x0F, 0x00);
    
    TRISGbits.TRISG1 = 0; // Set TRIS value for red LED (output)
    TRISAbits.TRISA4 = 0; // Set TRIS value for green LED (output)
    TRISFbits.TRISF7 = 0; // Set TRIS value for blue LED (output)
    
    TRISDbits.TRISD3 = 0; // Sets tris value for head lamp output
    TRISDbits.TRISD4 = 0; // sets tris value for red leds output
    
    RED_LED=0;
    BLUE_LED=0;
    GREEN_LED=0;
    
    LATDbits.LATD3=0;  // Toggles the front white LEDs (at full brightness)
    LATDbits.LATD4=0;     // Toggles the rear red LEDs (at full brightness)
    
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_lum(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at L low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the L LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the L MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}
unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}


void readColours (struct RGB *vals) {
    
    vals ->R = color_read_Red(); // reads colour red and stores it in R location of struct
    vals ->B = color_read_Blue(); // reads colour Blue and stores it in R location of struct
    vals ->G = color_read_Green(); // reads colour Green and stores it in R location of struct
    vals ->L = color_read_lum();        // reads colour Clear and stores it in R location of struct
}


void colour_rel(struct RGB *vals, struct RGB_rel *rel){
    float R = vals->R;  // transfers red value to float variable 
    float G = vals->G; // transfers Green value to float variable
    float B = vals->B; // transfers Blue value to float variable
    float L = vals->L;// transfers Clear value to float variable
    rel -> R = R/(R+G+B+L); // Calculates relative value of red out of total RGBL values
    rel -> B = B/(R+G+B+L); // Calculates relative value of blue out of total RGBL values
    rel -> G = G/(R+G+B+L); // Calculates relative value of Green out of total RGBL values
    // stores all relative value in new struct of floats for relative values
}

int Colour_decider(struct RGB *vals, struct RGB_rel *rel){//added motor structure as inputs 
    float Cmax = 0;
    float Cmin= 100000; 
    int Cmax_i =4; // intialises Cmax_i as anything will be replaced after for loop
    
    int i; // initalises int i for for loop
    float RGB_col[3]={vals->R,vals->G,vals->B}; // initalises array made of values of current RGB readings
    float Hue; // initalises float variable to store the hue
    
    // code to store max and min RGB colour and max index
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
    //return Hue;
    // if the hue is between the found ranges of red return that its red
    if ((330<=Hue)&(Hue<=360)){
        return 1; // red
    }    
    // if the hue is between the found ranges of orange return that its red
    else if ((5<=Hue)&(Hue<=11)){
        return 2; // orange
    } 
    // if the hue is between the found ranges of white, yellow or blue 
    else if ((18<=Hue)&(Hue<=25)){
        // check if the relative values lie in the white range if it does then its white
        if ((rel->R<=0.26)&&(rel->B>0.10)&&(vals->L>1000)){
            return 0;//white
        }
        else if ((rel->R>=0.26)&&(rel->B<0.10)){
            return 3; // yellow
        }
        else{
            return 10;
        }
               
    }
        // if the hue is between the found ranges of light blue return light blue 
    else if ((85<=Hue)&(Hue<=160)){
        return 4; //blue
    }

    // if the hue is between the found ranges of green return green 
    else if ((60<=Hue)&(Hue<=77)){
        if ((0.12<=rel->B)&(0.21>rel->G)){
            return 6; // light blue
        }
        else if ((0.12>rel->B)&(0.21<rel->G)){
            return 5; // green
        }
        else{
            return 10;
        }
        
    }
    // if the hue is between the found ranges of white or pink 
    else if ((12<=Hue)&(Hue<=17)){
            return 7; // pink
          }
    //}

    else{
        return 10; // could not recognise colour
    }
  
    
    
    
            
    
   
} 
    