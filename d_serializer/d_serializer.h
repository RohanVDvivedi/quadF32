#ifndef D_SERIALIZER_H
#define D_SERIALIZER_H

#include<stdint.h>

char hex_to_char(uint8_t n);

uint8_t char_to_hex(char c);

void stringify_32(char* num, uint32_t n);

uint32_t numify_32(char* num);

void stringify_8(char* num, uint8_t n);

uint32_t numify_8(char* num);

#endif