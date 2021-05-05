#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <util/delay.h>
#include <avr/io.h>

#include "radio_tx.h"

// ADC0 = Port C, Pin 0 ANALOG
// ADC1 = Port C, Pin 1 ANALOG

#define NUM_SAMPLES 32
#define SOCKET_OFF   5
#define SOCKET_ON   10

void adc_init()
{
    // Prescaler 128
    ADCSRA = (1 << ADEN) | (0 << ADATE) | (0 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR);
    DDRC &= (0 << 0) | (0 << 1);
    PORTC |= (1 << 0) | (1 << 1);
}

uint8_t get_adc_value(uint8_t mux)
{
    ADMUX &= 0b11110000;
    ADMUX |= (mux & 0b00001111);
    ADCSRA |= (1 << ADSC); // Start conversion
    while ((ADCSRA & (1 << ADIF)) == 0); // Wait until conversion has finished
    ADCSRA &= ~(1 << ADIF); // Clear flag
    return ADCH;
}

int main()
{
    adc_init();
    radio_init_tx();

    uint16_t a = 0;
    uint16_t b = 0;

    while (true)
    {
        uint16_t new_a = 0;
        uint16_t new_b = 0;
        for (size_t i = 0; i < NUM_SAMPLES; i++)
        {
            new_a += get_adc_value(0);
            new_b += get_adc_value(1);
        }
        new_a /= NUM_SAMPLES;
        new_b /= NUM_SAMPLES;

        if (new_a <= SOCKET_OFF && a > SOCKET_OFF)
        {
            radio_socket_off();
        }
        else
        {
            if (new_a >= SOCKET_ON && a < SOCKET_ON)
            {
                radio_socket_on();
            }
        }

        a = new_a;
        b = new_b;

        uint8_t to_send[] = { a, b };
        radio_send(to_send, 2);
    }

    return 0;
}
