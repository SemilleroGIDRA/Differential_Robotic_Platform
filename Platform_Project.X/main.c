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
#define IN1 LATDbits.LD4 
#define IN2 LATDbits.LD5
#define IN3 LATDbits.LD6
#define IN4 LATDbits.LD7
#define Right_Motor_Enable LATEbits.LATE0 //Macro to enable right output of driver
#define Left_Motor_Enable LATEbits.LATE2 //Macro to enable left output of driver
#define Cycle_100 1023.00 //Macro to use 100% of PWM signal.

//Prototype functions. 
void Configurations(void); //Function to set registers.
void Receive_Interrupt(void);
void Moving_Platform(unsigned char Command);
void Init_Message_Platform(void);
void Set_PWM_Right_Motor(float value1_pwm);
void Set_PWM_Left_Motor(float value2_pwm);
void __interrupt(high_priority) Interrupt_Rx(void);
void __interrupt(low_priority) Interrupt(void);

//Global variables.
unsigned char Rx_Buffer;
float Duty_Cycle1, Duty_Cycle2;

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
    TRISCbits.RC4 = 0; //Pins to LCD 
    TRISCbits.RC5 = 0;
    TRISD = 0; //Pins configuration to LCD and Motor driver. 
    TRISE = 0; //Pins configuration to PWM.

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
    PR2 = 0xF9; //Set period signal (1 ms) 8 bits using.
    T2CON = 0x00; //Timer 2 configuration. TMR off, prescaler 16.
    CCP3CON = 0x0C; //Duty cycle of 100%. 
    CCPR3L = 0xFA; //Send Duty cycle of 100%.
    CCP5CON = 0x0C; //Duty cycle of 100%.
    CCPR5L = 0xFA; //      
    T2CONbits.TMR2ON = 1; //Turn on timer 2. 

}

void Receive_Interrupt(void) {

    Rx_Buffer = RCREG1;

    switch (Rx_Buffer) {

        case 'M':
            //Moving_Platform(Forward);

            Set_PWM_Right_Motor(800.00);
            IN1 = 0;
            IN2 = 1;
            Set_PWM_Left_Motor(800.00);
            IN3 = 1;
            IN4 = 0;

            __delay_ms(5000);
            
            break;

        default:
            
            IN1 = 0; 
            IN2 = 0; 
            IN3 = 0; 
            IN4 = 0; 
            
            break;

    }

}

//Develop to set PWM for right motor.

void Set_PWM_Right_Motor(float value1_pwm) {

    Duty_Cycle1 = (float) (value1_pwm * (1000.00 / Cycle_100));
    CCPR3L = (int) Duty_Cycle1 >> 2;
    CCP3CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle1 & 0x03) << 4));

}

//Develop to set PWM for left motor.

void Set_PWM_Left_Motor(float value2_pwm) {

    Duty_Cycle2 = (float) (value2_pwm * (1000.00 / Cycle_100));
    CCPR5L = (int) Duty_Cycle2 >> 2;
    CCP5CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle2 & 0x03) << 4));

}

void Moving_Platform(unsigned char Command) {

    //    Right_Motor = 1;
    //    Left_Motor = 1;
    //    LATD = Command;

}

//Develop message platform function

void Init_Message_Platform(void) {

    Send_Instruction_Data(Set, ROW1);
    Send_String("Research Project");
    Send_Instruction_Data(Set, ROW2);
    Send_String("Robotic Platform");

}