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
void Bluetooth_Receiver(void); //Function to EUSART module. 
void Init_Message_Platform(void); //Function to test LCD.
void Driver_Control(float PWM_RMotor, float PWM_LMotor, unsigned char Direction); //Function to send PWM to each motor and manage platform direction. 
void Platform_Mode(unsigned char Data); //Function to enable mode in the platform. 
void Manual(unsigned char Data);

//Global variables.
unsigned char Rx_Buffer; //Variable to read RCREG1 register. 
unsigned char Mode;
unsigned char Direction;
float Duty_Cycle1, Duty_Cycle2; //Variables to save PWM from equation. 

//Main function.

void main(void) {

    //Call functions. 
    Configurations();
    Init_LCD();

    //Initialize Driver .
    //Driver_Control(Duty_Cycle_0, Duty_Cycle_0, STOP);

    //Initialize the driver. 
    //    Send_PWM_Motors(Duty_Cycle_0, Duty_Cycle_0);
    //    Manage_Motor_Direction(0, 0, 0, 0);

    //Initialize Driver .
    Driver_Control(Duty_Cycle_0, Duty_Cycle_0, STOP);



    //Infinite Loop. 
    while (1) {

        Manual(Direction);

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


void Bluetooth_Receiver(void) {

    if (PIR1bits.RC1IF) { //Check interrupt has been activated. 

        Rx_Buffer = RCREG1; //Assign RCREG1 buffer to clean the flag. 

        switch (Rx_Buffer) {

            case Manual_Mode:

                Send_Instruction_Data(Set, CLR);
                Send_Instruction_Data(Set, ROW1);
                Send_String("Manual Mode");
                Mode = 'm';

                break;

            case Auto_Mode:

                Send_Instruction_Data(Set, CLR);
                Send_Instruction_Data(Set, ROW2);
                Send_String("Automatic Mode");
                Mode = 'a';

                break;

            case Semi_Mode:

                Send_Instruction_Data(Set, CLR);
                Send_Instruction_Data(Set, ROW3);
                Send_String("Semi Mode");
                Mode = 's';

                break;

            case '1':

                Direction = '1';

                break;

            case '2':

                Direction = '2';

                break;

            case '3':

                Direction = '3';

                break;

            default: //Stop 

                Send_Instruction_Data(Set, CLR);
                Send_Instruction_Data(Set, ROW1);
                Send_String("   Value Entered");
                Send_Instruction_Data(Set, ROW2);
                Send_String("   Is not Valid!");

                break;

        }

    }

}


//Develop function to send PWM to the motors. 

void Driver_Control(float PWM_RMotor, float PWM_LMotor, unsigned char Direction) {

    //PWM configuration. 
    Duty_Cycle1 = (float) (PWM_RMotor * (1000.00 / 1023.00)); //Assign to the Duty_Cycle1 PWM signal. 
    CCPR3L = (int) Duty_Cycle1 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR3L.
    CCP3CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle1 & 0x03) << 4)); //Send the rest of the bits to CCP3CON. 

    Duty_Cycle2 = (float) (PWM_LMotor * (1000.00 / 1023.00)); //Assign to the Duty_Cycle2 PWM signal. 
    CCPR5L = (int) Duty_Cycle2 >> 2; //Bitwise operation to send 8 of the 10 Least significant bits to  CCPR5L.
    CCP5CON = ((CCP3CON & 0x0F) | (((int) Duty_Cycle2 & 0x03) << 4)); //Send the rest of the bits to CCP5CON. 

    //Check direction.
    if (Direction == Move_Forward) {

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

        IN1 = 1;
        IN2 = 0;
        IN3 = 0;
        IN4 = 0;

    } else if (Direction == Move_Left) {

        IN1 = 0;
        IN2 = 0;
        IN3 = 1;
        IN4 = 0;

    } else if (Direction == STOP) {

        IN1 = 0;
        IN2 = 0;
        IN3 = 0;
        IN4 = 0;

    }

}

void Receive_Interrupt(void) {

    Rx_Buffer = RCREG1; //Assign RCREG1 buffer to clean the flag. 

    //    switch (Rx_Buffer) {
    //
    //        case 'M': //Test
    //
    //            Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
    //            Manage_Motor_Direction(0, 1, 1, 0); //Backward Instruction. 
    //            __delay_ms(2000);
    //
    //            break;
    //
    //        case 'A':
    //
    //            Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
    //            Manage_Motor_Direction(1, 0, 0, 1); //Forward Instruction. 
    //            __delay_ms(2000);
    //
    //        default: //Stop 
    //
    //            Send_PWM_Motors(Duty_Cycle_0, Duty_Cycle_0);
    //            Manage_Motor_Direction(1, 0, 0, 1); //Forward Instruction. 
    //
    //            break;
    //
    //    }

    if (Rx_Buffer == 'M') {

        Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
        Manage_Motor_Direction(0, 1, 1, 0); //Backward Instruction. 
        //__delay_ms(2000);

    } else if (Rx_Buffer == 'A') {

        Send_PWM_Motors(Duty_Cycle_100, Duty_Cycle_100);
        Manage_Motor_Direction(1, 0, 0, 1); //Backward Instruction. 

    } else if (Rx_Buffer == 'S') {

        Send_PWM_Motors(Duty_Cycle_0, Duty_Cycle_0);
        Manage_Motor_Direction(0, 0, 0, 0); //Backward Instruction. 

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

    if (Data == '1') {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_100, Move_Forward);

    } else if (Data == '2') {

        Driver_Control(Duty_Cycle_0, Duty_Cycle_0, STOP);

    } else if (Data == '3') {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_100, Move_Backward);

    } else if (Data == '4') {

        Driver_Control(Duty_Cycle_50, Duty_Cycle_100, Move_Left);

    } else if (Data == '5') {

        Driver_Control(Duty_Cycle_100, Duty_Cycle_50, Move_Right);

    }

}