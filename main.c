#define F_CPU 20000000    // AVR clock frequency in Hz, used by util/delay.h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/twi.h>
#include <util/delay.h>

#include "main.h"
#include "i2c.h"
#include "display.h"
#include "max86916.h"

volatile uint8_t samples_updated = 0;
volatile uint8_t samples[16];

int main(void) {
  uart_init();
  uart_transmit_string("a\n");
  i2c_init();
  display_init();
  MAX86916_init();
  sei();
  while (1) {
    if (samples_updated) {
      for (int i = 0; i < 4; i++) {
        char uartString[20] = ""; // 255, 255, 255, 255
        snprintf(uartString, 20, "%u, %u, %u, %u\n", samples[4*i], samples[4*i+1], samples[4*i+2], samples[4*i+3]);
        uart_transmit_string(uartString);
      }
    }
  }
}

ISR(INT0_vect) {
  // Read 4 samples x 4 channels of data and write them to the buffer
  i2c_start();
  i2c_write(0xAF);
  // Read from register 0xA7
  for (int i = 0; i < 16; i++) {
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    samples[i] = TWDR;
  }
  i2c_stop();
  samples_updated = 1;
}

void panic(char* errorCode) {
	uart_transmit_string(errorCode);

	cli(); // Turn off global interrupts
	// Stall execution
	while (1) {
		// Do nothing
	}
}

void uart_init(void) {
	// Set up UART
	// 9600 baud (F_CPU/(16*9600) - 1)
	UBRR0 = 127;
	// Enable interrupt, receiver and transmitter
	UCSR0B = (1<< RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	//  8 bits, no parity, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit_char(char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0))) {
		// Still not free, wait...
	}

	UDR0 = data;
}

void uart_transmit_string(char* string) {
	for (int i = 0; i < strlen(string); i++) {
		uart_transmit_char(string[i]);
	}
}
