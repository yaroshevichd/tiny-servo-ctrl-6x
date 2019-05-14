#ifndef __TIM0_H__
#define __TIM0_H__

#include <stdint.h>

typedef uint8_t tim0_cnt_t;

typedef enum
{
    tim0_clock_div_1,
    tim0_clock_div_8,
    tim0_clock_div_64,
    tim0_clock_div_256,
    tim0_clock_div_1024,
} tim0_clock_div_t;

typedef void(*tim0_callback_t)();

void tim0_init(const tim0_clock_div_t prescale);
void tim0_start();
void tim0_stop();
void tim0_reset();
void tim0_count_to(const tim0_cnt_t value);
void tim0_set_callback(const tim0_callback_t callback);

#endif // __TIM0_H__
