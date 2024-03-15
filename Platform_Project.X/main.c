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
// ---- Macros to control motors
#define IN1 LATDbits.LD4 
#define IN2 LATDbits.LD5
#define IN3 LATDbits.LD6
#define IN4 LATDbits.LD7
// ---- Macros to control PWM
#define Duty_Cycle_100 1023.0 //Macro to use 100% of PWM signal.
#define Duty_Cycle_75 767.2 //Macro to use 75% of PWM signal.
#define Duty_Cycle_50 511.5 //Macro to use 50% of PWM signal.
#define Duty_Cycle_25 255.7 //Macro to use 25% of PWM signal.
#define Duty_Cycle_0 0.00 //Macro to stop platform. 
#define Manual_Mode 'M' //Macro to check manual mode. 
#define Auto_Mode 'A' //Macro to check automatic mode. 
#define Semi_Mode 'S' //Macro to check semiautomatic mode. 
#define Move_Forward 'F' //Macro to move platform forward.
#define Move_Backward 'B' //Macro to move platform backward. 
#define Move_Right 'R' //Macro to move platform to the right.
#define Move_Left 'L' //Macro to move platform to the left. 
#define STOP 'T' //Macro to stop platform. 
#define Exit_Mode 'e' //Macro to exit from the mode.

//Prototype functions. 
void Configurations(void); //Function to set registers.
void Bluetooth_Receiver(void); //Function to EUSART module. 
void Init_Message_Platform(void); //Function to test LCD.
void Driver_Control(float PWM_RMotor, float PWM_LMotor, unsigned char Direction); //Function to send PWM to each motor and manage platform direction. 
void Platform_Mode(unsigned char Data); //Function to enable mode in the platform. 
void Manual(unsigned char Data); //Function to use manual mode. 
void Automatic(void); //Function to use automatic mode. 
void Semi_Automatic(void); //Function to use semiautomatic. 

//Global variables.
unsigned char Rx_Buffer; //Variable to read RCREG1 register. 
unsigned char Platform_Status = 'i'; //Variable to detect what is the current state of the platform and assign the initial value. 
unsigned char Manual_Direction;
float Duty_Cycle1, Duty_Cycle2; //Variables to save PWM from equation. 

//Main function.

void main(void) {

    //Call functions. 
    Configurations();
    Init_LCD();

    //Infinite Loop. 
    while (1) {

        Platform_Mode(Platform_Status); //Call function to check the platform status. 

    }

}

//Develop interrupt high priority function.

void __interrupt(high_priority) Interrupt_Rx(void) {

    Bluetooth_Receiver();

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

//Reception data interrupt function.

void Bluetooth_Receiver(void) {

    if (PIR1bits.RC1IF) { //Check interrupt has been activated. 

        Rx_Buffer = RCREG1; //Assign RCREG1 buffer to clean the flag.

        if (Rx_Buffer == Manual_Mode) {

            Send_Instruction_Data(Set, CLR);
            Send_Instruction_Data(Set, ROW1);
            Send_String("   Manual Mode");
            Manual_Direction = '0';
            Platform_Status = Manual_Mode;


        } else if (Rx_Buffer == Auto_Mode) {

            Send_Instruction_Data(Set, CLR);
            Send_Instruction_Data(Set, ROW2);
            Send_String("Automatic Mode");
            Platform_Status = Auto_Mode;

        } else if (Rx_Buffer == Semi_Mode) {

            Send_Instruction_Data(Set, CLR);
            Send_Instruction_Data(Set, ROW3);
            Send_String("Semi Mode");
            Platform_Status = Semi_Mode;

        } else if (Rx_Buffer == Move_Forward) {

            Manual_Direction = Move_Forward;

        } else if (Rx_Buffer == Move_Backward) {

            Manual_Direction = Move_Backward;

        } else if (Rx_Buffer == STOP) {

            Manual_Direction = STOP;

        } else if (Rx_Buffer == Move_Right) {

            Manual_Direction = Move_Right;

        } else if (Rx_Buffer == Move_Left) {

            Manual_Direction = Move_Left;

        } else if (Rx_Buffer == 'E') {

            Manual_Direction = 'e';

        }

    }
}



//Develop function to send PWM to the motors. 

void Driver_Control(float PWM_RMotor, float PWM_LMotor, unsigned char Direction) {

    //PWM configuration. 
    Duty_Cycle1 = (float) (PWM_RMotor * (1000.0 / 1023.0)); //Assign to the Duty_Cycle1 PWM signal. 
    CCPR3L = (int) Duty_Cycle1 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR3L.
    CCP3CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle1 & 0x03) << 4)); //Send the rest of the bits to CCP3CON. 

    Duty_Cycle2 = (float) (PWM_LMotor * (1000.0 / 1023.0)); //Assign to the Duty_Cycle2 PWM signal. 
    CCPR5L = (int) Duty_Cycle2 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR5L.
    CCP5CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle2 & 0x03) << 4)); //Send the rest of the bits to CCP5CON. 

    //Check direction.
    if (Direction == Move_Forward) {

        //Enable motor controller direction. 
        IN1 = 1;
        IN2 = 0;
        IN3 = 0;
        IN4 = 1;

    } else if (Direction == Move_Backward) {

        IN1 = 0;
        IN2 = 1;
        IN3 = 1;
        IN4 = 0;

    } else if (Direction == Move_Right) {

        IN1 = 0;
        IN2 = 1;
        IN3 = 1;
        IN4 = 0;

    } else if (Direction == Move_Left) {

        IN1 = 0;
        IN2 = 1;
        IN3 = 1;
        IN4 = 0;

    } else if (Direction == STOP) {

        IN1 = 0;
        IN2 = 0;
        IN3 = 0;
        IN4 = 0;

    }

}

//Develop message platform function

void Init_Message_Platform(void) {

    Send_Instruction_Data(Set, ROW1);
    Send_String("Research Project");
    Send_Instruction_Data(Set, ROW2);
    Send_String("Robotic Platform");

}

//Develop Manual Mode of the platform.

void Manual(unsigned char Data) {

    //Check conditions. 
    if (Data == Move_Forward) {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_100, Move_Forward);

    } else if (Data == Move_Backward) {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_100, Move_Backward);

    } else if (Data == STOP) {

        Driver_Control(Duty_Cycle_0, Duty_Cycle_0, STOP);

    } else if (Data == Move_Right) {

        Driver_Control(Duty_Cycle_25, Duty_Cycle_100, Move_Left);

    } else if (Data == Move_Left) {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_25, Move_Right);

    } else if (Data == 'e') {

        Platform_Status = 'i';

    }

}

//Develop Automatic Mode of the platform. 

void Automatic(void) {



}

//Develop Semi-Automatic Mode of the platform. 

void Semi_Automatic(void) {



}

//Develop function to test state of the platform.

void Platform_Mode(unsigned char Data) {

    if (Data == Manual_Mode) {

        Manual(Manual_Direction);

    } else if (Data == Semi_Mode) {

        Semi_Automatic(); 

    } else if (Data == Auto_Mode) {

        Automatic(); 

    }

}
