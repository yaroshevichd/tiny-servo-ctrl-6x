#ifndef __TIM1_H__
#define __TIM1_H__

#include <stdint.h>

typedef uint8_t tim1_cnt_t;

typedef enum
{
    tim1_clock_div_1,
    tim1_clock_div_8,
    tim1_clock_div_64,
    tim1_clock_div_256,
    tim1_clock_div_1024,
} tim1_clock_div_t;

typedef void(*tim1_callback_t)();

void tim1_init(const tim1_clock_div_t prescale);
void tim1_start();
void tim1_stop();
void tim1_reset();
void tim1_count_to(const tim1_cnt_t value);
void tim1_set_callback(const tim1_callback_t callback);

#endif // __TIM1_H__
