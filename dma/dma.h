#ifndef DMA_H
#define DMA_H

#include<stdint.h>

int dma_init();

int mem_to_preiph_transfer(int dma, int dma_channel, uint8_t priority, uint8_t mem_bit_size, uint8_t periph_bit_size, void* source, void* dest, uint16_t address_locations);

int preiph_to_mem_transfer(int dma, int dma_channel, uint8_t priority, uint8_t mem_bit_size, uint8_t periph_bit_size, void* source, void* dest, uint16_t address_locations);

#endif