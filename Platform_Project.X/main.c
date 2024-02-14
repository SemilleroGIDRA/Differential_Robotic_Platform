/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on February 14, 2024, 6:11 PM
 */


#include <xc.h>
#include <stdint.h>
#include "Fuses_Configuration.h"

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

//Pins of the LCD.
#define RS LATCbits.LATC4 //Define pin (RC4) Register Select as bit flag.
#define EN LATCbits.LATC5 //Define pin (RC5) Enable as bit flag.

//Prototype functions. 
void Init_LCD(void); //Function to initialize the LCD. 
void LCD_Instruction(unsigned char Instruction); //Function to send data or instruction inside LCD.
void Send_Instruction_Data(unsigned char Instruction, unsigned char Data); //Function to enable or disable RS.
void Send_String(unsigned char *String);
//Main function.

void main(void) {

    //Call functions. 
    
    Init_LCD(); 
    //Infinite Loop. 
    while (1) {



    }

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

    for (uint8_t i = 0; String[i] != '\0'; i++) { //i from 0, to find null character ('\0'), increment 1. 

        Send_Instruction_Data(Write, String[i]); //Print each character of string. 

    }

}