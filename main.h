#ifndef MAIN_H
#define MAIN_H

void panic(char* errorCode);

void uart_init(void);
void uart_transmit_char(char data);
void uart_transmit_string(char* string);

#endif
