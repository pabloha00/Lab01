/*
 * File:   Lab 1
 * Author: Pablo Herrarte
 * Curso: Electrónica Digital 2 
 * Fecha: 21/07/2021
 */

//IMPORTACIÓN DE LIBRERÍAS
#include <xc.h>
#include <stdint.h>
#include "segments.h" //Llamamos a la librería



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


// DECLARACIÓN DE VARIABLES
uint8_t anlec = 0; //Variable analógica
uint8_t mux = 0; //Variable para los transistores
uint8_t nowadc = 0; //Variable para que el adc haga su función
uint8_t HNibble = 0; //Variable para los 4 bits más significativos de la señal analógica
uint8_t LNibble = 0; //Variable para los 4 bits menos significativo
uint8_t AR1 = 0; //Variables para el antirrebote 
uint8_t AR2 = 0;
uint8_t AR3 = 0;
uint8_t AR4 = 0;

// PROTOTIPO DE FUNCIONES
void Setup(void); //Setup
void ADC(void); //Función del ADC
void Alarm(void); //Función para encender la alarma

//VECTOR DE INTERRUPCIÓN
void __interrupt() ISR(void){
    if(INTCONbits.TMR0IF == 1){
        INTCONbits.TMR0IF = 0;
        TMR0 = 236;
        nowadc++; //Se va sumando el valor de la variable de la ADC cada vez que el timer 0 hace un ciclo
        PORTEbits.RE0 = 0;
        PORTEbits.RE1 = 0; //Se colocan los bits que van a servir para los tranistores del 7 segmentos en 0
        if (mux == 0){ //Cuando esta variable es 0, la señal que habilita el transistor del 7 segmentos más significativo se activa
            mux = 1;
            PORTEbits.RE0 = 1; 
            HNibble = (anlec & 0b11110000) >>4; //Lee los 4 bits más significativos de la señal analógica
            tabla(HNibble); //Se utiliza la librería para encender los pines deseados
        }
        else{
            mux = 0; //Cuando mux es 0 se habilita el transistor del 7 segmentos menos significativo
            PORTEbits.RE1 = 1;
            LNibble = anlec & 0b00001111; //Lee los 4 bits menos significativos de la señal analógica
            tabla(LNibble); //Se utiliza la librería para encender los pines deseados
        }
    }
    if (PIR1bits.ADIF == 1){ //Si la conversión AD fue realizada se regresa la bandera a 0
        PIR1bits.ADIF = 0;
        anlec = ADRESH; //Señal analógica
        
    }
    if (INTCONbits.RBIF == 1){ //Cambia puerto B
        INTCONbits.RBIF = 0;
        //Anti rebote de los dos botones
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
    Setup(); //Setup
    while (1){ //Ciclo infinito
    ADC(); //Función ADC
    Alarm(); //Función de alarma

}
}
//FUNCIONES
void Setup(void){
    //CONFIG I&0
    PORTA = 0; //Potenciometro
    PORTB = 0; //Botones
    PORTC = 0; //7 segmentos
    PORTD = 0; //Contador
    PORTE = 0; //Alarma & 2 Trans
    ANSEL = 0;
    ANSELbits.ANS0 = 1; //Entrada analógica
    ANSELH = 0;
    TRISA = 1; //Input
    TRISB = 0b11111111; //Intputs
    TRISC = 0; //Outputs
    TRISD = 0; //Outputs
    TRISE = 0; //Outputs
    INTCONbits.TMR0IF = 0; //Interrupciones
    INTCONbits.RBIF = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.RBIE = 1;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    WPUBbits.WPUB0 = 1; //Weak pull-ups
    WPUBbits.WPUB1 = 1;
    PIR1bits.ADIF = 0; //Función AD lisa para comenzar
    OSCCONbits.IRCF0 = 0; //Configuración del oscilador
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;
    ADCON1 = 0;
    ADCON0 = 0b01000001; //Control de operación ADC
    TMR0 = 236; //Donde comienza el timer 0
       
 
    
    OPTION_REG = 0b01010111; //Configuración de timer 0 y pull ups
}

void ADC(void){ //Función de ADC
    if (nowadc > 5){ //Si la variable nowadc es mayor a 5 regresará a 0 y comienza la converción AD
     nowadc = 0;
     ADCON0bits.GO_nDONE = 1;
    }
}
void Alarm(void){ //Función de alarma
    if(anlec > PORTD){ //Si el puerto D es menor a la señal analógica se encenderá el pin de la alarma
        PORTEbits.RE2 = 1;        
    }
    else{
        PORTEbits.RE2 = 0;
    }
}