#include "tim1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

const uint8_t tim1_map_clock_div[] =
{
    0b001,  // 1
    0b010,  // 8
    0b011,  // 64
    0b100,  // 256
    0b101,  // 1024
};

static tim1_callback_t pfn_callback = 0;
static uint8_t clock_mask = 0;

ISR(TIM1_COMPA_vect, ISR_NAKED)
{
    asm("mov r6, r30"::);
    asm("mov r7, r31"::);

    pfn_callback();

    asm("mov r31, r7"::);
    asm("mov r30, r6"::);

    reti();
}

void tim1_init(const tim1_clock_div_t prescale)
{
    clock_mask = tim1_map_clock_div[prescale];
}

void tim1_start()
{
    TCCR1B = _BV(WGM12) | clock_mask;
}

void tim1_stop()
{
    TCCR1B = 0;
}

void tim1_reset()
{
    TCNT1 = 0;
}

void tim1_count_to(const tim1_cnt_t value)
{
    OCR1AL = value;
}

void tim1_set_callback(tim1_callback_t callback)
{
    pfn_callback = callback;
    TIMSK1 = (callback != 0) ? _BV(OCIE1A) : 0;
}
