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
 * LOGGER
 *****************************************************************************/

/********* Standard Header Files *********/
#include <stdint.h>

/********* User defined Header Files *********/
#include "logger.h"

/********* Board specific Header Files *********/
#include "fsl_debug_console.h"

/********** Function to print different modes of the project **********/
void log_Level(eLog_level mode)
{
	if(mode == Status)
	{
		PRINTF("\n\rSTATUS: ");
	}
	else if(mode == Debug)
	{
		PRINTF("\n\rDEBUG: ");
	}
	return;
}

/********** Function to print different function names **********/
void log_Str(eLog_level mode, eFunction_name func, char *string)
{
	log_Level(mode);
	Print_Timestamp();
	if(func == DAC_task)
	{
		PRINTF(": DAC_task: ");
	}
	else if(func == ADC_task)
	{
		PRINTF(": ADC_task: ");
	}
	else if(func == DSP_task)
	{
		PRINTF(": DSP_task: ");
	}
	else if(func == main_fun)
	{
		PRINTF(": main: ");
	}
	PRINTF("%s",string);
	return;
}

/********** Function to print a number **********/
void log_Int(uint32_t number)
{
	PRINTF("%d",number);
}

/********** Function to print a character **********/
void log_Char(char ch)
{
	PRINTF("%c",ch);
}

/********** Function to print a string (used only for uc testing) **********/
void log_string(char *str)
{
	PRINTF("%s",str);
}
