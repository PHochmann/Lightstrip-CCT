#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "radio_rv.h"
#include "../common/protocol.h"
//#include "../common/logger.h"

/*
Timer/Counter 1 is used to capture input at Pin B0
*/

#define F_CPU 16000000
#define PRESCALER 1024
#define TICKS_PER_PULSE ((F_CPU / PRESCALER) / (1000000 / PULSE_US)) // 6
#define BUFFER_SIZE 100
#define ICP1 0

volatile size_t head;
volatile size_t tail;
volatile uint8_t buffer[BUFFER_SIZE];

void radio_init_rv()
{
    // Noise canceller, rising edge detection, 1024 prescaler
    DDRB &= ~(1 << 0);
    PORTB &= ~(1 << 0);
    TCCR1A = 0;
    TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12) | (0 << CS11) | (1 << CS10);
    TIMSK1 = (1 << ICIE1) | (0 << TOIE1);
    sei();
}

ISR (TIMER1_CAPT_vect)
{
    static bool is_listening = false;
    static uint8_t high_pulses = 0;
    static uint8_t low_pulses = 0;
    static uint8_t curr_byte = 0;
    static uint8_t curr_bit = 0;
    static uint8_t tentative_head = 0;
    static uint8_t checksum = 0;

    uint16_t pulses = ((double)ICR1 / TICKS_PER_PULSE) + 0.5;
    TCNT1 = 0; // reset timer 1 counter
    TCCR1B ^= (1 << ICES1); // Toggle edge detection

    if (((PINB >> ICP1) & 1) == 0)
    {
        high_pulses = pulses;
    }
    else
    {
        low_pulses = pulses;
        //logger_printf("%d %d\n", high_pulses, low_pulses);

        // We always end on a low pulse, so process high-low-pair
        if (is_listening)
        {
            // Check for end of transmission
            if (high_pulses == END_HIGH && low_pulses == END_LOW)
            {
                is_listening = false;

                // Bitwise even parity
                // XOR over all bytes must be 0
                if (checksum == 0)
                {
                    // -1 % 100 = -1 and not 99 :(
                    if (tentative_head == 0)
                    {
                        head = BUFFER_SIZE - 1;
                    }
                    else
                    {
                        head = (tentative_head - 1) % BUFFER_SIZE;
                    }
                }
                return;
            }
            else
            {
                // Extract data bit
                if (high_pulses == ONE_HIGH && low_pulses == ONE_LOW)
                {
                    curr_byte |= (1 << curr_bit);
                }
                else
                {
                    if (high_pulses == ZERO_HIGH && low_pulses == ZERO_LOW)
                    {
                        curr_byte &= ~(1 << curr_bit);
                    }
                    else
                    {
                        // High-low-pair not corresponding to anything
                        return;
                    }
                }

                curr_bit++;
                if (curr_bit == 8)
                {
                    // Don't overrun the reading buffer
                    // Don't receive any more data, stop listening
                    if (head != tail && tentative_head == tail)
                    {
                        //logger_print("stopping...\n");
                        is_listening = false;
                        return;
                    }
                    buffer[tentative_head] = curr_byte;
                    checksum ^= curr_byte;
                    //logger_printf("byte rcvd: %d\n", curr_byte);
                    curr_bit = 0;
                    tentative_head = (tentative_head + 1) % BUFFER_SIZE;
                }
            }
        }
        
        // Listen for start of transmission
        if (high_pulses == START_HIGH && low_pulses == START_LOW)
        {
            tentative_head = head;
            is_listening = true;
            curr_bit = 0;
            curr_byte = 0;
            checksum = 0;
        }
    }
}

void radio_receive(uint8_t *buff, size_t bytes)
{
    for (size_t i = 0; i < bytes; i++)
    {
        while (head == tail);
        buff[i] = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
    }
}
