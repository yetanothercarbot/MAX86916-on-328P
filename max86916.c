#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/twi.h>
#include <util/delay.h>

#include "i2c.h"

#define MAX_WRITE_ADDR 0xAE
#define MAX_READ_ADDR 0xAF

/* Initialise MAX86916 */
void MAX86916_init(void) {
  // Configure AVR interrupts
  DDRD &= ~(1 << 2); // Set as input
  PORTD = (1 << 2); // Pull up
  EICRA = (1 << ISC01); // Falling edge of INT0
  EIMSK = (1);

  _delay_ms(2);
  uint8_t max86916_reset[] = {0x09, (1 << 6)};
  uint8_t max86916_config1[] = {0x08, (0b111 << 5) | (0b1000), 0b111, (5 << 2) | (0b11 << 0)};
  uint8_t max86916_config2[] = {0x0C, 0b00110010, 0b00110010, 0b00011010, 0b00011010};
  uint8_t max86916_config3[] = {0x13, (0b0001 << 0) | (0b0010 << 4), (0b0011 << 0) | (0b0100 << 4)};
  // Perform reset of chip
  i2c_send_bytes(MAX_WRITE_ADDR, max86916_reset, 2);
  _delay_ms(2); // Give the MAX86916 time to reset
  // Write to 0x08 (FIFO Config): Average 32 samples per LED, trigger interrupt on 24 samples in FIFO
  // Write to 0x09 (Mode config 1): Flex LED mode
  // Write to 0x0A (Mode config 2): Set to 1000 samples/sec and 420µs LED pulse width
  i2c_send_bytes(MAX_WRITE_ADDR, max86916_config1, 4);
  _delay_ms(2);
  // Skip 0x0B (reserved register)
  // Write to 0x0C (LED1_PA): Set LED1 to ~5mA (as RGE is set to 0x0)
  // Write to 0x0D (LED2_PA): Set LED2 to ~5mA (as RGE is set to 0x0)
  // Write to 0x0E (LED3_PA): Set LED3 to ~5mA (as RGE is set to 0x0)
  // Write to 0x0F (LED4_PA): Set LED4 to ~5mA (as RGE is set to 0x0)
  i2c_send_bytes(MAX_WRITE_ADDR, max86916_config2, 5);
  _delay_ms(2);
  // Write to 0x13 (LED Sequence 1). LED 1 and 2 in position 1 and 2 respectively
  // Write to 0x14 (LED Sequence 2). LED 3 and 4 in pos 3 and 4 respectively.
  i2c_send_bytes(MAX_WRITE_ADDR, max86916_config3, 3);
  _delay_ms(2);

  // Point back at FIFO output buffer
  i2c_send_byte(MAX_WRITE_ADDR, 0x07);
}
