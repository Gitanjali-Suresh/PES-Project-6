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
/*****************************************************************************
 * CIRCULAR BUFFER OPERATIONS
 *****************************************************************************/

/********* Standard Header Files *********/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/********* Board specific Header Files *********/
#include "fsl_debug_console.h"

/********* User defined Header Files *********/
#include "circular_buffer.h"

/***************** Function to reset the circular buffer **************/
void buf_reset(circ_buf_p c_buf)
{
   c_buf->head = 0;
   c_buf->tail = 0;
   c_buf->full = false;
}

/***************** Function to initialize the circular buffer **************/
circ_buf_p buf_init(uint32_t* buf, size_t size)
{
    circ_buf_p c_buf = malloc(sizeof(circ_buf_v));
    if(c_buf != NULL)
    {
        c_buf->buffer = buf;
        c_buf->length = size;
        buf_reset(c_buf);
    }
    return c_buf;
}

/***************** Function to verify the circular buffer **************/
buf_status buf_verify(circ_buf_p c_buf)
{
    if(c_buf == NULL)
        return fail;
    else
        return success;
}

/***************** Function to check the validity of the buffer pointer **************/
buf_status ptr_validity(uint32_t* buf)
{
    if(buf == NULL)
        return fail;
    else
        return success;
}

/***************** Function to destroy the circular buffer **************/
buf_status buf_free(circ_buf_p c_buf)
{
	if(c_buf == NULL)
		return fail;
	free(c_buf);
	return success;
}

/***************** Function to check if the circular buffer is full **************/
bool buf_full(circ_buf_p c_buf)
{
    return c_buf->full;
}

/***************** Function to check if the circular buffer is empty **************/
bool buf_empty(circ_buf_p c_buf)
{
    return (!c_buf->full && (c_buf->head == c_buf->tail));
}

/***************** Function to return the count in the circular buffer **************/
size_t buf_size(circ_buf_p c_buf)
{
    c_buf->count = c_buf->length;
    if(!c_buf->full)
    {
        c_buf->count = (c_buf->head - c_buf->tail);
    }
    return c_buf->count;
}

/***************** Function to increment the buffer pointers **************/
void inc_ptr(circ_buf_p c_buf)
{
    if(c_buf->full)
    {
        c_buf->tail = (c_buf->tail+1) % c_buf->length;
    }
    c_buf->head = (c_buf->head+1) % c_buf->length;
    c_buf->full = (c_buf->head == c_buf->tail);
}

/***************** Function to decrement the buffer pointers **************/
void dec_ptr(circ_buf_p c_buf)
{
    c_buf->full = false;
    c_buf->tail = (c_buf->tail+1) % c_buf->length;
}


/***************** Function to write to the buffer **************/
buf_status buf_write(circ_buf_p c_buf, uint32_t data)
{
    if(!buf_full(c_buf))
    {
        c_buf->buffer[c_buf->head] = data;
        inc_ptr(c_buf);
        return success;
    }
    return fail;
}

/***************** Function to read from the buffer **************/
buf_status buf_read(circ_buf_p c_buf, uint32_t *data)
{
    if(!buf_empty(c_buf))
    {
        *data = c_buf->buffer[c_buf->tail];
        dec_ptr(c_buf);
        return success;
    }
    return fail;
}

/***************** Function to print the buffer status **************/
void buf_print(circ_buf_p c_buf)
{
    PRINTF("\n\rFull: %d, Empty: %d and Present Count: %u",buf_full(c_buf),buf_empty(c_buf),buf_size(c_buf));
}

/****************************************** References *************************************
 * [1] https://embedjournal.com/implementing-circular-buffer-embedded-c/
 *******************************************************************************************/
