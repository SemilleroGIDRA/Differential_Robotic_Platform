/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on January 12, 2024, 9:16 AM
 */


#include <xc.h> //Library to Microchip Microcontrollers.
#include "Fuses_Set.h" //Library to set fuses of the microcontroller. 

//Instructions of the LCD 
#define CLR 0x01 //Function to clear the LCD. 
#define ROW1 0X00
#define ROW2 0x00
#define ROW3 0X00
#define ROW4 0X00
#define Left 0x00
#define Right 0x00
#define Set_Instruction 0
#define Write_Instruction 1
//Pins of the LCD 
#define RS LATCbits.LC4 //Define pin (RC4) Register Select as bit flag.
#define E LATCbits.LC5 //Define pin (RC5) Enable as bit flag.   


//Prototype Functions
void Configurations(void); //Function to set registers.
void Init_LCD(void); //Function to initialize the LCD. 
void Instruction_LCD(unsigned char Command, unsigned char Data); //Function to send instruction to LCD. 

//Main function.

void main(void) {

    //Call functions. 
    Configurations();
    Init_LCD();
    //Infinite Loop.
    while (1) {

    }

}

//Develop configurations function. 

void Configurations(void) {

    OSCCON = 0x72; //Oscillator Controller set as internal oscillator and frequency (16MHz). 

    //Analog & Digital configuration. 
    ANSELD = 0x00; //Analog input control, set port as digital. 
    ANSELCbits.ANSC4 = 0; //Set as digital pin. 
    ANSELCbits.ANSC5 = 0; //Set as digital pin. 

    //Inputs & Outputs configuration. 
    TRISD = 0x00;
    TRISCbits.RC4 = 0;
    TRISCbits.RC5 = 0;

    //

}

//Develop initialize LCD function.

void Init_LCD(void) {

    __delay_ms(20);



}

//Develop the LCD instruction. 

void Instruction_LCD(unsigned char Command, unsigned char Data) {

    RS = Command;


}