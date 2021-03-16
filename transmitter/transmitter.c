#define F_CPU 16000000L
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <util/delay.h>
#include <avr/io.h>

#include "radio_tx.h"
#include "../common/logger.h"
#include "../common/protocol.h"

// ADC0 = Port C, Pin 0 ANALOG
// ADC1 = Port C, Pin 1 ANALOG

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
    logger_init();

    logger_print("Transmitter started!\n");

    /*while (true)
    {
        PORTD = (1 << 3);
        _delay_us(PULSE_US * 10);
        PORTD = 0;
        _delay_us(PULSE_US * 55);
    }*/

    while (true)
    {
        char *to_send = "Henlo, smol bean!";
        //logger_printf("%d %d\n", to_send[0], to_send[1]);
        radio_send((uint8_t*)to_send, strlen(to_send) + 1);
    }

    return 0;
}
