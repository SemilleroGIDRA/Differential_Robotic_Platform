/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on February 14, 2024, 4:06 PM
 */


#include <xc.h>
#include "Fuses_Configuration.h"
#include <stdint.h> //Function to use 8 bits integer

//Instructions or commands of the LCD 
#define CLR 0x01 //Command to clear the LCD. 
#define RH 0x02 //Command to Return to Home that is the first position of the LCD (inside first raw and column).
#define EMS 0x06 //Command Entry Mode Set to use moving direction cursor. 
#define DC 0x0F //Command Display Control to set display, cursor and blink. 
#define DSR 0x1C //Command to control the cursor shift to Right (Display Shift Right).
#define DSL 0x18 //Command to control the cursor shift to Left (Display Shift Left). 
#define FS 0x28 //Command to control the 4(28)/8(38) bits mode, 5x8/5x11 character, 1/2 lines display.
#define ROW1 0X80 //Command to access the address of row 1. 
#define ROW2 0xC0 //Command to access the address of row 2. 
#define ROW3 0X94 //Command to access the address of row 3. 
#define ROW4 0XD4 //Command to access the address of row 4. 
#define Set 0 //Command to configuration an instruction. 
#define Write 1 //Command to write on the LCD. 

//Pins of the LCD 
#define RS LATCbits.LATC4 //Define pin (RC4) Register Select as bit flag.
#define EN LATCbits.LATC5 //Define pin (RC5) Enable as bit flag.

//Prototype Functions.
void Configurations(void); //Function to set registers.
void Init_LCD(void); //Function to initialize the LCD. 
void LCD_Instruction(unsigned char Instruction); //Function to send data or instruction inside LCD.
void Send_Instruction_Data(unsigned char Instruction, unsigned char Data); //Function to enable or disable RS.
void Send_String(unsigned char *String);
void Test(void);

//Global variables. 
unsigned char Text1 [20] = {"Hello!"}; //Variable to show on first row of the LCD.
unsigned char Text2 [26] = {"Everyone!"}; //Variable to show on the second row of the LCD. 
unsigned char Text3 [30] = {"Welcome to this"};
unsigned char Text4 [30] = {"Microcontroller!"};

//Main function 

void main(void) {

    //Call functions. 

    //Infinite loop.
    while (1) {



    }

    return;
}

//Develop configurations function

void Configurations(void) {

    OSCCON = 0x72; //Internal oscillator 
    //Set pins as digital
    ANSELC = 0;
    ANSELD = 0;
    //Set pins as outputs
    TRISCbits.RC4 = 0;
    TRISCbits.RC5 = 0;

    TRISD = 0;
    //Clean pins 
    LATCbits.LC4 = 0;
    LATCbits.LC5 = 0;

    LATD = 0;

}

//Develop initialize LCD function.

void Init_LCD(void) {

    __delay_ms(60); //Delay set by the manufacturer. 
    Send_Instruction_Data(Set, 0x30); //Data set by the manufacturer. 
    __delay_ms(5); //Delay set by the manufacturer. 
    Send_Instruction_Data(Set, 0x30); //Data set by the manufacturer. 
    __delay_ms(5); //Delay set by the manufacturer. 
    Send_Instruction_Data(Set, 0x30); //Data set by the manufacturer. 
    Send_Instruction_Data(Set, 0x02); //Data set by the manufacturer. 
    Send_Instruction_Data(Set, EMS); //Send entry mode set.
    Send_Instruction_Data(Set, DC); //Send command to Display Control. 
    Send_Instruction_Data(Set, FS); //Send command control.
    Send_Instruction_Data(Set, CLR); //Send clear LCD. 
    __delay_ms(100); //Delay set by the manufacturer. 

}

//Develop LCD function to send data or instruction. 

void Send_Instruction_Data(unsigned char Instruction, unsigned char Data) {

    RS = Instruction; //Enable or disabled the register select to write data, or send instruction set.
    LCD_Instruction(Data >> 4); //Send first the most significant bits
    LCD_Instruction(Data); //Send the least significant bits. 

}
//Develop LCD instruction.

void LCD_Instruction(unsigned char Instruction) {

    EN = 1; //Pin ENabled. 
    __delay_ms(15); //Wait for the instruction. 
    LATD = Instruction; //Send Instruction to the port. 
    __delay_ms(15); //Wait for the instruction. 
    EN = 0; //Pin disabled.
    __delay_ms(15); //Wait for the instruction. 

}

//Develop send string

void Send_String(unsigned char *String) { //Receiver string data. 

    for (uint8_t i = 0; String[i] != '\n'; i++) {   //i from 0, to find null character, increment 1. 

        Send_Instruction_Data(Write, String[i]); //Print each character of string. 

    }

}

//void main(void) {
//
//    //Call functions. 
//    Configurations();
//    Init_LCD();
//    Test();
//
//    //Infinite Loop.
//    while (1) {
//
//    }
//
//}
//
////Function to show data on LCD. 
//
//void Test(void) {
//
//    Send_Instruction_Data(Set, ROW1);
//
//    for (int i = 0; Text3[i] != '\n'; i++) {
//
//        Send_Instruction_Data(Write, Text3[i]);
//
//    }
//
//    Send_Instruction_Data(Set, ROW2);
//
//    for (int i = 0; Text4[i] != '\n'; i++) {
//
//        Send_Instruction_Data(Write, Text4[i]);
//
//    }
//
//    Send_Instruction_Data(Set, ROW3);
//
//    for (int i = 0; Text1[i] != '\n'; i++) {
//
//        Send_Instruction_Data(Write, Text1[i]);
//
//    }
//
//    __delay_ms(100);
//
//    Send_Instruction_Data(Set, ROW4);
//
//    for (int j = 0; Text2[i] != '\n'; j++) {
//
//        Send_Instruction_Data(Write, Text2[j]);
//
//    }
//
//}
