#include<dma/dma.h>

#define DMA1			(volatile uint32_t*) 0x40020000
#define DMA2			(volatile uint32_t*) 0x40020400

#define DMA_ISR 		0x00
#define DMA_IFCR 		0x04
#define DMA_CCR 	0x08 + 0x14 * (dma_channel - 1)
#define DMA_CNDTR 	0x0c + 0x14 * (dma_channel - 1)
#define DMA_CPAR 	0x10 + 0x14 * (dma_channel - 1)
#define DMA_CMAR 	0x14 + 0x14 * (dma_channel - 1)

int dma_init()
{

}

int mem_to_preiph_transfer(int dma, int dma_channel, uint8_t priority, uint8_t mem_bit_size, uint8_t periph_bit_size, void* source, void* dest, uint16_t address_locations)
{
	volatile uint32_t* DMA = ((dma & 1) ? DMA1 : DMA2);
	volatile uint32_t* DMA_CCRx = DMA + DMA_CCR;
	volatile uint32_t* DMA_CNDTRx = DMA + DMA_CNDTR;
	volatile uint32_t* DMA_CPARx = DMA + DMA_CPAR;
	volatile uint32_t* DMA_CMARx = DMA + DMA_CMAR;

	*DMA_CPARx = (uint32_t)dest;
	*DMA_CMARx = (uint32_t)source;
	*DMA_CNDTRx = (uint32_t)address_locations;
	*DMA_CCRx = (priority<<12) | (((mem_bit_size/8)-1)<<10) | (((periph_bit_size/8)-1)<<8) | 0x10;
	*DMA_CCRx &= 0x1;
}

int preiph_to_mem_transfer(int dma, int dma_channel, uint8_t priority, uint8_t mem_bit_size, uint8_t periph_bit_size, void* source, void* dest, uint16_t address_locations)
{
	volatile uint32_t* DMA = ((dma & 1) ? DMA1 : DMA2);
	volatile uint32_t* DMA_CCRx = DMA + DMA_CCR;
	volatile uint32_t* DMA_CNDTRx = DMA + DMA_CNDTR;
	volatile uint32_t* DMA_CPARx = DMA + DMA_CPAR;
	volatile uint32_t* DMA_CMARx = DMA + DMA_CMAR;

	*DMA_CPARx = (uint32_t)source;
	*DMA_CMARx = (uint32_t)dest;
	*DMA_CNDTRx = (uint32_t)address_locations;
	*DMA_CCRx = (priority<<12) | (((mem_bit_size/8)-1)<<10) | (((periph_bit_size/8)-1)<<8) | 0x00;
	*DMA_CCRx &= 0x1;
}