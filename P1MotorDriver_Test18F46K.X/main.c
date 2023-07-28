/*
 * File:   main.c
 * Author: Alejandro
 *
 * Created on June 8, 2023, 2:08 PM
 */

//  Fuses. 
#include <xc.h>
#include <stdio.h>

//  Register configuration. 
#pragma config FOSC = INTIO67   //  Internal crystal configuration.
#pragma config WDTEN = OFF  //  Watch dog timmer disabled. 
#pragma config LVP = OFF    // Low voltage programming disabled. 

#define _XTAL_FREQ 16000000 //  Internal crystal frequency configuration (16Mhz). 
#define Time 1000

//  Functions prototype.
void Configurations(void);

//Global Variables.
int8_t X = 0b00001010; //Another way to express an integer data. Example: int  x  = 0. 
int8_t Counter = 0;

//  Main function. 

void main(void) {

    //  Calling functions. 
    Configurations();

    //  Infinite loop. 
    while (1) {
        //Platform_Instruction();
        LATAbits.LA6 ^= 1; //  This statement change the logic state in specific pin of port A. 
        __delay_ms(100); //  A little delay function. 

    }
}

//  This function set the registers bit by bit.

void Configurations(void) {

    OSCCON = 0x52; //  This register set oscillator. 
    
    //  Port A settings.
    ANSELA = 0x00; //  This register set the pins of port A as digital. (Digital = 0 / Analog = 1)
    TRISA = 0x00; //  This register set the pins of port A as outputs. (Output = 0 / Input = 1)
    LATA = 0X00; //  This register set the pin status in 0V (Security reason start in 0V). (Logic status High = 1 / Logic status Low = 0)
    
    //  Port B settings. 
    ANSELBbits.ANSB0 = 0; //This statement set specific pin of port B(Bit 0), as a digital. (We can set specific pin with this register)
    TRISBbits.RB0 = 1; 
    LATBbits.LATB0 = 0;
    
    //  Interruptions settings on pin 0 of port B (RB0). 
    GIE = 1; // (Global Interrupt Enable) Register to enable the global interrupts.
    INT0IE = 1; //  (Interrupt Enable) Register to enable external interrupts on pin 0 of port B (RB0). 
    INT0IF = 0; //  (Interrupt Flag) Register to enable and set the flag of interrupt. Initialize to 0.
    INTEDG0 = 0; // (Interrupt Edge) Register to enable and set according to the edge.

}

//  Interruption Function.
/*
 Important note: This function won't be declared. 
 */

void __interrupt() Test(void) {
    /*
     * Only a button we can test a motor driver L298N. 
     */

    if (INT0IF) { // This statement evaluate the change of state of the Interruption Flag on pin 0. 

        INT0IF = 0; // Clean the flag. 

        Counter++; // This variable count the pulses of the button in RB0.

        switch (Counter) { 
            case 1:
                LATA = 0b00001001;
                __delay_ms(Time);
                break;

            case 2:
                LATA = 0b00000110;
                __delay_ms(Time);
                break;

            case 3:
                LATA = 0b00001000;
                __delay_ms(Time);
                break;

            case 4:
                LATA = 0b00000010;
                __delay_ms(Time);
                break;

            case 5:
                LATA = 0b00000000;
                __delay_ms(Time);
                Counter = 0;
                break;

        }

    }

}

//  This function works with drivers output. 

/*  Important Note: To use this expressions on the keyboard are the next: 
        XOR --> Alt + 92 = ^
        Bitwise NOT --> Alt + 126 = ~
 */