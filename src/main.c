#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "servo.h"
#include "lcd.h"
#include "millis.h"

// B (digital pin 8 to 13)
// C (analog input pins)
// D (digital pins 0 to 7)
#define DEBUG_LED_PIN PB4
#define BUTTON_PIN 1


#define BIT_SET(a, b) (a |= (1U << b))
#define BIT_CLEAR(a, b) (a &= ~(1U << b))
#define BIT_FLIP(a, b) (a ^= (1U << b))
#define BIT_CHECK(a, b) (a & (1U << b)) 
const uint8_t rowPins[3] = {PB0, PB1, PB2};
const uint8_t columnPins[3] = {PC0, PC1, PC2};
#define BUTTON_IS_CLICKED(PINB, BUTTON_PIN) !BIT_CHECK(PINB, BUTTON_PIN)
#define rowPin1 PD2
#define rowPin2 PD3
#define rowPin3 PD4
#define columnPin1 PC0
#define columnPin2 PC1
#define columnPin3 PC2

// char matrix[3][3] = {{'1','2','3'},
//                     {'4','5','6'},
//                     {'7','8','9'}};
// this to hold locked or unlocked value 0 or 1
enum lockstatus {

    unlocked = 0,
    locked = 1
};
// holds the chars for matrix
char matrix[3][3] = {{'1','4','7'},
                    {'2','5','8'},
                    {'3','6','9'}};

char buttonmatrix() {
for (int i = 0; i < 3; i++) {
    // Clear all rows
    PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2);
    PORTB &= ~(1 << rowPins[i]);
    for (int j = 0; j < 3; j++) {
        if (!BIT_CHECK(PINC, columnPins[j])) { // active LOW
            _delay_ms(20); // debounce
            if (!BIT_CHECK(PINC, columnPins[j])) {
                while (!BIT_CHECK(PINC, columnPins[j])) {} // wait until release
                    return matrix[i][j];
                }
            }
        }
    }
    return '\0'; // no key pressed
}
int main(void) {
    lcd_init();
    lcd_puts("Safe is:");
    lcd_set_cursor(0,1);
    lcd_printf("locked");
    lcd_clear();
    init_serial();
    init_servo();
    millis_init();
    sei();
    // declaring millis so i can have noninterupting delays
    millis_t millis_since_last_change = 0;
    millis_t current_millis = 0;
    millis_t millis_since_last_clear = 0;
    millis_t current_millis_clear = 0;
    
    //
    DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
    DDRC  &= ~((1<<PC0)|(1<<PC1)|(1<<PC2));       // columns input
    PORTC |= (1<<PC0)|(1<<PC1)|(1<<PC2);         // pull-ups
    
    DDRB |= (1<<DEBUG_LED_PIN);                   // debug LED output
    PORTB |= (1<<DEBUG_LED_PIN);
    
    // declaring all variables needed for the loop
    char rightcode[] = "1234";
    char code[5] = "";
    int idx = 0;
    int mainscreenclear = 2000;
    enum lockstatus status = locked;
    
    while(1)
    {
        
        current_millis = millis_get();
        current_millis_clear = millis_get();
        // adds char key to the code array        
        char key = buttonmatrix();
        if(key){

            code[idx++] = key;
            code[idx] = '\0';
        }
        if(current_millis_clear - millis_since_last_clear >= mainscreenclear){

            lcd_clear();
            // lcd screen
            lcd_set_cursor(0,0);
            lcd_printf("code:");
            lcd_set_cursor(0,1);
            lcd_printf(code);

            millis_since_last_clear = current_millis_clear;
            mainscreenclear = 5000;
        }

        if(idx == 4){
            
            lcd_clear();
            // unlocks the safe
            if(strncmp(code, rightcode, 4) == 0 && status == 1){
                BIT_FLIP(PORTB, DEBUG_LED_PIN);
                servo1_set_percentage(100);
                
                // lcd screen
                lcd_set_cursor(0,0);
                lcd_printf("Safe is:");
                lcd_set_cursor(0,1);
                lcd_printf("unlocked");
                status = unlocked;
            }
            else{ // if the code is not correct

                lcd_puts("wrong code");
                _delay_ms(2000);
                lcd_clear();
            }

            idx = 0;
            code[0] = '\0';
            millis_since_last_change = current_millis;
        }
        // locks after 5 sec
        if(status == 0 && current_millis - millis_since_last_change >= 5000){

            lcd_clear();
            BIT_FLIP(PORTB, DEBUG_LED_PIN);
            servo1_set_percentage(0);

            // lcd screen
            lcd_set_cursor(0,0);
            lcd_printf("Safe is:");
            lcd_set_cursor(0,1);
            lcd_printf("locked");
            status = locked;

            lcd_clear();
        }
    }
    return 0;
}
