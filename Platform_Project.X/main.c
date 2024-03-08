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
//--> Macros to control motors
#define IN1 LATDbits.LD4 
#define IN2 LATDbits.LD5
#define IN3 LATDbits.LD6
#define IN4 LATDbits.LD7
// --> Macros to control PWM
#define Duty_Cycle_100 1023.00 //Macro to use 100% of PWM signal.
#define Duty_Cycle_75 767.25 //Macro to use 75% of PWM signal.
#define Duty_Cycle_50 511.5 //Macro to use 50% of PWM signal.
#define Duty_Cycle_25 255.75 //Macro to use 25% of PWM signal.
#define Duty_Cycle_0 0.00 //Macro to stop platform. 
#define Manual_Mode 'M' //Macro to check manual mode. 
#define Auto_Mode 'A' //Macro to check automatic mode. 
#define Semi_Mode 'S' //Macro to check semiautomatic mode. 
#define Move_Forward 'F' //Macro to move platform forward.
#define Move_Backward 'B' //Macro to move platform backward. 
#define Move_Right 'R' //Macro to move platform to the right.
#define Move_Left 'L' //Macro to move platform to the left. 
#define STOP 'T' //Macro to stop platform. 

//Prototype functions. 
void Configurations(void); //Function to set registers.
void Bluetooth_Interrupt(void); //Function to EUSART module. 
void Init_Message_Platform(void); //Function to test LCD.
void Send_PWM_Motors(float PWM_RMotor, float PWM_LMotor); //Function to send PWM to each motor. 
void Manage_Motor_Direction(char in1, char in2, char in3, char in4); //Function to control polarity of the motors. 

//Global variables.
unsigned char Rx_Buffer; //Variable to read RCREG1 register. 
float Duty_Cycle1, Duty_Cycle2; //Variables to save PWM from equation. 

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

    Bluetooth_Interrupt();

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
    CCPR5L = 0xFA; //Send Duty cycle of 100%.      
    T2CONbits.TMR2ON = 1; //Turn on timer 2. 

}

void Bluetooth_Interrupt(void) {

    if (PIR1bits.RC1IF) { //Check interrupt has been activated. 

        Rx_Buffer = RCREG1; //Assign RCREG1 buffer to clean the flag. 

        if (Rx_Buffer == Manual_Mode) {

            Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
            Manage_Motor_Direction(1, 0, 0, 1); //Forward Instruction. 

        } else if (Rx_Buffer == Auto_Mode) {

            Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
            Manage_Motor_Direction(0, 1, 1, 0); //Forward Instruction.

        } else if (Rx_Buffer == STOP) {

            Send_PWM_Motors(Duty_Cycle_0, Duty_Cycle_0);
            Manage_Motor_Direction(0, 0, 0, 0); //Forward Instruction.

        }

    }

}



//Develop function to send PWM to the motors. 

void Send_PWM_Motors(float PWM_RMotor, float PWM_LMotor) {

    Duty_Cycle1 = (float) (PWM_RMotor * (1000.00 / 1023.00)); //Assign to the Duty_Cycle1 PWM signal. 
    CCPR3L = (int) Duty_Cycle1 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR3L.
    CCP3CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle1 & 0x03) << 4)); //Send the rest of the bits to CCP3CON. 

    Duty_Cycle2 = (float) (PWM_LMotor * (1000.00 / 1023.00)); //Assign to the Duty_Cycle2 PWM signal. 
    CCPR5L = (int) Duty_Cycle2 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR5L.
    CCP5CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle2 & 0x03) << 4)); //Send the rest of the bits to CCP5CON. 

}

//Develop function to control direction 

void Manage_Motor_Direction(char in1, char in2, char in3, char in4) {

    IN1 = in1;
    IN2 = in2;
    IN3 = in3;
    IN4 = in4;

}

//Develop message platform function

void Init_Message_Platform(void) {

    Send_Instruction_Data(Set, ROW1);
    Send_String("Research Project");
    Send_Instruction_Data(Set, ROW2);
    Send_String("Robotic Platform");

}