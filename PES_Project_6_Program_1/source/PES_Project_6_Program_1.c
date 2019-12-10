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

/* Standard includes. */
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_dac.h"

/* User includes */
#include "logger.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_DAC_BASEADDR DAC0
uint16_t sine_val[100] = {
		2482,2559.985825,2637.663876,2714.727593,2790.87284,2865.799107,2939.210694,3010.81788,3080.338063,3147.496879,3212.029283,3273.680595,3332.207506,3387.379035,3438.977448,3486.799107,3530.655284,3570.372897,3605.795199,3636.782396,3663.212193,3684.980286,3702.000765,3714.206459,3721.549197,3724,3721.549197,3714.206459,3702.000765,3684.980286,3663.212193,3636.782395,3605.795199,3570.372896,3530.655283,3486.799107,3438.977447,3387.379035,3332.207505,3273.680595,3212.029283,3147.496879,3080.338063,3010.81788,2939.210694,2865.799107,2790.872839,2714.727592,2637.663876,2559.985825,2481.999999,2404.014174,2326.336123,2249.272407,2173.12716,2098.200892,2024.789305,1953.182119,1883.661936,1816.50312,1751.970716,1690.319404,1631.792494,1576.620964,1525.022552,1477.200893,1433.344716,1393.627103,1358.204801,1327.217604,1300.787807,1279.019714,1261.999234,1249.793541,1242.450803,1240,1242.450803,1249.793541,1261.999235,1279.019714,1300.787807,1327.217605,1358.204801,1393.627104,1433.344717,1477.200893,1525.022553,1576.620965,1631.792495,1690.319405,1751.970717,1816.503121,1883.661938,1953.182121,2024.789306,2098.200894,2173.127161,2249.272408,2326.336125,2404.014176
};
volatile uint8_t dac_write = 0;  //Declaration of DAC Flag
#define dac_task_PRIORITY (tskIDLE_PRIORITY + 1)

uint8_t hour, minute, second, n;

#define D_MODE
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
static void dac_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    dac_config_t dacConfigStruct;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SystemCoreClockUpdate();

    /* LED Initialization */
    LED_BLUE_INIT(1);
	#ifdef D_MODE
		log_Str(Debug, main_fun, "LED Initialized");
	#endif

    /* DAC Initialization */
    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DEMO_DAC_BASEADDR, true);             /* Enable output. */
    DAC_SetBufferReadPointer(DEMO_DAC_BASEADDR, 0U);	//Setting the buffer read point

	#ifdef D_MODE
		log_Str(Debug, main_fun, "DAC Initialized.");
	#endif

    /* Task Creation */
    xTaskCreate(dac_task, "DAC_task", configMINIMAL_STACK_SIZE + 10, NULL, dac_task_PRIORITY, NULL);
	#ifdef D_MODE
		log_Str(Debug, main_fun, "DAC Task Created!");
	#endif

	log_Str(Status, main_fun, "Scheduling started!");
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

static void SwTimerCallback(TimerHandle_t xTimer)
{
    dac_write = 1;
    LED_BLUE_TOGGLE();
	n += 10;
	if(n == 100)
	{
		//Steps to print the time stamps
		n = 0;
		second += 1;
		if(second == 60)
		{
			second = 0;
			minute += 1;
			if(minute == 60)
			{
				hour += 1;
				minute = 0;
			}
		}
	}
}

static void dac_task(void *pvParameters)
{
	static uint8_t j = 0;

	/* Timer Creation */
    TimerHandle_t SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
    							 pdMS_TO_TICKS(100), /* Timer period. */
                                 pdTRUE,             /* Enable auto reload. */
                                 0,                  /* ID is not used. */
                                 SwTimerCallback);   /* The callback function. */
    xTimerStart(SwTimerHandle, 0);

    for (;;)
    {
    	if(dac_write)
    	{
    		DAC_SetBufferValue(DEMO_DAC_BASEADDR, 0U, sine_val[j]);
    		j++;
    		if(j == 100)  //For 100 values of sine_val
    		{
    			j = 0;
    			log_Str(Status, DAC_task, "Sine wave generated!");
    		}
    		dac_write = 0;
    	}
    }
}

/************** Function to print timestamps **************/
extern void Print_Timestamp(void)
{
	log_Int(hour);
	log_Char(':');
	log_Int(minute);
	log_Char(':');
	log_Int(second);
	log_Char('.');
	log_Int(n);
}
