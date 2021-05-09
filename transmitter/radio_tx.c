#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "radio_tx.h"
#include "../common/protocol.h"

#define RADIO_TX_PIN 3

#define TX_HIGH ((PORTD & ~(1 << RADIO_TX_PIN)) | (1 << RADIO_TX_PIN))
#define TX_LOW  ((PORTD & ~(1 << RADIO_TX_PIN)) & ~(1 << RADIO_TX_PIN))

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
            PORTD = TX_HIGH;
            _delay_us(ONE_HIGH * PULSE_US);
            PORTD = TX_LOW;
            _delay_us(ONE_LOW * PULSE_US);
        }
        else
        {
            PORTD = TX_HIGH;
            _delay_us(ZERO_HIGH * PULSE_US);
            PORTD = TX_LOW;
            _delay_us(ZERO_LOW * PULSE_US);
        }
    }
}

void radio_send(uint8_t *buffer, size_t bytes)
{
    // End low pulse
    PORTD = TX_HIGH;
    _delay_us(PULSE_US * 10);
    PORTD = TX_LOW;
    _delay_us(PULSE_US * 10);

    // Send preamble
    PORTD = TX_HIGH;
    _delay_us(START_HIGH * PULSE_US);
    PORTD = TX_LOW;
    _delay_us(START_LOW * PULSE_US);

    uint8_t checksum = 0;
    for (size_t i = 0; i < bytes; i++)
    {
        send_byte(buffer[i]);
        checksum ^= buffer[i];
    }

    send_byte(checksum);

    // Send stop sequence
    PORTD = TX_HIGH;
    _delay_us(END_HIGH * PULSE_US);
    PORTD = TX_LOW;
    _delay_us(END_LOW * PULSE_US);

    // End low pulse
    PORTD = TX_HIGH;
    _delay_us(PULSE_US);
    // Put into low until next transmission
    PORTD = TX_LOW;
}

// GT-FSI-07 Protocol
#define SOCKET_PULSE_US      100
#define SOCKET_START_HIGH     30
#define SOCKET_START_LOW      70
#define SOCKET_ONE_HIGH        5
#define SOCKET_ONE_LOW        10
#define SOCKET_ZERO_HIGH      10
#define SOCKET_ZERO_LOW        5
#define SOCKET_PAYLOAD_LENGTH 24
#define SOCKET_NUM_REPEATS     5

// GT-FSI-07 Group 4 switching codes
const uint32_t  on = 0b000101101011001001100000;
const uint32_t off = 0b000110000000110101010000;

void socket_send(uint32_t code)
{
    for (size_t repeat = 0; repeat < SOCKET_NUM_REPEATS; repeat++)
    {
        PORTD = TX_HIGH;
        _delay_us(SOCKET_START_HIGH * SOCKET_PULSE_US);
        PORTD = TX_LOW;
        _delay_us(SOCKET_START_LOW * SOCKET_PULSE_US);

        for (size_t j = 0; j < SOCKET_PAYLOAD_LENGTH; j++)
        {
            // Send LSB first
            if ((code & (1 << j)) != 0)
            {
                PORTD = TX_HIGH;
                _delay_us(SOCKET_ONE_HIGH * SOCKET_PULSE_US);
                PORTD = TX_LOW;
                _delay_us(SOCKET_ONE_LOW * SOCKET_PULSE_US);
            }
            else
            {
                PORTD = TX_HIGH;
                _delay_us(SOCKET_ZERO_HIGH * SOCKET_PULSE_US);
                PORTD = TX_LOW;
                _delay_us(SOCKET_ZERO_LOW * SOCKET_PULSE_US);
            }
        }
    }
}

void radio_socket_on()
{
    socket_send(on);
}

void radio_socket_off()
{
    socket_send(off);
}
