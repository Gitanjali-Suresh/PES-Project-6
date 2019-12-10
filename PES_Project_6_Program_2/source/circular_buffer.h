/******************************************************************************
*                                                                             *
*                       PRINCIPLES OF EMBEDDED SOFTWARE                       *
*                   PROJECT 6 - FreeRTOS, ADC, DAC, DMA, DSP                  *
*   Project By:GITANJALI SURESH (GISU9983) & SARAYU MANAGOLI (SAMA2321)       *
*                   Cross Platform IDE: MCUXpresso IDE v11                    *
*                   Compiler: MinGW/GNU gcc v8.2.0 (PC version)               *
*                       Cross-Compiler: ARM GCC (FB version)                  *
*                                                                             *
*******************************************************************************/

#ifndef _CIRCULAR_BUFFER_H

#define _CIRCULAR_BUFFER_H

#include <stdbool.h>

#define FREQ_SIZE 126
#define BUF_SIZE 64

typedef struct
{
    uint32_t *buffer;
    size_t head;
    size_t tail;
    size_t length;
    size_t count;
    bool full;
}circ_buf_s;

typedef circ_buf_s circ_buf_v;
typedef circ_buf_v* circ_buf_p;

typedef enum
{
    success = 0,
    fail
}buf_status;

void buf_reset(circ_buf_p c_buf);
circ_buf_p buf_init(uint32_t* buf, size_t size);
buf_status buf_verify(circ_buf_p c_buf);
buf_status ptr_validity(uint32_t* buf);
buf_status buf_free(circ_buf_p c_buf);
bool buf_full(circ_buf_p c_buf);
bool buf_empty(circ_buf_p c_buf);
size_t buf_size(circ_buf_p c_buf);
void inc_ptr(circ_buf_p c_buf);
void dec_ptr(circ_buf_p c_buf);
buf_status buf_write(circ_buf_p c_buf, uint32_t data);
buf_status buf_read(circ_buf_p c_buf, uint32_t *data);
void buf_print(circ_buf_p c_buf);

#endif
