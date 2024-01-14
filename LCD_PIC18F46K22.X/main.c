/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on January 12, 2024, 9:16 AM
 */


#include <xc.h> //Library to Microchip Microcontrollers.
#include <string.h> //Library to manipulating strings. 
#include "Fuses_Set.h" //Library to set fuses of the microcontroller. 

//Instructions of the LCD 
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
void Send_Instruction_Data(unsigned char Instruction, unsigned char Data);
void Test(void);
//Global variables. 
char Text1 [20] = {"Hello!"}; //Variable to show on first row of the LCD.
char Text2 [26] = {"Everyone!"}; //Variable to show on the second row of the LCD. 

//Main function.

void main(void) {

    //Call functions. 
    Configurations();
    Init_LCD();
    Test();

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

    //Clean ports. 
    LATC = 0;
    LATD = 0;

}

//Develop initialize LCD function.

void Init_LCD(void) {

    __delay_ms(20);
    Send_Instruction_Data(Set, 0x30);
    __delay_ms(5);
    Send_Instruction_Data(Set, 0x30);
    __delay_ms(5);
    Send_Instruction_Data(Set, 0x30);
    Send_Instruction_Data(Set, 0x02);
    Send_Instruction_Data(Set, EMS);
    Send_Instruction_Data(Set, DC);
    Send_Instruction_Data(Set, FS);
    Send_Instruction_Data(Set, CLR);
    __delay_ms(5);

}

//Develop LCD function to send data or instruction. 

void Send_Instruction_Data(unsigned char Instruction, unsigned char Data) {

    RS = Instruction; //Enable or disabled the register select to write data, or send instruction set.
    LCD_Instruction(Data >> 4); //Send first the most significant bits
    LCD_Instruction(Data); //Send the least significant bits. 

}

//Develop LCD instruction.

void LCD_Instruction(unsigned char Instruction) {

    EN = 1;
    __delay_ms(15);
    LATD = Instruction;
    __delay_ms(15);
    EN = 0;
    __delay_ms(15);

}

//Function to show data on LCD. 

void Test(void) {

    Send_Instruction_Data(Set, ROW3);

    for (int i = 0; i < strlen(Text1); i++) {

        Send_Instruction_Data(Write, Text1[i]);

    }

    __delay_ms(100);

    Send_Instruction_Data(Set, ROW4);

    for (int j = 0; j < strlen(Text2); j++) {

        Send_Instruction_Data(Write, Text2[j]);

    }

}
