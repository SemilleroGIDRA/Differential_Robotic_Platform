///*
// * File:   main.c
// * Author: Alejandro
// *
// * Created on May 3, 2023, 2:11 PM
// */
//
//
//#include <xc.h>
//
//void main(void) {
//    return;
//}

#include <xc.h>
#include <string.h>
#include <stdio.h>

#pragma config FOSC = INTIO67
#pragma config WDTEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 16000000
#define time 10

//LCD
#define CD 0x01
#define RH 0x02
#define EMS 0x06
#define DC 0x0F
#define DSr 0x1C
#define DSl 0x18
#define FS 0x28
#define RAW1 0x80
#define RAW2 0xC0
#define E LATE0
#define RS LATE1

void settings(void);
//LCD
void SettingsLCD(unsigned char word);
void WriteLCD(unsigned char word);
void LCD(unsigned char word);
void DataLCD(void);
void Data1LCD(void);
//INTERRUPCION
void __interrupt() RXTX(void);
unsigned char d, servo;
char text[20], text2[20];
int confir, i;
int flag = 0;
float a, b, c;
char textx[40] = {"coordenadas iniciales X/Y"};
char texty[40] = {"coordenadas finales X/Y"};

void main(void) {
    settings();
    while (1) {
        DataLCD();
        Data1LCD();
    }
}

void settings(void) {
    OSCCON = 0x72;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;
    TRISCbits.RC5 = 0;
    LATCbits.LC5 = 0;
    TRISD = 0x00;
    TRISE = 0x00;
    LATD = 0x00;
    LATE = 0x00;
    //LCD
    SettingsLCD(0x02);
    SettingsLCD(EMS);
    SettingsLCD(DC);
    SettingsLCD(FS);
    SettingsLCD(CD);
    //UART
    TRISCbits.RC7 = 1;
    TRISCbits.RC6 = 0;
    SPBRG1 = 0x19;
    RCSTA1 = 0x90;
    TXSTA1 = 0x20;
    //interrupcion
    GIE = 1;
    PEIE = 1;
    RCIE = 1;
    RCIF = 0;
}

void SettingsLCD(unsigned char word) {
    RS = 0;
    LCD(word >> 4);
    LCD(word & 0x0F);
}

void WriteLCD(unsigned char word) {
    RS = 1;
    LCD(word >> 4);
    LCD(word & 0x0F);
}

void LCD(unsigned char data) {
    E = 1;
    __delay_us(time);
    LATD = data;
    __delay_us(time);
    E = 0;
    __delay_us(time);
}

void __interrupt() RXTX(void) {
    if (RC1IF) {
        d = RCREG1;
        if (d == 'S') {
            __delay_ms(1000);
            for (i = 0; i <= strlen(textx); i++) {
                while (TX1IF == 0);
                TXREG1 = textx[i];
            }
            while (TX1IF == 0);
            TXREG1 = 0x0D; //retorno de carro
            __delay_ms(100);
        }
        if (d == 'X' || d == 'Y') {
            servo = d;
            confir = 0;
        } else if (d == '0' || d == '1' || d == '2' || d == '3' || d == '4' || d == '5' || d == '6' || d == '7' || d == '8' || d == '9') {
            if (flag == 1) {
                a = d - 48;
                flag = 2;
            } else if (flag == 2) {
                a = (a * 10) + (d - 48);
            }//Decimales
            else if (flag == 3) {
                a = a + (d - 48)*0.1;
                flag = 4;
            } else if (flag == 4) {
                a = a + (d - 48)*0.01;
                flag = 5;
            }
        } else if (d == '.') {
            flag = 3;
        } else if (d == 'O') {
            if (servo == 'X') {
                b = a;
            } else if (servo == 'Y') {
                c = a;
            }
            flag = 1;
            a = 0.0;
        } else if (d == 'K') {
            confir = 1;
            __delay_ms(1000);
            for (i = 0; i <= strlen(texty); i++) {
                while (TX1IF == 0);
                TXREG1 = texty[i];
            }
            while (TX1IF == 0);
            TXREG1 = 0x0D; //retorno de carro
            __delay_ms(100);
        }
    }
}

void DataLCD(void) {
    SettingsLCD(RAW1);
    sprintf(text, "X: %.1f", b);
    for (i = 0; i <= strlen(text); i++) {
        WriteLCD(text[i]);
    }
}

void Data1LCD(void) {
    SettingsLCD(RAW2);
    sprintf(text2, "Y: %.1f", c);
    for (i = 0; i <= strlen(text2); i++) {
        WriteLCD(text2[i]);
    }
}