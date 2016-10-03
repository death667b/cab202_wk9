#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>
#include "lcd.h"
#include "graphics.h"
#include "sprite.h"
#include "cpu_speed.h"


/* this takes care of DDR for SWs and LED and LCD*/
void setup(void);
/*
void setup_timer(void);
void debounce(int p);
// these functions will be filled in the lecture
//
void process_DB(void);
void process_T(void);
*/
int main(){
    setup();

    while(1){
    }

    return 0;
}


void setup(){
    set_clock_speed(CPU_8MHz);
    DDRB |=    1 << PIN2 | 1 << PIN3 ;
    DDRF &= ~( 1 << PIN5 | 1 << PIN6);

    DDRC  |= 1 << PIN7;
    PORTC |= 1 << PIN7;
    lcd_init(LCD_DEFAULT_CONTRAST);
    clear_screen();
    show_screen();
}

