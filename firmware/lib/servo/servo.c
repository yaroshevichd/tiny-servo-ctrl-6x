#include "servo.h"
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include <tim0.h>
#include <tim1.h>

#define PWM_PORT            PORTA
#define PWM_DDR             DDRA

#define SERVO0_PIN          (PB0)
#define SERVO0_MASK         (_BV(SERVO0_PIN))

#define SERVO1_PIN          (PA1)
#define SERVO1_MASK         (_BV(SERVO1_PIN))

#define SERVO2_PIN          (PA2)
#define SERVO2_MASK         (_BV(SERVO2_PIN))

#define SERVO3_PIN          (PA3)
#define SERVO3_MASK         (_BV(SERVO3_PIN))

#define SERVO4_PIN          (PA5)
#define SERVO4_MASK         (_BV(SERVO4_PIN))

#define SERVO5_PIN          (PA7)
#define SERVO5_MASK         (_BV(SERVO5_PIN))

#define SERVO_ALL_MASK      (SERVO0_MASK | SERVO1_MASK | SERVO2_MASK | SERVO3_MASK | SERVO4_MASK | SERVO5_MASK)


#define TIM_50HZ_INT        (F_CPU / 50 / 1024)                 // 156
#define TIM_50HZ_ERROR      (F_CPU / 50 - TIM_50HZ_INT * 1024)  // 256

#define PULSE_HIGH(mask)    { PWM_PORT |= mask; }
#define PULSE_LOW(mask)     { PWM_PORT &= ~(mask); }

// specify mapping between PWM index and bit mask
const uint8_t MAP_IDX_TO_MASK[SERVOS_COUNT] = {
    SERVO0_MASK,
    SERVO1_MASK,
    SERVO2_MASK,
    SERVO3_MASK,
    SERVO4_MASK,
    SERVO5_MASK
};

// defines type to control PWM: timer counter when to fire interrupt and PWM to disable
typedef struct  {
    uint8_t timer_counts;
    uint8_t disable_mask;
} pwm_output_t;
typedef pwm_output_t pwms_output_t[SERVOS_COUNT + 1];

// servo minimum pwm pulse length in 10-nht of us (lower boundary or -90deg)
uint8_t pwm_pulse_min = 50;

// servo pwm pulse control length in 10-nth of us (excl min len) (upper boundary or +90deg)
uint8_t pwm_pulse_len = 200;

// hold actual current PWM settings
pwms_output_t pwms_output = {0};

pwms_output_t pwm_output_buffer;

// used in interrupts to iterate over PWM output data
pwm_output_t *pwm_output_cur = 0;


/// @brief Timer callback which is responsible for pulses frequency (50Hz)
void tim0_callback()
{
    asm("in r5, __SREG__"::);
    asm("mov r8, r24"::);

    tim1_reset();
    pwm_output_cur = pwms_output;
    tim1_count_to(pwm_output_cur->timer_counts);

    _delay_loop_1((TIM_50HZ_ERROR - (4 + 9 + 13)) / 3 - 1); // -5cc to ideal

    tim1_start();               // +4cc
    PULSE_HIGH(SERVO_ALL_MASK); // +7cc

    asm("mov r24, r8"::);
    asm("out __SREG__, r5"::);
}

/// @brief Timer callback which is responsible for pulse length in range [0.5-2.5] (ms)
/// @details Actual PWM output happen 30cc later than interrupt fired, in the same
/// time first interrupt (min pulse length) triggered with -32cc error, as a result
/// total error should be around 2cc or 0.25us
void tim1_callback()
{
    asm("in r5, __SREG__"::);
    asm("mov r8, r24"::);
    asm("mov r9, r25"::);
    asm("mov r10, r1"::);
    asm("eor r1, r1"::);

    asm(
        "ld     r25, Z+ \n\t"
        "ld     r25, Z+ \n\t"
        "com    r25     \n\t"
        "in     r24, %2 \n\t"
        "and    r24, r25\n\t"
        "out    %2, r24 \n\t"   // PULSE_LOW(pwm_output_cur->disable_mask);     +4+9+17cc=30cc
        "ld     r24, Z  \n\t"   // ++pwm_output_cur;
        "cpse   r24, r1 \n\t"   // if (pwm_output_cur->timer_counts == 0)
        "rjmp   1f      \n\t"
        "out    %3, r1  \n\t"   // TIM1_STOP()
    "1:""out    %4, r24 \n\t"   // TIM1_COUNT_TO(pwm_output_cur->timer_counts);
        : "=z" (pwm_output_cur)
        : "z" (pwm_output_cur),
          "I" (_SFR_IO_ADDR(PWM_PORT)),
          "I" (_SFR_IO_ADDR(TCCR1B)),
          "I" (_SFR_IO_ADDR(OCR1AL))
    );

    asm("mov r1, r10"::);
    asm("mov r25, r9"::);
    asm("mov r24, r8"::);
    asm("out __SREG__, r5"::);
}

__attribute__((noinline))
uint8_t transle_pos_to_pwm_pulse_len(const servo_pos_t servo_pos)
{
    return ((unsigned long)pwm_pulse_len * servo_pos / MAX_POS_DEG);
}

uint8_t pulse_len_to_counts(const uint8_t pulse_len)
{
    // at CLK/64 we need to multiply value to 1.25 (because of settings are expressed in us * 10)
    uint8_t ticks = pulse_len + (pulse_len >> 2);
    if (ticks > 0)
        --ticks;
    return ticks;
}

__attribute__((noinline))
void prepare_pwm_output(const servos_pos_t positions, const uint8_t *sorted_idx, pwms_output_t pwms_output)
{
    tim1_cnt_t prev_count = 0;
    pwms_output->timer_counts = pulse_len_to_counts(pwm_pulse_min);
    pwms_output->disable_mask = 0;

    for (uint8_t i = 0; i < SERVOS_COUNT; ++i, ++sorted_idx)
    {
        const servo_pos_t position = positions[*sorted_idx];
        if (position == SERVOS_NO_POS)
        {
            continue;
        }

        uint8_t pulse_len = transle_pos_to_pwm_pulse_len(position);
        tim1_cnt_t counts = pulse_len_to_counts(pulse_len);
        if (counts != prev_count)
        {
            pwms_output++;
            pwms_output->timer_counts = counts - prev_count;
            pwms_output->disable_mask = 0;
            prev_count = counts;
        }
        pwms_output->disable_mask |= MAP_IDX_TO_MASK[*sorted_idx];
    }

    pwms_output++;
    pwms_output->timer_counts = 0;
}

void servo_init(const servo_pulse_t pulse_min, const servo_pulse_t pulse_len)
{
    pwm_pulse_min = pulse_min;
    pwm_pulse_len = pulse_len;

    PWM_DDR |= SERVO_ALL_MASK;
    PULSE_LOW(SERVO_ALL_MASK);

    tim0_init(tim0_clock_div_1024);
    tim0_count_to(TIM_50HZ_INT - 1);
    tim0_set_callback(tim0_callback);

    tim1_init(tim1_clock_div_64);
    tim1_set_callback(tim1_callback);
}

void servo_start()
{
    tim0_start();
}

void servo_stop()
{
    tim0_stop();
}

void servo_set_many(const servos_pos_t positions)
{
    static uint8_t sorted_idx[SERVOS_COUNT];
    for (uint8_t i = 0; i < SERVOS_COUNT; ++i)
    {
        sorted_idx[i] = i;
    }
    for (uint8_t i = 0; i < SERVOS_COUNT; ++i)
    {
        uint8_t min = i;
        for (uint8_t j = i + 1; j < SERVOS_COUNT; ++j)
        {
            if (positions[sorted_idx[j]] < positions[sorted_idx[min]])
            {
                min = j;
            }
        }
        uint8_t tmp = sorted_idx[i];
        sorted_idx[i] = sorted_idx[min];
        sorted_idx[min] = tmp;
    }

    prepare_pwm_output(positions, sorted_idx, pwm_output_buffer);

    // wait a time windows to set new settings
    while (TCNT0 < 20 || TCNT0 >= TIM_50HZ_INT - 1);

    // it's safe to disable interrupts here, because no critical interrups will fire
    cli();
    memcpy(pwms_output, pwm_output_buffer, sizeof(pwms_output_t));
    sei();
}
