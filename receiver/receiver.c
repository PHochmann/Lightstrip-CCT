#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#define RADIO_RV
#include "radio_rv.h"
#include "../common/logger.h"

// OC2A = Port B, Pin 3 PWM COLD WHITE
// OC2B = Port D, Pin 3 PWM WARM WHITE

#define MIN_BRIGHTNESS 0.1

void pwm_init()
{
    // OC2A, OC2B non-inverting fast PWM, top=0xFF
    TCCR2A = (1 << COM2A1) | (0 << COM2A0) | (1 << COM2B1) | (0 << COM2B0) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (0 << WGM22) | (0 << CS22) | (0 << CS21) | (1 << CS20);
    DDRD |= (1 << 3);
    DDRB |= (1 << 3);
    OCR2A = MIN_BRIGHTNESS * 255;
    OCR2B = MIN_BRIGHTNESS * 255;
}

int main()
{
    pwm_init();
    radio_init_rv();
    logger_init();

    logger_print("Receiver started!\n");

    while (true)
    {
        uint8_t to_receive[2] = { 0 };
        radio_receive(to_receive, 2);
        //logger_printf("rcv: %d %d\n", to_receive[0], to_receive[1]);
        
        float brightness = ((MIN_BRIGHTNESS - 1) / 255) * to_receive[0] + 1;
        uint8_t hue = to_receive[1];
        uint8_t cold = (hue <= 128) ? 255 : (255 - 2 * (hue + 127));
        uint8_t warm = (hue >= 128) ? 255 : 2 * hue;

        OCR2A = cold * brightness;
        OCR2B = warm * brightness;
    }

    return 0;
}
