#ifndef _HEAD_DMA_H
#define _HEAD_DMA_H

#define BUFFER_SIZE   10//(5*1024)
extern uint8_t spiBuff0[BUFFER_SIZE];
extern uint8_t spiBuff1[BUFFER_SIZE];
extern spi_dma_handle_t master0Handle;
extern spi_dma_handle_t master1Handle;
extern dma_handle_t master0TxHandle;
extern dma_handle_t master0RxHandle;
extern dma_handle_t master1TxHandle;
extern dma_handle_t master1RxHandle;
extern volatile bool master0Finished;
extern volatile bool master1Finished;
extern volatile bool memFinished;
extern void MemDmaTransfer(uint8_t* dest ,uint8_t* src, uint32_t transferBytes);
#endif

