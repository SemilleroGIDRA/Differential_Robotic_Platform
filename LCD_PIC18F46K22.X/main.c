/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on January 12, 2024, 9:16 AM
 */


#include <xc.h> //Library to Microchip Microcontrollers.
#include <string.h>
#include "Fuses_Set.h" //Library to set fuses of the microcontroller. 

//Instructions of the LCD 
#define CLR 0x01 //Function to clear the LCD. 
#define ROW1 0X80
#define ROW2 0xC0
#define ROW3 0X94
#define ROW4 0XD4
#define CD 0x01 //Command to Clear Display.
#define RH 0x02 //Command to Return to Home that is the first position of the LCD (inside first raw and column).
#define EMS 0x06 //Command Entry Mode Set to use moving direction cursor. 
#define DC 0x0F //Command Display Control to set display, cursor and blink. 
#define DSR 0x1C //Command to control the cursor shift to Right (Display Shift Right).
#define DSL 0x18 //Command to control the cursor shift to Left (Display Shift Left). 
#define FS 0x28 //Command to control the 4/8 bits mode, 5x8/5x11 character, 1/2 lines display. 
//Pins of the LCD 
#define RS LATCbits.LATC4 //Define pin (RC4) Register Select as bit flag.
#define EN LATCbits.LATC5 //Define pin (RC5) Enable as bit flag.   


//Prototype Functions.
void Configurations(void); //Function to set registers.
void Init_LCD(void); //Function to initialize the LCD. 
void Set_Instruction(unsigned char S_Instruction);
void Write_Instruction(unsigned char W_Instruction);
void LCD_Instructions(unsigned char Instruction);
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

    Set_Instruction(0x02);
    Set_Instruction(EMS);
    Set_Instruction(DC);
    Set_Instruction(FS);
    Set_Instruction(CD);

}

//Develop initialize LCD function.

void Init_LCD(void) {

    //    Set_Instruction(0x02);
    //    Set_Instruction(EMS);
    //    Set_Instruction(DC);
    //    Set_Instruction(FS);
    //    Set_Instruction(CD);
    //    __delay_ms(20);
    //    Set_Instruction(0x30);
    //    __delay_ms(5);
    //    Set_Instruction(0x30);
    //    __delay_ms(5);
    //    Set_Instruction(0x30);
    //    Set_Instruction(0x28);
    //    Set_Instruction(0x0F);
    //    Set_Instruction(CLR);
    //    __delay_ms(15);

}


//Develop 

void Set_Instruction(unsigned char S_Instruction) {

    RS = 0;
    LCD_Instructions(S_Instruction >> 4);
    //    LCD_Instructions(S_Instruction & 0x0F);
    LCD_Instructions(S_Instruction);


}

//Develop

void Write_Instruction(unsigned char W_Instruction) {

    RS = 1;
    LCD_Instructions(W_Instruction >> 4);
    //    LCD_Instructions(W_Instruction & 0x0F);
    LCD_Instructions(W_Instruction);

}

//Develop 

void LCD_Instructions(unsigned char Instruction) {

    EN = 1;
    __delay_ms(15);
    LATD = Instruction;
    __delay_ms(15);
    EN = 0;
    __delay_ms(15);

}

//Function to show data on LCD. 

void Test(void) {

    Set_Instruction(ROW3);

    for (int i = 0; i < strlen(Text1); i++) {

        Write_Instruction(Text1[i]);

    }

    __delay_ms(100);

    Set_Instruction(ROW4);

    for (int j = 0; j < strlen(Text2); j++) {

        Write_Instruction(Text2[j]);

    }

}