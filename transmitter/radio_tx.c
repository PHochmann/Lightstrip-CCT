#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "radio_tx.h"
#include "../common/protocol.h"

#define RADIO_TX_PIN 3

void radio_init_tx()
{
    DDRD |= (1 << RADIO_TX_PIN);
    PORTD &= ~(1 << RADIO_TX_PIN);
}

void send_byte(uint8_t byte)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        if ((byte & (1 << i)) != 0)
        {
            PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) | (1 << RADIO_TX_PIN);
            _delay_us(ONE_HIGH * PULSE_US);
            PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) & ~(1 << RADIO_TX_PIN);
            _delay_us(ONE_LOW * PULSE_US);
        }
        else
        {
            PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) | (1 << RADIO_TX_PIN);
            _delay_us(ZERO_HIGH * PULSE_US);
            PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) & ~(1 << RADIO_TX_PIN);
            _delay_us(ZERO_LOW * PULSE_US);
        }
    }
}

void radio_send(uint8_t *buffer, size_t bytes)
{
    PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) | (1 << RADIO_TX_PIN);
    _delay_us(START_HIGH * PULSE_US);
    PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) & ~(1 << RADIO_TX_PIN);
    _delay_us(START_LOW * PULSE_US);

    uint8_t checksum = 0;
    for (size_t i = 0; i < bytes; i++)
    {
        send_byte(buffer[i]);
        checksum ^= buffer[i];
    }

    send_byte(checksum);

    PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) | (1 << RADIO_TX_PIN);
    _delay_us(END_HIGH * PULSE_US);
    PORTD = (PORTD & ~(1 << RADIO_TX_PIN)) & ~(1 << RADIO_TX_PIN);
    _delay_us(END_LOW * PULSE_US);
}
