#include "twi.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PORTA6
#define PORT_USI_SCL        PORTA4
#define PIN_USI_SDA         PINA6
#define PIN_USI_SCL         PINA4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#define TWI_RX_BUFFER_SIZE (32)
#define TWI_RX_BUFFER_MASK (TWI_RX_BUFFER_SIZE - 1)

#define TWI_TX_BUFFER_SIZE (32)
#define TWI_TX_BUFFER_MASK (TWI_TX_BUFFER_SIZE - 1)

#define SET_USI_TO_SEND_ACK() {                                 \
  USIDR    =  0;                                                \
  DDR_USI |= (1 << PORT_USI_SDA);                               \
  USISR    = (0 << USI_START_COND_INT)  | (1 << USIOIF) |       \
             (1 << USIPF) | (1 << USIDC)| (0x0E << USICNT0);    \
}

#define SET_USI_TO_READ_ACK() {                                     \
    DDR_USI &= ~(1 << PORT_USI_SDA);                                \
    USIDR    =   0;                                                 \
    USISR    =  (0 << USI_START_COND_INT)   | (1 << USIOIF) |       \
                (1 << USIPF) | (1 << USIDC) | (0x0E << USICNT0);    \
}

#define SET_USI_TO_TWI_START_CONDITION_MODE() {                                   \
    USICR = (1 << USISIE) | (0 << USIOIE) | (1 << USIWM1) | (0 << USIWM0) |       \
            (1 << USICS1) | (0 << USICS0) | (0 << USICLK) | (0 << USITC);         \
    USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) |                           \
            (1 << USIPF)  | (1 << USIDC)  | (0x0 << USICNT0);                     \
}

#define SET_USI_TO_SEND_DATA() {                                              \
    DDR_USI |= (1 << PORT_USI_SDA);                                           \
    USISR    = (0 << USI_START_COND_INT)    | (1 << USIOIF) |                 \
               (1 << USIPF) | ( 1 << USIDC) | ( 0x0 << USICNT0 );             \
}

#define SET_USI_TO_READ_DATA() { \
    DDR_USI &= ~(1 << PORT_USI_SDA);                                          \
    USISR    =  (0 << USI_START_COND_INT)   | (1 << USIOIF) |                 \
                (1 << USIPF) | (1 << USIDC) | (0x0 << USICNT0);               \
}

#define USI_RECEIVE_CALLBACK() {                                              \
    if (usi_onReceiverPtr) {                                                  \
        if (twi_has_in_data()) {                                              \
            usi_onReceiverPtr(twi_has_in_data());                             \
        }                                                                     \
    }                                                                         \
}

#define ONSTOP_USI_RECEIVE_CALLBACK() {                                       \
    if (USISR & (1 << USIPF)) {                                               \
        USI_RECEIVE_CALLBACK();                                               \
    }                                                                         \
}

#define USI_REQUEST_CALLBACK() {                                              \
    USI_RECEIVE_CALLBACK();                                                   \
    if(usi_onRequestPtr) usi_onRequestPtr();                                  \
}

typedef enum {
    USI_SLAVE_CHECK_ADDRESS                = 0x00,
    USI_SLAVE_SEND_DATA                    = 0x01,
    USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA = 0x02,
    USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA   = 0x03,
    USI_SLAVE_REQUEST_DATA                 = 0x04,
    USI_SLAVE_GET_DATA_AND_SEND_ACK        = 0x05
} overflowState_t;

static uint8_t slaveAddress;
static volatile overflowState_t overflowState;

static uint8_t rxBuf[ TWI_RX_BUFFER_SIZE ];
static volatile uint8_t rxHead;
static volatile uint8_t rxTail;
static volatile uint8_t rxCount;

static uint8_t txBuf[ TWI_TX_BUFFER_SIZE ];
static volatile uint8_t txHead;
static volatile uint8_t txTail;
static volatile uint8_t txCount;

void (*usi_onRequestPtr)(void);
void (*usi_onReceiverPtr)(uint8_t);

static void flushTwiBuffers(void)
{
    rxTail  = 0;
    rxHead  = 0;
    rxCount = 0;
    txTail  = 0;
    txHead  = 0;
    txCount = 0;
}

void twi_init(const uint8_t address, void (*on_request_ptr)(void), void (*on_receive_ptr)(uint8_t))
{
    usi_onRequestPtr = on_request_ptr;
    usi_onReceiverPtr = on_receive_ptr;
    
    flushTwiBuffers();
    slaveAddress = address;
    DDR_USI  |=  (1 << PORT_USI_SCL) | (1 << PORT_USI_SDA);
    PORT_USI |=  (1 << PORT_USI_SCL);
    PORT_USI |=  (1 << PORT_USI_SDA);
    DDR_USI  &= ~(1 << PORT_USI_SDA);
    USICR     =  (1 << USISIE) | (0 << USIOIE) | (1 << USIWM1) | (0 << USIWM0) |
                 (1 << USICS1) | (0 << USICS0) | (0 << USICLK) | (0 << USITC);
    USISR     =  (1 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

void twi_stop()
{
    USICR = 0;
}

void twi_send_uint8(const uint8_t data)
{
    while (txCount == TWI_TX_BUFFER_SIZE);
    txBuf[txHead] = data;
    txHead = (txHead + 1) & TWI_TX_BUFFER_MASK;
    txCount++;
}

uint8_t twi_get_uint8()
{
    uint8_t rtn_byte;
    while (!rxCount);
    rtn_byte = rxBuf [rxTail];
    rxTail = (rxTail + 1) & TWI_RX_BUFFER_MASK;
    rxCount--;
    return rtn_byte;
}

void twi_drop_out_data(void)
{
    txCount = 0;
}

uint8_t twi_has_in_data(void)
{
    return rxCount ? 1 : 0;
}

uint8_t twi_has_out_data(void)
{
    return txCount;
}

ISR(USI_START_VECTOR)
{
    overflowState = USI_SLAVE_CHECK_ADDRESS;
    DDR_USI &= ~(1 << PORT_USI_SDA);
    while ((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA))));
    if (!(PIN_USI & (1 << PIN_USI_SDA)))
    {
        USICR = (1 << USISIE) | (1 << USIOIE) | (1 << USIWM1) | (1 << USIWM0) |
                (1 << USICS1) | (0 << USICS0) | (0 << USICLK) | (0 << USITC);
    }
    else
    {
        USICR = (1 << USISIE) | (0 << USIOIE) | (1 << USIWM1 ) | (0 << USIWM0) |
                (1 << USICS1) | (0 << USICS0) | (0 << USICLK ) | (0 << USITC);
    }
    USISR = (1 << USI_START_COND_INT)   | (1 << USIOIF) |
            (1 << USIPF) | (1 << USIDC) | (0x0 << USICNT0);
}

ISR(USI_OVERFLOW_VECTOR)
{
    switch (overflowState)
    {
        case (USI_SLAVE_CHECK_ADDRESS):
            if ((USIDR == 0) || ((USIDR >> 1) == slaveAddress))
            {
                if (USIDR & 0x01)
                {
                    USI_REQUEST_CALLBACK();
                    overflowState = USI_SLAVE_SEND_DATA;
                }
                else
                {
                    overflowState = USI_SLAVE_REQUEST_DATA;
                }
                SET_USI_TO_SEND_ACK();
            }
            else
            {
                SET_USI_TO_TWI_START_CONDITION_MODE();
            }
            break;
        case (USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA):
            if (USIDR)
            {
                SET_USI_TO_TWI_START_CONDITION_MODE();
                return;
            }
        case (USI_SLAVE_SEND_DATA):
            if (txCount)
            {
                USIDR = txBuf[txTail];
                txTail = (txTail + 1) & TWI_TX_BUFFER_MASK;
                txCount--;
            }
            else
            {
                SET_USI_TO_READ_ACK();
                SET_USI_TO_TWI_START_CONDITION_MODE();
                return;
            }
            overflowState = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
            SET_USI_TO_SEND_DATA();
            break;
        case (USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA):
            overflowState = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
            SET_USI_TO_READ_ACK();
            break;
        case (USI_SLAVE_REQUEST_DATA):
            overflowState = USI_SLAVE_GET_DATA_AND_SEND_ACK;
            SET_USI_TO_READ_DATA();
            break;
        case (USI_SLAVE_GET_DATA_AND_SEND_ACK):
            if (rxCount < TWI_RX_BUFFER_SIZE)
            {
                rxBuf[rxHead] = USIDR;
                rxHead = (rxHead + 1) & TWI_RX_BUFFER_MASK;
                rxCount++;
            }
            overflowState = USI_SLAVE_REQUEST_DATA;
            SET_USI_TO_SEND_ACK();
            break;
    }

}
