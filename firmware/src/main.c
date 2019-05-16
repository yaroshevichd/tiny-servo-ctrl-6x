#include <string.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "avr_mcu_section.h"
#include "i2c.h"
#include "servo.h"
#include "twi.h"

#define MAIN_ADDRESS    (0x40)

#define NO_POSITION     (2^15 - 1)

#define MIN_FREQ_HZ     (32)        // each 256 timer ticks at 1024 div
#define MAX_FREQ_HZ     (255)       // each   1 timer tick at 1024 div

#define MIN_PULSE_10TH  (1)         // =0.001ms, or   1 timer tick  at 64 div
#define MAX_PULSE_10TH  (204)       // =2.040ms, or 256 timer ticks at 64 div


static uint8_t cur_register = 0;
static uint8_t registers[] =
{
    0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0,
};


const struct avr_mmcu_vcd_trace_t _AVR_MMCU_REGS[]  _MMCU_ = {
    { AVR_MCU_VCD_SYMBOL("SERVO0L"), .what = (void*)&registers[REG_SERVO0L], },
    { AVR_MCU_VCD_SYMBOL("SERVO0H"), .what = (void*)&registers[REG_SERVO0H], },
};

#define IS_TEST_MODE(x) (x & _BV(SFTM))
#define SET_TEST_MODE() { registers[REG_STATUS] |= _BV(SFTM); }
#define CLR_TEST_MODE() { registers[REG_STATUS] &= ~(_BV(SFTM)); }

#define IS_POS_DIRTY(x) (x & _BV(SFPD))
#define SET_DIRTY_POS() { registers[REG_STATUS] |= _BV(SFPD); }
#define CLR_DIRTY_POS() { registers[REG_STATUS] &= ~(_BV(SFPD)); }


void on_data_request()
{
    twi_drop_out_data();    // always send data from scratch
    while (cur_register < sizeof(registers))
    {
        twi_send_uint8(registers[cur_register++]);
    }
    cur_register = 0;
}

void on_data_received(uint8_t bytes)
{
    cur_register = twi_get_uint8();
    while (twi_has_in_data())
    {
        const uint8_t data = twi_get_uint8();
        if (cur_register == REG_STATUS)
        {
            registers[cur_register] = data;
            // if (IS_TEST_MODE(data))
            // {
            //     SET_TEST_MODE();
            // }
            // if (IS_POS_DIRTY(data))
            // {
            //     SET_DIRTY_POS();
            // }
        }
        else if (cur_register >= REG_SERVO0L)
        {
            registers[cur_register] = data;
        }
        cur_register++;
    }
    cur_register = 0;
}

void update_positions()
{
    static servos_pos_t copy;
    memcpy(copy, &registers[REG_SERVO0L], sizeof(servos_pos_t));
    servo_set_many(copy);
}


int main()
{
    PRR |= _BV(PRADC);
    ACSR |= _BV(ACD);

    uint8_t subaddress = 0;
    // asm(
    //     "in     %[subaddr], %[port]\n\t"
    //     "sbrc   %[subaddr], %[pin3]\n\t"
    //     "sbr    %[subaddr], %[pin2m]\n\t"
    //     "cbr    %[subaddr], %[pin3m]\n\t"
    //     : [subaddr] "=&r" (subaddress)
    //     : [port] "I" (_SFR_IO_ADDR(PINB)),
    //       [pin3] "M" (PINB3),
    //       [pin3m] "M" (_BV(PINB3)),
    //       [pin2m] "M" (_BV(PINB2))
    // );

    servo_init(50, 200);
    twi_init(MAIN_ADDRESS + subaddress, on_data_request, on_data_received);
    servo_start();

    SET_TEST_MODE();

    sei();

    while (1)
    {
        if (!IS_TEST_MODE(registers[REG_STATUS]))
        {
            if (IS_POS_DIRTY(registers[REG_STATUS]))
            {
                update_positions();
                CLR_DIRTY_POS();
            }
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();
            continue;
        }

        static uint8_t count_up = 1;
        static servo_pos_t test_pos = MIN_POS_DEG;

        servo_pos_t *ptr_reg = (servo_pos_t*)&(registers[REG_SERVO0L]);
        for (uint8_t i = 0; i < SERVOS_COUNT; ++i, ++ptr_reg)
        {
            *ptr_reg = test_pos;
        }
        update_positions();
        int16_t adj = count_up ? +5 : -5;

        if (test_pos + adj >= MAX_POS_DEG)
        {
            count_up = 0;
            test_pos = MAX_POS_DEG;
        }
        else if (test_pos + adj <= MIN_POS_DEG)
        {
            count_up = 1;
            test_pos = MIN_POS_DEG;
        }
        else
        {
            test_pos += adj;
        }

        _delay_ms(35);
    };

    return 0;
}
