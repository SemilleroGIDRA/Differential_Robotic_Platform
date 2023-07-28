/*
 * File:   main.c
 * Author: Alejandro
 *
 * Created on May 9, 2023, 6:53 PM
 */



#include <xc.h>
#include <stdio.h>
#include <string.h>

#pragma config FOSC = INTIO67
#pragma config WDTEN = OFF
#pragma config LVP = OFF
#pragma config CCP2MX = PORTB3
#pragma config CCP3MX = PORTE0

#define _XTAL_FREQ 4000000
#define time 100
//LCD
#define CD 0x01
#define RH 0x02
#define EMS 0x06
#define DC 0x0E
#define DSr 0x1C
#define DSl 0x18
#define FS 0x28
#define RAW1 0x80
#define RAW2 0xC0
//#define RAW3 0x94
//#define RAW4 0xD4
#define RS LATB0
#define E LATB1

//constant (MOTOR)
#define in1 LATA4
#define in2 LATA5
#define in3 LATA6
#define in4 LATA7

void settings(void);
//modes
void Manual(void);
void automatic(void);
//PWM
void PWM_increase(void);
void PWM(void);
void PWMI(void);
void PWMD(void);
//movements
void forward(void);
void back(void);
void right(void);
void left(void);
void brake(void);
//LCD
void SettingsLCD(unsigned char word);
void WriteLCD(unsigned char word);
void LCD(unsigned char data);
void Data1LCD(void);
void Data2LCD(unsigned char pwm);
void Data3LCD(void);
void Data4LCD(void);

void ups(void);
void ups2(void);
//Interruption
void __interrupt(high_priority) UART_Rx(void);
void __interrupt(low_priority) sensor(void);

unsigned int t1, t3;
int i, digital, digital1, digital2, binario, cont = 0, mood = 0;
int s = 0, x, a, t = 0;
float b, conversion, conversion1, conversion2, conversion3, conversion4, DutyCycle, DutyCycle1, DutyCycle2, voltage, distance1, distance2, delay, fI, wI, t2, t4, fD, wD;
char text[10] = {"MI:"}, text2[10] = {"MD:"}, text3[20], text4[20], buffer[15], text5[10], text6[10];
unsigned char flag = 0, flag1 = 0, flag2 = 0, canal = 0, m, n;

void main(void) {
    settings();
    while (1) {
        //Data3LCD();
        if (mood == 1) {
            if (s == 0) { //hay presencia
                LATAbits.LA2 = 1;
                LATAbits.LA3 = 0;
                LATBbits.LB2 = 0;
                Manual();
            } else if (s == 1) { //no hay presencia
                LATBbits.LB2 = 1;
                brake();
            }
        } else if (mood == 2) {
            if (s == 0) { //hay presencia
                LATAbits.LA3 = 1;
                LATAbits.LA2 = 0;
                LATBbits.LB2 = 0;
                automatic();
            } else if (s == 1) { //no hay presencia
                LATBbits.LB2 = 1;
                brake();
            }
        }
    }
}

void settings(void) {
    OSCCON = 0x52;
    ANSELA = 0x01;
    ANSELB = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
    TRISA = 0b00000001;
    TRISB = 0xF0;
    TRISCbits.TRISC2 = 1;
    TRISD = 0;
    TRISE = 0x04;
    LATA = 0;
    LATB = 0x00;
    LATD = 0;
    LATE = 0;
    //ADC
    ADCON0 = 0x01;
    ADCON1 = 0x00;
    ADCON2 = 0x94;
    //PWM f=1kHz
    PR2 = 0x3E;
    CCPR2L = 0x2E;
    CCPR3L = 0x00;
    T2CON = 0x02;
    TMR2 = 0;
    CCP2CON = 0x3C;
    CCP3CON = 0x0C;
    //LCD
    SettingsLCD(0x02);
    SettingsLCD(EMS);
    SettingsLCD(DC);
    SettingsLCD(FS);
    SettingsLCD(CD);
    Data1LCD();
    //Serial UART
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    SPBRG1 = 25;
    RCSTA1 = 0x90;
    TXSTA1 = 0x24;
    //Interruption
    GIE = 1;
    PEIE = 1;
    RCONbits.IPEN = 1;
    RC1IE = 1;
    RC1IF = 0;
    RC1IP = 1;
    //RB Interruption
    RBIE = 1;
    RBIF = 0;
    IOCB = 0xF0; //11110000
    RBIP = 0;
    //Encoder interruption
    CCP1IE = 1;
    CCP1IF = 0;
    CCP5IE = 1;
    CCP5IF = 0;
    //CCP1 - CCP5 Configuration
    CCP1CON = 0x05;
    CCPR1 = 0x00;
    CCP5CON = 0x05;
    CCPR5 = 0x00;
    //TMR1 Configuration
    T1CON = 0x32;
    TMR1 = 0;
    T5CON = 0x32;
    TMR5 = 0;
    TMR1ON = 1;
    TMR5ON = 1;
    //TMR2
    TMR2ON = 1;
    ADCON0bits.GO = 1;
    //Interrupcion TMR3
    TMR3IE = 1;
    TMR3IF = 0;
    TMR3IP = 0;
    //TMR3
    T3CON = 0x12;
    TMR3 = 0x3CAF;
    //TMR3ON = 1;
}

void SettingsLCD(unsigned char word) {
    RS = 0;
    LCD(word >> 4); // 4 MSB
    LCD(word & 0x0F); // 4 LSB
}

void WriteLCD(unsigned char word) {
    RS = 1;
    LCD(word >> 4);
    LCD(word & 0x0F);
}

void LCD(unsigned char data) { //Opción bits
    E = 1;
    __delay_us(time);
    LATDbits.LATD0 = (data & 0x01);
    __delay_us(time);
    LATDbits.LATD1 = (data & 0x02) >> 1;
    __delay_us(time);
    LATDbits.LATD2 = (data & 0x04) >> 2;
    __delay_us(time);
    LATDbits.LATD3 = (data & 0x08) >> 3;
    __delay_us(time);
    E = 0;
    __delay_us(time);
}

void __interrupt(high_priority) UART_Rx(void) {
    unsigned char a;
    if (RC1IF == 1) {
        a = RCREG1;
        if (a == 'm' || a == 'M') {//Manual mode
            mood = 1;
        } else if (a == 'a' || a == 'A') {//Automatic mode
            mood = 2;
        } else if (a == 's' || a == 'S') {//Semi-automatic mode
            mood = 3;
        } else if (a == '0') {//stop
            m = 0;
        } else if (a == '1') {//Forward movement
            m = 1;
        } else if (a == '2') {//Move backward
            m = 2;
        } else if (a == '3') {//Movement left
            m = 3;
        } else if (a == '4') {//Movement right
            m = 4;
        } else if (a == '5') { //Increase PWM1
            n = 1;
        } else if (a == '6') { //Decrease PWM1
            n = 2;
        } else if (a == '7') { //Increase PWM2
            n = 3;
        } else if (a == '8') { //Decrease PWM2
            n = 4;
        }
    }
    if (CCP1IF == 1) {
        t1 = TMR1;
        t2 = (float) t1 * 8.0;
        CCPR1 = 0;
        TMR1 = 0;
        CCP1IF = 0;
    }
    if (CCP5IF == 1) {
        t3 = TMR5;
        t4 = (float) t3 * 8.0;
        CCPR5 = 0;
        TMR5 = 0;
        CCP5IF = 0;
    }
}

void __interrupt(low_priority) sensor(void) {
    if (RBIF) {
        RBIF = 0;
        if ((PORTB & 0xF0) != 0x00) { //Detecta flanco de subida (No ha detectado presencia)
            s = 0;
        } else if ((PORTB & 0xF0) != 0xF0) { //Detecta flanco de bajada (Se detecta presencia)
            s = 1;
            //brake();
        }
    }
    if (TMR3IF) {
        //TMR3ON = 1;
        TMR3 = 0X3CAF;
        t++;
        LATAbits.LA1 = ~LATAbits.LA1;
        TMR3IF = 0;
    }
}

void start(void) {
    ADCON0bits.GO = 1; //habilita la conversion en el ADCON0
    while (GO == 1); //mientras se ejecute el factor de conversion
    __delay_ms(time); //tiempo
    a = ADRESH << 8 | ADRESL; //con esta operacion se justifica el bit 7 del ADCON2 (ADFM = todos los 10 bits del conversor a la derecha)
    b = (float) a * (5.0 / 1023.0); //operacion conversora (el valor de a es el valor del conversor, h es un rango y 1023 es el valor maximo del conversor)
    __delay_us(3); // Para comenzar la siguiente adquisición se requiere un tiempo mínimo de 3 TAD
}

void Manual(void) {
    if (m == 0) {///quiescent
        brake();
    } else if (m == 1) {//Forward platform
        if (s == 0) {
            forward();
        } else {
            ups2();
        }
    } else if (m == 2) {//Back platform
        if (s == 0) {
            back();
        } else {
            ups2();
        }
    } else if (m == 3) {//Turn left platform
        if (s == 0) {
            left();
        } else {
            ups2();
        }
    } else if (m == 4) {//Right turn platform
        if (s == 0) {
            right();
        } else {
            ups2();
        }
    }
    PWM_increase();
}

void ups(void) {
    TMR3ON = 0;
    while (1) {
        brake();
        if (s == 0) {//no hay presencia
            TMR3ON = 1;
            break;
        }
    }
}

void ups2(void) {
    while (1) {
        brake();
        Data3LCD();
        if (s == 0) {//no hay presencia
            Data4LCD();
            break;
        }
    }
}

void automatic(void) {
    for (i = 0; i <= 3; i++) {
        while (b >= 4.5) {
            start();
        }
        t = 0;
        TMR1 = 0x3CAF;
        TMR3ON = 1;
        //*****************
        while (t <= 24) {//timer es de 100ms
            //Data4LCD();
            if (s == 0) {
                forward();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 8) {
            //Data4LCD();
            if (s == 0) {
                left();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 22) {
            //Data4LCD();
            if (s == 0) {
                forward();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 29) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        //*****************
        start();
        while (b < 4.5) {
            start();
        }
        //*****************
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 8) {
            //Data4LCD();
            if (s == 0) {
                left();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 19) {
            //Data4LCD();
            if (s == 0) {
                forward();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 8) {
            //Data4LCD();
            if (s == 0) {
                left();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 18) {
            //Data4LCD();
            if (s == 0) {
                forward();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 20) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 8) {
            //Data4LCD();
            if (s == 0) {
                left();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
        while (t <= 50) {
            //Data4LCD();
            if (s == 0) {
                brake();
            } else {
                ups();
            }
        }
        t = 0;
        TMR1 = 0x3CAF;
    }
}

void PWM(void) {
    if (flag == 0) {
        digital = 511; //50% ciclo útil
        flag = 1;
    }
    conversion = ((float) digital * (250.0 / 1023.0));
    DutyCycle = conversion * (100.0 / 250.0);
    CCPR2L = ((int) conversion >> 2);
    CCP2CON = (CCP2CON & 0x0F) | (((int) conversion & 0x03) << 4);
    Data2LCD(2);
    CCPR3L = ((int) conversion >> 2);
    CCP3CON = (CCP3CON & 0x0F) | (((int) conversion & 0x03) << 4);
    Data2LCD(3);
}

void PWMI(void) {//PWM motor (izquierda de la plataforma)
    if (flag1 == 0) {
        digital1 = 256; //25% ciclo útil (motor 1)
        digital2 = 767; //75% ciclo útil (motor 2)
        flag1 = 1;
    }
    flag2 = 0;
    conversion1 = ((float) digital1 * (250.0 / 1023.0));
    DutyCycle = conversion1 * (100.0 / 250.0);
    CCPR2L = ((int) conversion1 >> 2);
    CCP2CON = (CCP2CON & 0x0F) | (((int) conversion1 & 0x03) << 4);
    Data2LCD(2);
    conversion2 = ((float) digital2 * (250.0 / 1023.0));
    DutyCycle = conversion2 * (100.0 / 250.0);
    CCPR3L = ((int) conversion2 >> 2);
    CCP3CON = (CCP3CON & 0x0F) | (((int) conversion2 & 0x03) << 4);
    Data2LCD(3);
}

void PWMD(void) {//PWM motor (derecha de la plataforma)
    if (flag2 == 0) {
        digital2 = 256; //25% ciclo útil (motor 2)
        digital1 = 767; //75% ciclo útil (motor 1)
        flag2 = 1;
    }
    flag1 = 0;
    conversion3 = ((float) digital1 * (250.0 / 1023.0));
    DutyCycle = conversion3 * (100.0 / 250.0);
    CCPR2L = ((int) conversion3 >> 2);
    CCP2CON = (CCP2CON & 0x0F) | (((int) conversion3 & 0x03) << 4);
    Data2LCD(2);
    conversion4 = ((float) digital2 * (250.0 / 1023.0));
    DutyCycle = conversion4 * (100.0 / 250.0);
    CCPR3L = ((int) conversion4 >> 2);
    CCP3CON = (CCP3CON & 0x0F) | (((int) conversion4 & 0x03) << 4);
    Data2LCD(3);
}

void PWM_increase(void) {
    if (n == 1) {//+PWM1
        n = 0;
        if (m == 1 || m == 2) { //Forward and backward platform
            digital += 100;
            if (digital > 1023) {
                digital = 1023;
            }
        } else if (m == 3) {//Left turn platform
            digital1 += 100;
            if (digital1 >= digital2) {
                digital1 -= 100;
            }
        } else if (m == 4) {//Right turn platform
            digital1 += 100;
            if (digital1 > 1023) {
                digital1 = 1023;
            }
        }
    } else if (n == 2) { //-PWM1
        n = 0;
        if (m == 1 || m == 2) {//Forward and backward platform
            digital -= 100;
            if (digital < 0) {
                digital = 0;
            }
        } else if (m == 3) {//Left turn platform
            digital1 -= 100;
            if (digital1 < 0) {
                digital1 = 0;
            }
        } else if (m == 4) {//Right turn platform
            digital1 -= 100;
            if (digital1 <= digital2) {
                digital1 += 100;
            }
        }
    } else if (n == 3) {//+PWM2
        n = 0;
        if (m == 1 || m == 2) {///Forward and backward platform
            digital += 100;
            if (digital > 1023) {
                digital = 1023;
            }
        } else if (m == 3) {//Left turn platform
            digital2 += 100;
            if (digital2 > 1023) {
                digital2 = 1023;
            }
        } else if (m == 4) {//Right turn platform
            digital2 += 100;
            if (digital2 >= digital1) {
                digital2 -= 100;
            }
        }
    } else if (n == 4) {//IOC3 //-PWM2
        n = 0;
        if (m == 1 || m == 2) {//Forward and backward platform
            digital -= 100;
            if (digital < 0) {
                digital = 0;
            }
        } else if (m == 3) {//Left turn platform
            digital2 -= 100;
            if (digital2 <= digital1) {
                digital2 += 100;
            }
        } else if (m == 4) {//Right turn platform
            digital2 -= 100;
            if (digital2 < 0) {
                digital2 = 0;
            }
        }
    }
}

void Data1LCD(void) {
    SettingsLCD(RAW1);
    for (i = 0; i <= strlen(text); i++) {
        WriteLCD(text[i]);
    }
    SettingsLCD(RAW2);
    for (i = 0; i <= strlen(text2); i++) {
        WriteLCD(text2[i]);
    }
}

void Data2LCD(unsigned char pwm) {
    if (pwm == 2) {
        SettingsLCD(RAW1 + 4);
        fI = (1.0 / (t2 * 0.000001));
        wI = (fI * 60.0) / 20.0;
        sprintf(buffer, "%.2f%% %d RPM", DutyCycle, (int) wI);
    } else if (pwm == 3) {
        SettingsLCD(RAW2 + 4);
        fD = (1.0 / (t4 * 0.000001));
        wD = (fD * 60.0) / 20.0;
        sprintf(buffer, "%.2f%% %d RPM", DutyCycle, (int) wD);
    }
    for (i = 0; i <= strlen(buffer); i++) {
        WriteLCD(buffer[i]);
    }
}

void Data3LCD(void) {
//    SettingsLCD(RAW1); //posicion 10 linea superior
//    sprintf(text5, "A"); //transformacion caracteres variables
//    for (i = 0; i <= strlen(text5); i++) {//bucle para el string
//        WriteLCD(text5[i]); //funcion de escritur
//    }
}

void Data4LCD(void) {
//    SettingsLCD(RAW2); //posicion 10 linea superior
//    sprintf(text6, "C"); //transformacion caracteres variables
//    for (i = 0; i <= strlen(text6); i++) {//bucle para el string
//        WriteLCD(text6[i]); //funcion de escritur
//    }
}

void forward(void) {//Funciones de motor
    __delay_ms(100);
    PWM();
    in1 = 1;
    in2 = 0;
    in3 = 1;
    in4 = 0;

}

void back(void) {
    __delay_ms(100);
    PWM();
    in1 = 0;
    in2 = 1;
    in3 = 0;
    in4 = 1;

}

void right(void) {
    PWMD();
    in1 = 1;
    in2 = 0;
    in3 = 1;
    in4 = 0;

}

void left(void) {

    PWMI();
    in1 = 1;
    in2 = 0;
    in3 = 1;
    in4 = 0;

}

void brake(void) {
    in1 = 0;
    in2 = 0;
    in3 = 0;
    in4 = 0;
}

