#include "tim0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t tim0_map_clock_div[] =
{
    0b001,  // 1
    0b010,  // 8
    0b011,  // 64
    0b100,  // 256
    0b101,  // 1024
};

static tim0_callback_t pfn_callback = 0;
static uint8_t clock_mask = 0;

ISR(TIM0_COMPA_vect, ISR_NAKED)
{
    asm("mov r6, r30"::);
    asm("mov r7, r31"::);

    pfn_callback();

    asm("mov r31, r7"::);
    asm("mov r30, r6"::);

    reti();
}

void tim0_init(const tim0_clock_div_t prescale)
{
    clock_mask = tim0_map_clock_div[prescale];
}

void tim0_start()
{
    TCCR0A = _BV(WGM01);
    TCCR0B = clock_mask;
}

void tim0_stop()
{
    TCCR0B = 0;
}

void tim0_reset()
{
    TCNT0 = 0;
}

void tim0_count_to(const tim0_cnt_t value)
{
    OCR0A = value;
}

void tim0_set_callback(tim0_callback_t callback)
{
    pfn_callback = callback;
    TIMSK0 = (callback != 0) ? _BV(OCIE0A) : 0;
}
