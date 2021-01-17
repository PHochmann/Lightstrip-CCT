#define F_CPU 16000000L
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// OC2A = Port B, Pin 3 PWM COLD WHITE
// OC2B = Port D, Pin 3 PWM WARM WHITE
// ADC0 = Port C, Pin 0 ANALOG
// ADC0 = Port C, Pin 1 ANALOG

void init()
{
    // OC2A, OC2B non-inverting fast PWM, top=0xFF
    TCCR2A = (1 << COM2A1) | (0 << COM2A0) | (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (0 << WGM22) | (0 << CS22) | (0 << CS21) | (1 << CS20);
    DDRD |= (1 << 3);
    DDRB |= (1 << 3);
    OCR2A = 10;
    OCR2B = 0;

    // Prescaler 128
    ADCSRA = (1 << ADEN) | (0 << ADATE) | (0 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (0 << REFS1) | (1 << REFS0) | (1 << ADLAR);
    DDRC &= (0 << 0) | (0 << 1);
    PORTC |= (1 << 0) | (1 << 1);
}

uint8_t get_adc_value()
{
    //ADMUX &= 0b11110000;
    //ADMUX |= (mux & 0b00001111);
    ADCSRA |= (1 << ADSC); // Start
    while ((ADCSRA & (1 << ADIF)) == 0); // Wait
    ADCSRA &= ~(1 << ADIF); // Clear flag
    return ADCH;
}

int main()
{
    init();
    while (true)
    {
        OCR2A = get_adc_value();
        OCR2B = get_adc_value();
    }
    return 0;
}
