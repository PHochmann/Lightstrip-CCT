#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "radio.h"
#include "logger.h"

#define RADIO_DDR  DDRD
#define RADIO_PORT PORTD
#define RADIO_PIN  PIND

#define RADIO_RV_PIN 2
#define RADIO_TX_PIN 2

#define RADIO_DELAY_US 2000

// Preamble is only ones
#define PREAMBLE_SIZE 64

uint8_t codes[16] = { 0b11110, 0b01001, 0b10100, 0b10101, 0b01010, 0b01011, 0b01110, 0b01111, 0b10010, 0b10011, 0b10110, 0b10111, 0b11010, 0b11011, 0b11100, 0b11101 };
uint8_t decodes[32] = {
    [0b11110] = 0,
    [0b01001] = 1,
    [0b10100] = 2,
    [0b10101] = 3,
    [0b01010] = 4,
    [0b01011] = 5,
    [0b01110] = 6,
    [0b01111] = 7,
    [0b10010] = 8,
    [0b10011] = 9,
    [0b10110] = 10,
    [0b10111] = 11,
    [0b11010] = 12,
    [0b11011] = 13,
    [0b11100] = 14,
    [0b11101] = 15
};

uint8_t start[2] = { 0b11000, 0b10001 };

void radio_init_tx()
{
    RADIO_DDR |= (1 << RADIO_TX_PIN);
    RADIO_PORT &= ~(1 << RADIO_TX_PIN);
}

void radio_init_rv()
{
    RADIO_DDR &= ~(1 << RADIO_RV_PIN);
}

void send_byte(uint8_t byte)
{
    // Send low nibble...
    for (uint8_t j = 0; j < 5; j++)
    {
        RADIO_PORT = (RADIO_PORT & ~(1 << RADIO_TX_PIN)) | (((codes[byte & 0x0F] >> j) & 1) << RADIO_TX_PIN);
        _delay_us(RADIO_DELAY_US);
    }
    // Send high nibble...
    for (uint8_t j = 0; j < 5; j++)
    {
        RADIO_PORT = (RADIO_PORT & ~(1 << RADIO_TX_PIN)) | (((codes[byte >> 4] >> j) & 1) << RADIO_TX_PIN);
        _delay_us(RADIO_DELAY_US);
    }
}

uint8_t receive_byte()
{
    // Receive low nibble...
    uint16_t code = 0;
    for (uint8_t j = 0; j < 10; j++)
    {
        code |= ((RADIO_PIN >> RADIO_RV_PIN) & 1) << j;
        _delay_us(RADIO_DELAY_US);
    }

    return (decodes[code >> 5] << 4) | decodes[code & 0b11111];
}

void radio_send(uint8_t *buffer, size_t bytes)
{
    send_byte(start[0]);
    send_byte(start[1]);

    uint8_t checksum = 0xFF;

    // Send payload
    for (size_t i = 0; i < bytes; i++)
    {
        checksum ^= buffer[i];
        send_byte(buffer[i]);
    }

    send_byte(checksum);
    logger_printf("%d\n", checksum);
}

void radio_receive(uint8_t *buffer, size_t bytes)
{
    start:
    // Wait for preamble, that is 32 ones
    if (receive_byte() != start[0]) goto start;
    if (receive_byte() != start[1]) goto start;

    uint8_t checksum = 0xFF;

    // Decode payload and put it into buffer
    for (size_t i = 0; i < bytes; i++)
    {
        buffer[i] = receive_byte();
        checksum ^= buffer[i];
    }

    uint8_t c = receive_byte();

    // Detect stop sequence
    // If stop bits not detected start again
    if (c != checksum)
    {
        logger_printf("! %d %d\n", c, checksum);
        goto start;
    }
}