#include <xc.h>
#include <stdint.h>
#include "segments.h"
void tabla(uint8_t dec) {
    switch (dec){
        case 0:
            PORTC = 0b00111111;
            break;
        case 1:
            PORTC = 0b00000110;
            break;
        case 2:
            PORTC = 0b01011011;
            break;
        case 3:
            PORTC = 0b01001111;
            break;
        case 4:
            PORTC = 0b01100110;
            break;
        case 5:
            PORTC = 0b01101101;
            break;
        case 6:
            PORTC = 0b01111101;
            break;
        case 7:
            PORTC = 0b00000111;
            break;
        case 8:
            PORTC = 0b01111111;
            break;
        case 9:
            PORTC = 0b01101111;
            break;
        case 10:
            PORTC = 0b01110111;
            break;
        case 11:
            PORTC = 0b01111100;
            break;
        case 12:
            PORTC = 0b00111001;
            break;
        case 13:
            PORTC = 0b01011110;
            break;
        case 14:
            PORTC = 0b01111001;
            break;
        case 15:
            PORTC = 0b01110001;
            break;
    }
}


