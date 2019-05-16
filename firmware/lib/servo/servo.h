#ifndef __SERVO_H__
#define __SERVO_H__

#include <avr/io.h>
#include <stdint.h>

#define SERVOS_COUNT    (6)

#define SERVOS_NO_POS   (-1)
#define MIN_POS_DEG     (0U)
#define MAX_POS_DEG     (1800U)

typedef uint8_t servo_pulse_t;
typedef uint16_t servo_pos_t;
typedef servo_pos_t servos_pos_t[SERVOS_COUNT];

void servo_init(const servo_pulse_t pulse_min, const servo_pulse_t pulse_len);
void servo_set_many(const servos_pos_t positions);
void servo_start();
void servo_stop();

#endif // __SERVO_H__
