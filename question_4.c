/*  CAB202 Tutorial 9
*	Question 4 - Solution
*
*	B.Talbot, May 2016
*	Queensland University of Technology
*/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "graphics.h"
#include "cpu_speed.h"
#include "sprite.h"

/*
* Useful defines you can use in your system time calculations
*/
#define FREQUENCY 8000000.0
#define PRESCALER 1024.0

/*
* Definitions for the states of the buttons
*/
#define NUM_BUTTONS 6
#define BTN_DPAD_LEFT 0
#define BTN_DPAD_RIGHT 1
#define BTN_DPAD_UP 2
#define BTN_DPAD_DOWN 3
#define BTN_LEFT 4
#define BTN_RIGHT 5

#define BTN_STATE_UP 0
#define BTN_STATE_DOWN 1

/*
* Variables used in recording the state of the buttons. Note the use of volatile
* keyword. For arrays this keyword isn't necessary (consider why...), but we leave
* it there to show how to use volatile Sprite variables with the "cab202_teensy"
* library
*/
volatile Sprite btn_sprites[NUM_BUTTONS];
volatile unsigned char btn_hists[NUM_BUTTONS];
volatile unsigned char btn_states[NUM_BUTTONS];

volatile unsigned int press_count = 0;

/*
* Bitmaps for visualising the button states
*/
unsigned char bm_pressed[] = {
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111
};
unsigned char bm_unpressed[] = {
    0b11111111,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b10000001,
    0b11111111
};

/*
* Function declarations
*/
void init_hardware(void);

double get_system_time(unsigned int timer_count);

/*
* Main - initialise the hardware, and run the code that increments the face's
* current position and draws it.
*/
int main() {
    set_clock_speed(CPU_8MHz);

    // Create and initialise the sprites
    // NOTE: our volatile Sprite pointers are explicitly cast back to a Sprite
    // pointer to remove the "volatile" qualifier. This is because our functions
    // in the library do not explicitly support volatile arguments!
    init_sprite((Sprite*) &btn_sprites[BTN_DPAD_LEFT], 0, 32, 8, 8, bm_unpressed);
    init_sprite((Sprite*) &btn_sprites[BTN_DPAD_RIGHT], 16, 32, 8, 8, bm_unpressed);
    init_sprite((Sprite*) &btn_sprites[BTN_DPAD_UP], 8, 24, 8, 8, bm_unpressed);
    init_sprite((Sprite*) &btn_sprites[BTN_DPAD_DOWN], 8, 40, 8, 8, bm_unpressed);
    init_sprite((Sprite*) &btn_sprites[BTN_LEFT], 60, 32, 8, 8, bm_unpressed);
    init_sprite((Sprite*) &btn_sprites[BTN_RIGHT], 76, 32, 8, 8, bm_unpressed);

    // Setup the hardware
    init_hardware();

    // Run the main loop
    char buff[80];
    while (1) {
        // Decide what bitmap we should have
        for (unsigned char i = 0; i< NUM_BUTTONS; i++) {
            btn_sprites[i].bitmap = (btn_states[i] == BTN_STATE_DOWN) ? bm_pressed : bm_unpressed;
        }

        // Draw the screen
        clear_screen();
        for (unsigned char i = 0; i < NUM_BUTTONS; i++) {
            // NOTE: the same casting technique is used as mentioned above.
            draw_sprite((Sprite*) &btn_sprites[i]);
        }
        sprintf(buff, "%5.4f", get_system_time(TCNT1));
        draw_string(54, 0, buff);
        sprintf(buff, "%4d", press_count);
        draw_string(0, 0, buff);
        show_screen();

        // Have a rest
        _delay_ms(50);
    }

    // We'll never get here...
    return 0;
}

/*
* Function implementations
*/
void init_hardware(void) {
    // Initialising the LCD screen
    lcd_init(LCD_DEFAULT_CONTRAST);
    DDRC  |= 1 << PIN7;
    PORTC |= 1 << PIN7;
    // Initalising the buttons as inputs
    // TODO
    DDRF &= ~((1 << PF5) | (1 << PF6));
    DDRB &= ~((1 << PB1) | (1 << PB7));
    DDRC &= ~((1 << PD0) | (1 << PD1));

    // Initialising the LEDs as outputs
    // TODO
    DDRB |= ((1 << PB2) | (1 << PB3));

    // Configure all necessary timers in "normal mode", enable all necessary
    // interupts, and configure prescalers as desired
    // TODO
    TCCR1B &= ~(1 << WGM02); // Normal Mode

    TCCR1B |= 1 << CS00 | 1 << CS02; // Set prescaler to 1024
    TCCR1B &= ~(1 << CS01); // Set prescaler to 1024

    TIMSK1 |= 1 << TOIE1; // Set Interrupt overflow


    TCCR0B &= ~(1 << WGM02);
    TCCR0B |= 1 << CS02;
    TCCR0B &= ~(1 << CS00);
    TCCR0B &= ~(1 << CS01);

    TIMSK0 |= 1 << TOIE0;

    // Globally enable interrupts
    // TODO
    sei();
}

double get_system_time(unsigned int timer_count) {
    // Based on the current count, frequency, and prescaler - return the current
    // count time in seconds
    // TODO
    return timer_count/7812.5;
}

/*
* Interrupt service routines
*/
// TODO
ISR(TIMER0_OVF_vect) {
    // Interrupt service routine for TIMER1. Toggle an LED everytime this ISR runs
    // TODO

    PORTB ^= 1 << PB2;
    for (int i = 0; i < NUM_BUTTONS; i++){
        btn_hists[i]=btn_hists[i]<<1;
    }

    btn_hists[BTN_DPAD_LEFT]  |= ((PINB>>PIN1)&1)<<0;
    btn_hists[BTN_DPAD_RIGHT] |= ((PIND>>PIN0)&1)<<0;
    btn_hists[BTN_DPAD_UP]    |= ((PIND>>PIN1)&1)<<0;
    btn_hists[BTN_DPAD_DOWN]  |= ((PINB>>PIN7)&1)<<0;
    btn_hists[BTN_LEFT]       |= ((PINF>>PIN6)&1)<<0;
    btn_hists[BTN_RIGHT]      |= ((PINF>>PIN5)&1)<<0;

    for (int i = 0; i < NUM_BUTTONS; i++){
        if (btn_hists[i] == 0xFF && btn_states[i] == BTN_STATE_UP) {
            btn_states[i] = BTN_STATE_DOWN;
            press_count++;
            PORTB ^= 1 << PB3;
        } else if (btn_hists[i] == 0x0 && btn_states[i] == BTN_STATE_DOWN) {
            btn_states[i] = BTN_STATE_UP;
        }
    }
}
