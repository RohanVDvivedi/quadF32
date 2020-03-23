#ifndef UART_H
#define UART_H

#include<regs/rcc.h>
#include<regs/gpio.h>
#include<regs/usart.h>
#include<regs/dma.h>

int uart_init();

void uart_write_byte(char byte);

char uart_read_byte();

void uart_write_blocking(char* str, unsigned int bytes_to_write);

void uart_write_through_dma(char* str, unsigned int bytes_to_write);

int uart_read(char* str, unsigned int bytes_to_read);

#endif