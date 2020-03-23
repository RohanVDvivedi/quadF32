#ifndef DMA_REGS_H
#define DMA_REGS_H

typedef struct DMA_c DMA_c;
struct DMA_c
{
	uint32_t DMA_CCR;
	uint32_t DMA_CNDTR;
	uint32_t DMA_CPAR;
	uint32_t DMA_CMAR;
	uint32_t RESERVED;
};

typedef struct DMA1_t DMA1_t;
struct DMA1_t
{
	uint32_t DMA_ISR;
	uint32_t DMA_IFCR;
	DMA_c channel[7];
};

typedef struct DMA2_t DMA2_t;
struct DMA2_t
{
	uint32_t DMA_ISR;
	uint32_t DMA_IFCR;
	DMA_c channel[5];
};

#define DMA1 ((volatile DMA1_t*)(0x40020000))
#define DMA2 ((volatile DMA2_t*)(0x40020400))

#define DMA1_CHANNEL(ch) DMA1->channel[ch-1]
#define DMA2_CHANNEL(ch) DMA2->channel[ch-1]

#endif