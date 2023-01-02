#include "i2c.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <util/twi.h>

void i2c_init(void) {
  // Set SCL frequency to ~92kHz - TWBR to 100 and prescalar to 1.
  // SCL freq = (clock speed) / (16 + 2*TWBR*prescalar)
  TWBR = 100;
  // Enable I²C peripheral
  TWCR = (1 << TWEN);
}

void i2c_start(void) {
	// Clear interrupt, generate start condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	// Wait until TWINT comes high, i.e. waiting for software
	while (!(TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != TW_START) {
		// Error condition
		// panic("TWI_START_FAILED");
	}
	// Start condition bit will be cleared by next function
}

/* Generate a stop condition on the I²C bus */
void i2c_stop(void) {
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while(TWCR & (1<<TWSTO)); // Wait until stop bit cleared by hw
}

/* Write one byte (8 bits) to the I²C bus */
void i2c_write(uint8_t data) {
  // Copy data into TWI data register
	TWDR = data;
	// Clear interrupt
	TWCR = (1 << TWINT) | (1 << TWEN);
	// Wait until done (TWINT comes high)
	while (!(TWCR & (1 << TWINT)));
}

void i2c_send_byte(uint8_t address, uint8_t data) {
	i2c_start();
	i2c_write(address);
	uint8_t status = (TWSR & 0xF8);
	if (status != TW_MT_SLA_ACK) {
		char error[21];
		snprintf(error, 21, "TWI_NO_ADDR_ACK %#x", status);
		// panic(error);
	}
	i2c_write(data);
	if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
		// panic("TWI_NO_DATA_ACK");
	}
	i2c_stop();
}

void i2c_send_bytes(uint8_t address, uint8_t *data, uint8_t length) {
	i2c_start();
	i2c_write(address);
	uint8_t status = (TWSR & 0xF8);
	if (status != TW_MT_SLA_ACK) {
		char error[21];
		snprintf(error, 21, "TWI_NO_ADDR_ACK %#x", status);
		// panic(error);
	}
	for (int i = 0; i < length; i++) {
		i2c_write(data[i]);
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
			// panic("TWI_NO_DATA_ACK");
		}
	}
	i2c_stop();
}
