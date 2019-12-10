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

#ifndef _LOGGER_H

#define _LOGGER_H

#endif

typedef enum {
	main_fun,
	DAC_task,
	ADC_task,
	DSP_task
}eFunction_name;

typedef enum {
	Status,
	Debug
}eLog_level;

void log_Str(eLog_level mode, eFunction_name func, char *str);
void log_Int(uint32_t number);
void log_Char(char ch);
void log_string(char *str);
extern void Print_Timestamp(void);
