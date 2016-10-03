#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu_speed.h"
#include <util/delay.h>
#include "graphics.h"
#include "lcd.h"
#include <avr/interrupt.h>


void setup(void);
void setup_timer(void);
void debounce(int p);
void process_btn(void);
void process_t(void);

volatile int i;

int main(){
    i = 0;
    setup();
    setup_timer();
    while(1){
        //clear_screen();
        //process_btn();
        //process_t();
        //show_screen();
        //_delay_ms(100);
    }

    return 0;
}

/*
This function to display the counter of timer0
*/
void process_t(){
    char cnt[80];
    sprintf(cnt,"%d",TCNT0);
    draw_string(20,10,cnt);

}

/*
This function is to setupu the switches and the LEDs and the LCD. 
Reuse it if you like (don't forget to setup sw3 as well)
*/
void setup(){
    set_clock_speed(CPU_8MHz);
    DDRB |= 1 << PIN2 | 1 << PIN3;
    DDRF &= ~ (1 << PIN5);
    DDRC  |= 1 << PIN7;
    PORTC |= 1 << PIN7;
    lcd_init(LCD_DEFAULT_CONTRAST);
    clear_screen();
    draw_string(10,10,"hi");
    show_screen();


}

/*
This function is just to declutter the code.
*/
void process_btn(){
    if (PINF >> PIN5 & 0b1){
        debounce(PIN5);
        PORTB ^= 1 << PIN2 | 1 << PIN3;
    }
}

/*
This function to setup the timer.
*/
void setup_timer(){
    TCCR0B &= ~(1 << WGM02);
    TCCR0B |= 1 << CS00 | 1 << CS02;
    TCCR0B &= ~(1 << CS01);

    TIMSK0 |= 1 << TOIE0;

    sei();
}
/*
this is a very simplified version to do debouncing
A more involved way to do it can be found here
http://www.avrfreaks.net/sites/default/files/forum_attachments/debounce.pdf
However, it follows the same concept.
*/
void debounce(int p){
  _delay_ms(50);
  while(PINF >> p & 0b1);
  _delay_ms(50);
}

/*
This the example suggested in the lecture. It also demonstrate the volatile variable i.
*/
ISR(TIMER0_OVF_vect){
    i++;
    clear_screen();
    char cnt[80];
    sprintf(cnt,"%d",i);
    draw_string(20,10,cnt);
    show_screen();
}


