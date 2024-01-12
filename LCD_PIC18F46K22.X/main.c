/*
 * File:   main.c
 * Author: Alejo
 *
 * Created on January 12, 2024, 9:16 AM
 */


#include <xc.h> //Library to Microchip Microcontrollers.
#include "Fuses_Set.h" //Library to set fuses of the microcontroller. 

//Instructions of the LCD 
#define CLR 0x01 //Function to clear the LCD. 
#define ROW1 0X00
#define ROW2 0x00
#define ROW3 0X00
#define ROW4 0X00
#define LS 0x00
#define RS 0x00
#define SET 0x00

//Prototype Functions
void Configurations (void); //Function to set registers.
void Init_LCD (void); //Function to initialize the LCD. 
void Instruction_LCD (unsigned char WR_Set, unsigned char Data); //Function to send instruction to LCD. 

//Main function.
void main(void) {
    
    //Infinite Loop.
    while (1){
        
    }
    
}

//Develop configurations function. 
void Configuration (void){
    
    
    
}

//Develop initialize LCD function.
void Init_LCD (void){
    
    
    
}

//Develop the LCD instruction. 
void Instruction_LCD (unsigned char WR_Set, unsigned char Data){
    
    
    
}