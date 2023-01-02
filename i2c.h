#include <stdint.h>

#ifndef I2C_H
#define I2C_H
extern void i2c_init(void);
extern void i2c_start(void);
extern void i2c_stop(void);
extern void i2c_write(uint8_t data);
extern void i2c_send_byte(uint8_t address, uint8_t data);
extern void i2c_send_bytes(uint8_t address, uint8_t *data, uint8_t length);
#endif
