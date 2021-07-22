/*
 * File:   main.c
 * Author: user
 *
 * Created on 19 de julio de 2021, 09:49 PM
 */

//IMPORTACIÓN DE LIBRERÍAS
#include <xc.h>
#include <stdint.h>
#include "segments.h"
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// PALABRAS DE CONFIGURACIÓN
#pragma config FOSC = XT        // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
// DECLARACIÓN DE VARIABLES
uint8_t anlec = 0;
uint8_t mux = 0;
uint8_t nowadc = 0;
uint8_t HNibble = 0;
uint8_t LNibble = 0;
uint8_t AR1 = 0;
uint8_t AR2 = 0;
uint8_t AR3 = 0;
uint8_t AR4 = 0;

// PROTOTIPO DE FUNCIONES
void Setup(void);
void ADC(void);
void Alarm(void);

//VECTOR DE INTERRUPCIÓN
void __interrupt() ISR(void){
    if(INTCONbits.TMR0IF == 1){
        INTCONbits.TMR0IF = 0;
        TMR0 = 236;
        nowadc++;
        PORTEbits.RE0 = 0;
        PORTEbits.RE1 = 0;
        if (mux == 0){
            mux = 1;
            PORTEbits.RE0 = 1;
            HNibble = (anlec & 0b11110000) >>4;
            tabla(HNibble);
        }
        else{
            mux = 0;
            PORTEbits.RE1 = 1;
            LNibble = anlec & 0b00001111;
            tabla(LNibble);
        }
    }
    if (PIR1bits.ADIF == 1){
        PIR1bits.ADIF = 0;
        anlec = ADRESH;
        
    }
    if (INTCONbits.RBIF == 1){
        INTCONbits.RBIF = 0;
        AR1 = PORTBbits.RB0;
        if (AR1==0){
            AR2=0;    
        }
        if (AR2==0){
            if (AR1==1){
                PORTD++;
                AR2=1;
            }
        }
        AR3 = PORTBbits.RB1;
        if (AR3==0){
            AR4=0;
        }
        if (AR4==0){
            if (AR3==1){
                PORTD--;
                AR4=1;
            }
        }
    }
}
//CICLO PRINCIPAL
void main(void) {
    Setup();
    while (1){
    ADC();
    Alarm();

}
}
//FUNCIONES
void Setup(void){
    //CONFIG I&0
    PORTA = 0; //POT
    PORTB = 0; //BOTONES
    PORTC = 0; //7SEG
    PORTD = 0; //CONT
    PORTE = 0; //Alarma & 2 Trans
    ANSEL = 0;
    ANSELbits.ANS0 = 1;
    ANSELH = 0;
    TRISA = 1; //Input
    TRISB = 0b11111111; //Intputs
    TRISC = 0; //Output
    TRISD = 0; //Output
    TRISE = 0; //Output
    INTCONbits.TMR0IF = 0;
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.RBIE = 1;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1;
    PIR1bits.ADIF = 0;
    OSCCONbits.IRCF0 = 0;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    ADCON1 = 0;
    ADCON0 = 0b01000001;
    TMR0 = 236;
       
 
    
    OPTION_REG = 0b01010111;
}

void ADC(void){
    if (nowadc > 5){
     nowadc = 0;
     ADCON0bits.GO_nDONE = 1;
    }
}
void Alarm(void){
    if(anlec > PORTD){
        PORTEbits.RE2 = 1;        
    }
    else{
        PORTEbits.RE2 = 0;
    }
}