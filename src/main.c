#define F_CPU 16000000L
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define BUTTON_DDR     DDRD
#define BUTTON_PORT    PORTD
#define BUTTON_PIN     PIND
#define BUTTON_INDEX   2
#define LONG_PRESS_MS  1000
#define WAIT_INTVAL_MS 100

#define STATUS_DDR   DDRB
#define STATUS_PORT  PORTB
#define STATUS_INDEX 5

#define LIGHTSTRIP_DDR    DDRB
#define LIGHTSTRIP_PORT   PORTB
#define LIGHTSTRIP_INDEX  2
#define NUM_LEDS          118
#define BUFFER_SIZE       (NUM_LEDS * 3)

#define NUM_PATTERNS 3

extern void output_grb(uint8_t *buffer, uint16_t bytes);
uint8_t colors[BUFFER_SIZE];
uint8_t current_pattern;

uint16_t pressed_ms;

// Button handling

void button_init()
{
    cli();
    BUTTON_DDR &= ~(1 << BUTTON_INDEX); // Input
    BUTTON_PORT |= (1 << BUTTON_INDEX); // Pullup
    PCICR |= 0b00000100;                // Use Port D for pin state change interrupt
    PCMSK2 |= (1 << BUTTON_INDEX);      // Bitmask for used pin
    // Configure Timer 0
    TCCR0A = (1 << WGM01);              // CTC mode
    TCCR0B = (1 << CS02) | (1 << CS00); // 1024 prescaler
    OCR0A = 155;                        // Timer interrupt every 100ms
    TCNT0 = 0;
    TIMSK0 |= (0 << OCIE0A);            // Disable timer
    sei();
}

bool is_pressed()
{
    if ((~BUTTON_PIN & (1 << BUTTON_INDEX)) != 0)
    {
        return true;
    }
    return false;
}

// Timer interrupt
ISR(TIMER0_COMPA_vect)
{
    // Interrupt occurs every 10ms
    pressed_ms += 10;
}

// Pin state change interrupt
ISR(PCINT2_vect)
{
    if (is_pressed())
    {
        // Start timer
        TIMSK0 |= (1 << OCIE0A);
    }
    else
    {
        // Stop timer and change pattern
        TIMSK0 |= (0 << OCIE0A);
        if (pressed_ms >= LONG_PRESS_MS)
        {
            current_pattern = 0;
        }
        else
        {
            current_pattern++;
            current_pattern %= NUM_PATTERNS;
        }
        pressed_ms = 0;
        TCNT0 = 0;
    }
}

// End button handling

void lightstrip_show()
{
    output_grb(colors, BUFFER_SIZE);
    LIGHTSTRIP_PORT &= ~(1 << LIGHTSTRIP_INDEX);
    _delay_us(60);
}

void lightstrip_init()
{
    LIGHTSTRIP_DDR  |= (1 << LIGHTSTRIP_INDEX);
    LIGHTSTRIP_PORT &= ~(1 << LIGHTSTRIP_INDEX);
}

int main()
{
    STATUS_DDR |= (1 << STATUS_INDEX);   // Set debugging LED as output
    STATUS_PORT &= ~(1 << STATUS_INDEX); // Turn LED off
    button_init();
    current_pattern = 0;
    
    while (true)
    {
        switch (current_pattern)
        {
            case 0:
                STATUS_PORT |= (1 << STATUS_INDEX);
                _delay_ms(100);
                STATUS_PORT &= ~(1 << STATUS_INDEX);
                _delay_ms(100);
                break;
            case 1:
                STATUS_PORT |= (1 << STATUS_INDEX);
                _delay_ms(500);
                STATUS_PORT &= ~(1 << STATUS_INDEX);
                _delay_ms(500);
                break;
            case 2:
                STATUS_PORT |= (1 << STATUS_INDEX);
                _delay_ms(1000);
                STATUS_PORT &= ~(1 << STATUS_INDEX);
                _delay_ms(1000);
                break;
            default:
                break;
        }
    }
}
