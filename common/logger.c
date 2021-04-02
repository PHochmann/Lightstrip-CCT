#include <stdio.h>
#include <stdarg.h>
#include <avr/io.h>
#include "logger.h"

#define MAX_PRINTF_LENGTH 30

void logger_init()
{
    // Baud rate: 500000 bps @ 16 MHz
    UBRR0H = (uint8_t)(3 >> 8);
    UBRR0L = (uint8_t)(3 & 0xFF);
    UCSR0A = (1 << U2X0);                  // Double speed
    UCSR0B = (0 << RXEN0) | (1 << TXEN0);  // disable the receiver, enable transmitter
    UCSR0C = (1 << USBS0) | (3 << UCSZ00); // 2 stop bits, 8 bit data
}

void logger_print(const char *str)
{
    while (*str != '\0')
    {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *str;
        str++;      
    }
}

void logger_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char str[MAX_PRINTF_LENGTH];
    vsnprintf(str, MAX_PRINTF_LENGTH, fmt, args);
    logger_print(str);
}
