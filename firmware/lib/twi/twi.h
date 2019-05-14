#ifndef __TWI_H__
#define __TWI_H__

#include <stdint.h>

void twi_init(const uint8_t address, void (*on_request_ptr)(void), void (*on_receive_ptr)(uint8_t));
void twi_stop();
void twi_send_uint8(const uint8_t value);
uint8_t twi_get_uint8(void);
uint8_t twi_has_in_data(void);
uint8_t twi_has_out_data(void);

#endif // __TWI_H__
