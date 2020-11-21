#define __SFR_OFFSET 0
#include <avr/io.h>

; Code taken and modified from: https://www.embeddedrelated.com/showarticle/528.php
; r18 = data byte
; r19 = 7-bit count
; r20 = 1 output
; r21 = 0 output
; r22 = SREG save
; r24:25 = 16-bit count
; r26:27 (X) = data pointer

.equ LED_PORT, PORTB
.equ LED_PIN,  2

.global output_grb
output_grb:
    movw   r26, r24           ; r26:27 = X = Pointer to color data
    movw   r24, r22           ; r24:25 = Counter of bytes to read
    in     r20, LED_PORT
    ori    r20, (1<<LED_PIN)  ; Our high output
    in     r21, LED_PORT
    andi   r21, ~(1<<LED_PIN) ; Our low output
    ldi    r19, 7             ; Bit counter initialized with 7 (8th bit is different)
    ld     r18, X+            ; Get first data byte and increment X
loop1:
    out    LED_PORT, r20 ; 1   +0  Start high pulse
    nop                  ; 1   +1
    nop                  ; 1   +2
    nop                  ; 1   +3
    nop                  ; 1   +4
    lsl    r18           ; 1   +5  Put current bit into C in SREG
    brcs   L1            ; 1/2 +6  Branch if C is set
    out    LED_PORT, r21 ; 1   +0  C was not set, end high pulse after 6 cycles and begin low pulse for 14 cycles
    nop                  ; 1   +1
    nop                  ; 1   +2
    nop                  ; 1   +3
    nop                  ; 1   +4
    nop                  ; 1   +5
    nop                  ; 1   +6
    nop                  ; 1   +7
    nop                  ; 1   +8
    nop                  ; 1   +9
    nop                  ; 1   +10
    subi   r19, 1        ; 1   +11 how many more bits for this byte?
    breq   bit8          ; 1/2 +12 Branch to last bit if zero bit was set in SREG
    rjmp   loop1         ; 2       14 total for 0 bit
L1:
    nop                  ; 1   +8
    nop                  ; 1   +9
    nop                  ; 1   +10
    nop                  ; 1   +11
    nop                  ; 1   +12
    nop                  ; 1   +13
    out    LED_PORT, r21 ; 1   +0  End high pulse after 13 cycles and begin low pulse for 7 cycles
    nop                  ; 1   +1
    nop                  ; 1   +2
    nop                  ; 1   +3
    nop                  ; 1   +4
    subi   r19, 1        ; 1   +5  Subtract from bit counter
    brne   loop1         ; 2/1 +6  Loop back if not last bit (subi did not set Z flag in SREG)
    nop                  ; 1   +7  Fall through if last bit
bit8:
    out    LED_PORT, r20 ; 1   +0  Start high pulse
    nop                  ; 1   +1
    ldi    r19, 7        ; 1   +2  Initialize bit counter for next byte
    lsl    r18           ; 1   +3  Put current bit into C in SREG
    ld     r18, X+       ; 2   +5  fetch next byte
    brcs   L2            ; 1/2 +6  Branch if C is set
    out    LED_PORT, r21 ; 1   +0  End high after 6 cycles and begin low for 14 cycles
    nop                  ; 1   +1
    nop                  ; 1   +2
    nop                  ; 1   +3
    nop                  ; 1   +4
    nop                  ; 1   +5
    nop                  ; 1   +6
    nop                  ; 1   +7
    nop                  ; 1   +8

    sbiw   r24, 1        ; 2   +12 dec byte counter
    brne   loop1         ; 2/1 +13 Loop back after 12 cycles (14 cycles total then) or end
    ret                  ; Return from procedure
L2:
    nop                  ; 1   +8
    nop                  ; 1   +9
    nop                  ; 1   +10
    nop                  ; 1   +11
    nop                  ; 1   +12
    nop                  ; 1   +13
    out    LED_PORT, r21 ; 1   +0  End high after 13 cycles and begin low for 7 cycles
    sbiw   r24, 1        ; 2   +2  dec byte counter
    brne   gotoLoop1     ; 2/1 +3  Loop back after 2 cycles (7 cycles total then) or end
    nop                  ; 1   +4
    nop                  ; 1   +5
    nop                  ; 1   +6
    ret                  ; 1
gotoLoop1:
    jmp    loop1         ; 3   +7
