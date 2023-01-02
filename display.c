#define F_CPU 20000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/twi.h>
#include <util/delay.h>

#include "i2c.h"

#define DISPLAY_READ_ADDR 0xE1
#define DISPLAY_WRITE_ADDR 0xE0

const uint16_t FourteenSegmentASCII[36] = {
	0b000110000111111, /* 0 */
	0b000010000000110, /* 1 */
	0b000000011011011, /* 2 */
	0b000000010001111, /* 3 */
	0b000000011100110, /* 4 */
	0b010000001101001, /* 5 */
	0b000000011111101, /* 6 */
	0b000000000000111, /* 7 */
	0b000000011111111, /* 8 */
	0b000000011101111, /* 9 */
	0b000000011110111, /* A */
	0b001001010001111, /* B */
	0b000000000111001, /* C */
	0b001001000001111, /* D */
	0b000000001111001, /* E */
	0b000000001110001, /* F */
	0b000000010111101, /* G */
	0b000000011110110, /* H */
	0b001001000001001, /* I */
	0b000000000011110, /* J */
	0b010010001110000, /* K */
	0b000000000111000, /* L */
	0b000010100110110, /* M */
	0b010000100110110, /* N */
	0b000000000111111, /* O */
	0b000000011110011, /* P */
	0b010000000111111, /* Q */
	0b010000011110011, /* R */
	0b000000011101101, /* S */
	0b001001000000001, /* T */
	0b000000000111110, /* U */
	0b000110000110000, /* V */
	0b010100000110110, /* W */
	0b010110100000000, /* X */
	0b000000011101110, /* Y */
	0b000110000001001, /* Z */
};

const uint16_t hi[4] = {0b0, 0b000000011110110, 0b101001000001001, 0b0};

void display_init(void) {
  // Start by enabling display oscillator
  i2c_send_byte(DISPLAY_WRITE_ADDR, 0b00100001);

  // Set output mode to ROW
  i2c_send_byte(DISPLAY_WRITE_ADDR, 0b10100000);

  // Set brightness to half
  i2c_send_byte(DISPLAY_WRITE_ADDR, 0b11100111);

  // Turn on display, disable blinking
  i2c_send_byte(DISPLAY_WRITE_ADDR, 0b10000001);

  // Print demo data
  i2c_start();
  i2c_write(DISPLAY_WRITE_ADDR);
  i2c_write(0x00);
  for (int j = 0; j < 4; j++) {
    i2c_write(hi[j] & 0xFF);
    i2c_write((hi[j] & 0xFF00) >> 8);
  }
  i2c_stop();
}
