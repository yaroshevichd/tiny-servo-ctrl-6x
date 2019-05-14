#ifndef __I2C_H__
#define __I2C_H__

#define REG_STATUS      (0x00)
#define REG_SERVO0L     (0x01)
#define REG_SERVO0H     (0x02)
#define REG_SERVO1L     (0x03)
#define REG_SERVO1H     (0x04)
#define REG_SERVO2L     (0x05)
#define REG_SERVO2H     (0x06)
#define REG_SERVO3L     (0x07)
#define REG_SERVO3H     (0x08)
#define REG_SERVO4L     (0x09)
#define REG_SERVO4H     (0x0A)
#define REG_SERVO5L     (0x0B)
#define REG_SERVO5H     (0x0C)

/// servo status flag position dirty
#define SFPD            (0)
/// status flag test mode
#define SFTM            (1)

#endif // __I2C_H__
