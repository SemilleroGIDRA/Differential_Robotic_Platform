/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on February 14, 2024, 6:11 PM
 */


#include <xc.h>
#include <stdint.h>
#include "Fuses_Configuration.h"
#include "LCD_Library.h"

//Driver LN298 commands to move platform.
#define Forward 0b10100000
#define Backward 0b01010000
#define Shift_Right 0x08 
#define Shift_Left 0x02
#define Right_Motor LATEbits.LATE0
#define Left_Motor LATEbits.LATE2

//Prototype functions. 
void Configurations(void); //Function to set registers.
void Receive_Interrupt(void);
void Moving_Platform(unsigned char Command);
void Init_Message_Platform(void);
void __interrupt(high_priority) Interrupt_Rx(void);
void __interrupt(low_priority) Interrupt(void);

//Global variables.
unsigned char Rx_Buffer;

//Main function.

void main(void) {

    //Call functions. 
    Configurations();
    Init_LCD();

    //Infinite Loop. 
    while (1) {



    }

}

//Develop interrupt high priority function.

void __interrupt(high_priority) Interrupt_Rx(void) {

    if (PIR1bits.RC1IF) { //Check interrupt has been activated. 

        Receive_Interrupt();

    }

}

//Develop interrupt low priority function.

void __interrupt(low_priority) Interrupt(void) {



}

//Develop configurations function

void Configurations(void) {

    OSCCON = 0x72; //Internal oscillator 
    //Set pins as digital
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    //Set pins as outputs
    TRISCbits.RC4 = 0;
    TRISCbits.RC5 = 0;
    TRISD = 0;
    TRISE = 0;

    //Clean pins 
    LATCbits.LC4 = 0;
    LATCbits.LC5 = 0;
    LATD = 0;
    LATE = 0;

    //---- Interrupts Configurations ----
    INTCONbits.GIE = 1; //Global Interrupt Enabled. 
    INTCONbits.PEIE = 1; //Peripheral Interrupt Enabled. 
    RCONbits.IPEN = 1; //Interrupt Priority Enabled. 

    PIE1bits.RC1IE = 1; //Receive Enabled. 
    PIR1bits.RC1IF = 0; //Receive Flag cleared. 

    //---- EUSART Configurations ----
    SPBRG = 103; //Configuration 9600 bauds. 
    TRISCbits.RC6 = 1; //Enable pin RC6 (TX1).
    TRISCbits.RC7 = 1; //Enable pin RC7 (RX1).

    //---- Transmitter configuration TXSTA1 (TX). 
    TXSTA1bits.TX91 = 0; //9 bits transmitter disabled. 
    TXSTA1bits.TXEN1 = 1; //Transmit Enabled. 
    TXSTA1bits.SYNC1 = 0; //Asynchronous mode. 
    TXSTA1bits.BRGH1 = 1; //High baud rate. 

    //---- Receive configuration RCSTA1 register (RX).
    RCSTAbits.SPEN1 = 1; //Serial port enable bit. 
    RCSTAbits.RX91 = 0; //9 bits transmitter disabled. 
    RCSTAbits.CREN1 = 1; //Receive continuous enabled. 

    //---- Baud configuration BAUDCON1. 
    BAUDCON1bits.BRG16 = 0; //16 bits mode disabled. 

    /*
     
     Remember data buffers. 
     * 
        RCREG1; ----> Receive buffer.
        TXREG1; ----> Transmitter buffer.
      
     */

    //----PWM Configurations ----
    PR2 = 0xF9; //Set period signal.
    CCP3CON = 0x0C; //Half to duty cycle of 50% and PWM mode. 
    CCPR3L = 0x7D; //Send Duty cycle of 50%
    T2CON = 0x02; //TMR off, prescaler 16. 
    T2CONbits.TMR2ON = 1; //Turn on timer 2. 

}

void Receive_Interrupt(void) {

    Rx_Buffer = RCREG1;

    switch (Rx_Buffer) {

        case 'M':
            Moving_Platform(Forward);
            
            break;

        case 'A':
            Moving_Platform(Backward);
            break;

        default:
            LATD = 0x00;
            break;

    }

}

void Moving_Platform(unsigned char Command) {

    Right_Motor = 1;
    Left_Motor = 1;
    LATD = Command;

}

//Develop message platform function

void Init_Message_Platform(void) {

    Send_Instruction_Data(Set, ROW1);
    Send_String("Research Project");
    Send_Instruction_Data(Set, ROW2);
    Send_String("Robotic Platform");

}