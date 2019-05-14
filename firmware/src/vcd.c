#include <avr/io.h>
#include "avr_mcu_section.h"

AVR_MCU(F_CPU, "attiny44");
AVR_MCU_VCD_FILE("tiny_servo_6x.vcd", 1);

const struct avr_mmcu_vcd_trace_t _AVR_MMCU_SERVO[]  _MMCU_ = {
    { AVR_MCU_VCD_SYMBOL("SERVO-0"), .mask = _BV(PA0), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("SERVO-1"), .mask = _BV(PA1), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("SERVO-2"), .mask = _BV(PA2), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("SERVO-3"), .mask = _BV(PA3), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("SERVO-4"), .mask = _BV(PA5), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("SERVO-5"), .mask = _BV(PA7), .what = (void*)&PORTA, },
    { AVR_MCU_VCD_SYMBOL("OCR1A"), .what = (void*)&OCR1A, },
    { AVR_MCU_VCD_SYMBOL("TCCR0B"), .what = (void*)&TCCR0B, },
    { AVR_MCU_VCD_SYMBOL("TCNT1L"), .what = (void*)&TCNT1L, },
    { AVR_MCU_VCD_SYMBOL("TCCR1B"), .what = (void*)&TCCR1B, },
};

AVR_MCU_VCD_IRQ_TRACE(6, 1, "TIM1_COMPA");
AVR_MCU_VCD_IRQ_TRACE(9, 1, "TIM0_COMPA");
