#ifndef UART_H
#define UART_H

#include<regs/gpio.h>
#include<regs/usart.h>

int uart_init();

void uart_write_byte(char byte);

char uart_read_byte();

int uart_write(char* str, unsigned int bytes_to_write);

int uart_read(char* str, unsigned int bytes_to_read);

#endif