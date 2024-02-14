/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on January 12, 2024, 9:16 AM
 */


#include <xc.h> //Library to Microchip Microcontrollers.
#include <string.h> //Library to manipulating strings. 
#include "Fuses_Set.h" //Library to set fuses of the microcontroller. 

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
void Test(void);
//Global variables. 
char Text1 [20] = {"Hello!"}; //Variable to show on first row of the LCD.
char Text2 [26] = {"Everyone!"}; //Variable to show on the second row of the LCD. 
char Text3 [30] = {"Welcome to this"};
char Text4 [30] = {"Microcontroller!"};

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
    TRISD = 0x00; //All port as output. 
    TRISCbits.RC4 = 0; //Pin 4 from the PORT C as output.
    TRISCbits.RC5 = 0; //Pin 5 from the port C as output. 

    //Clean port C and D. 
    LATC = 0;
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
    Send_Instruction_Data(Set, EMS);
    Send_Instruction_Data(Set, DC);
    Send_Instruction_Data(Set, FS);
    Send_Instruction_Data(Set, CLR);
    __delay_ms(200); //Delay set by the manufacturer. 

}

//Develop LCD function to send data or instruction. 

void Send_Instruction_Data(unsigned char Instruction, unsigned char Data) {

    RS = Instruction; //Enable or disabled the register select to write data, or send instruction set.
    LCD_Instruction(Data >> 4); //Send first the most significant bits
    LCD_Instruction(Data & 0x0F); //Send the least significant bits. 

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

//Function to show data on LCD. 

void Test(void) {

    Send_Instruction_Data(Set, ROW1);

    for (int i = 0; i < strlen(Text3); i++) {

        Send_Instruction_Data(Write, Text3[i]);

    }

    Send_Instruction_Data(Set, ROW2);

    for (int i = 0; i < strlen(Text4); i++) {

        Send_Instruction_Data(Write, Text4[i]);

    }

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
