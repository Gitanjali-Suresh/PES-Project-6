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
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_dac.h"
#include "fsl_adc16.h"
#include "fsl_dac.h"
#include "fsl_dma.h"
#include "fsl_dmamux.h"
#include "clock_config.h"
#include "pin_mux.h"

/* User defined includes */
#include "circular_buffer.h"
#include "logger.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_DAC_BASEADDR DAC0
uint16_t sine_val[100] = {
		2482,2559.985825,2637.663876,2714.727593,2790.87284,2865.799107,2939.210694,3010.81788,3080.338063,3147.496879,3212.029283,3273.680595,3332.207506,3387.379035,3438.977448,3486.799107,3530.655284,3570.372897,3605.795199,3636.782396,3663.212193,3684.980286,3702.000765,3714.206459,3721.549197,3724,3721.549197,3714.206459,3702.000765,3684.980286,3663.212193,3636.782395,3605.795199,3570.372896,3530.655283,3486.799107,3438.977447,3387.379035,3332.207505,3273.680595,3212.029283,3147.496879,3080.338063,3010.81788,2939.210694,2865.799107,2790.872839,2714.727592,2637.663876,2559.985825,2481.999999,2404.014174,2326.336123,2249.272407,2173.12716,2098.200892,2024.789305,1953.182119,1883.661936,1816.50312,1751.970716,1690.319404,1631.792494,1576.620964,1525.022552,1477.200893,1433.344716,1393.627103,1358.204801,1327.217604,1300.787807,1279.019714,1261.999234,1249.793541,1242.450803,1240,1242.450803,1249.793541,1261.999235,1279.019714,1300.787807,1327.217605,1358.204801,1393.627104,1433.344717,1477.200893,1525.022553,1576.620965,1631.792495,1690.319405,1751.970717,1816.503121,1883.661938,1953.182121,2024.789306,2098.200894,2173.127161,2249.272408,2326.336125,2404.014176
};

#define DEMO_ADC16_BASE ADC0
#define DEMO_ADC16_CHANNEL_GROUP 0U
#define DEMO_ADC16_USER_CHANNEL 0U /*PTE20, ADC0_SE0 */
adc16_config_t adc16ConfigStruct;
adc16_channel_config_t adc16ChannelConfigStruct;

volatile uint8_t dac_write = 0; //Flag for DAC
volatile uint8_t adc_read = 0; //Flag for ADC
#define dac_task_PRIORITY (tskIDLE_PRIORITY + 1)
#define adc_task_PRIORITY (tskIDLE_PRIORITY + 1)
#define dsp_task_PRIORITY (tskIDLE_PRIORITY + 1)

circ_buf_p adc_buf;
uint32_t adc_arr[BUF_SIZE], dsp_arr[BUF_SIZE];  //Declaration of ADC and DSP buffers
buf_status status;
volatile uint32_t adc_val;
uint32_t buf_data = 0;
volatile uint32_t i = 0;
#define DMA_CHANNEL 0
#define DMA_SOURCE 63 //Total number of elements to be present in ADC buffer

uint32_t max;
uint32_t min;
uint32_t sum = 0;
uint32_t avg = 0;
uint32_t sd, var, temp1 = 0, temp2 = 0;

dma_transfer_config_t transferConfig;
dma_handle_t g_DMA_Handle;
volatile bool g_Transfer_Done = false;  //Flag for DMA transfer
volatile bool transfer_complete = false;

uint8_t hour, minute, second, n;

SemaphoreHandle_t xMutex;

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);
static void dac_task(void *pvParameters);  //Task 1
static void adc_task(void *pvParameters);  //Task 2
static void dsp_task(void *pvParameters);  //Task 3

static void DMA_Callback(dma_handle_t *handle, void *param)
{
    g_Transfer_Done = true;
}

#define D_MODE
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

    LED_BLUE_INIT(1);
    LED_GREEN_INIT(1);
    LED_RED_INIT(1);
#ifdef D_MODE
	log_Str(Debug, main_fun, "LEDs initialized.");
#endif

    xMutex = xSemaphoreCreateMutex();  //Mutex declaration
#ifdef D_MODE
	log_Str(Debug, main_fun, "Semaphore created.");
#endif

    /* DAC Initialization */
    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DEMO_DAC_BASEADDR, &dacConfigStruct);
    DAC_Enable(DEMO_DAC_BASEADDR, true);             /* Enable output. */
    DAC_SetBufferReadPointer(DEMO_DAC_BASEADDR, 0U);  //Setting the buffer read pointer

	#ifdef D_MODE
    	log_Str(Debug, main_fun, "DAC Initialized.");
	#endif

    /* Circular Buffer Initialization */
    uint32_t *buf = malloc(BUF_SIZE * sizeof(uint32_t));	//Creating a pointer to store the circular buffer contents
    status = ptr_validity(buf);								//Checking if the buffer pointer is valid
	#ifdef D_MODE
    	if(status == success)
    	{
    		log_Str(Debug, main_fun, "Valid buffer pointer created");
    	}
    	if(status == fail)
    	{
    		log_Str(Debug, main_fun, "Buffer pointer creation failed!");
    		LED_RED_ON();
    	}
	#endif
    adc_buf = buf_init(buf,BUF_SIZE);		//Initializing the circular buffer
    status = buf_verify(adc_buf);			//Verifying the initialization of the circular buffer
	#ifdef D_MODE
    	if(status == success)
    	{
    		log_Str(Debug, main_fun, "Buffer initialized successfully");
    	}
    	if(status == fail)
    	{
    		log_Str(Debug, main_fun, "Buffer initializationn failed!");
    		LED_RED_ON();
    	}
	#endif

    /* ADC Initialization */
	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	#ifdef BOARD_ADC_USE_ALT_VREF
	    adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
	#endif
    ADC16_Init(DEMO_ADC16_BASE, &adc16ConfigStruct);
    ADC16_EnableHardwareTrigger(DEMO_ADC16_BASE, false); /* Make sure the software trigger is used. */
	#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
	    if (kStatus_Success == ADC16_DoAutoCalibration(DEMO_ADC16_BASE))
	    {
			#ifdef D_MODE
	        	log_Str(Debug, main_fun, "ADC16_DoAutoCalibration() Done.");
			#endif
	    }
	    else
	    {
			#ifdef D_MODE
	        	log_Str(Debug, main_fun, "ADC16_DoAutoCalibration() Failed.");
			#endif
	        LED_RED_ON();
	    }
	#endif /* FSL_FEATURE_ADC16_HAS_CALIBRATION */

	adc16ChannelConfigStruct.channelNumber = DEMO_ADC16_USER_CHANNEL;
	adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
	#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
	    adc16ChannelConfigStruct.enableDifferentialConversion = false;
	#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */

	#ifdef D_MODE
	    log_Str(Debug, main_fun, "ADC Initialized.");
	#endif

	/* DMA Initialization */
	DMAMUX_Init(DMAMUX0);
    DMAMUX_SetSource(DMAMUX0, DMA_CHANNEL, DMA_SOURCE);
	DMAMUX_EnableChannel(DMAMUX0, DMA_CHANNEL);
    /* Configure DMA one shot transfer */
	DMA_Init(DMA0);
    DMA_CreateHandle(&g_DMA_Handle, DMA0, DMA_CHANNEL);
	DMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);

	#ifdef D_MODE
		log_Str(Debug, main_fun, "DMA Initialized.");
	#endif

	/* Task Creations */
    xTaskCreate(dac_task, "DAC_task", configMINIMAL_STACK_SIZE + 10, NULL, dac_task_PRIORITY, NULL);
	#ifdef D_MODE
    	log_Str(Debug, main_fun, "DAC Task Created!");
	#endif
    xTaskCreate(adc_task, "ADC_task", configMINIMAL_STACK_SIZE + 10, NULL, adc_task_PRIORITY, NULL);
	#ifdef D_MODE
    	log_Str(Debug, main_fun, "ADC Task Created!");
	#endif
    xTaskCreate(dsp_task, "DSP_task", configMINIMAL_STACK_SIZE + 10, NULL, dsp_task_PRIORITY, NULL);
	#ifdef D_MODE
    	log_Str(Debug, main_fun, "DSP Task Created!");
	#endif


    log_Str(Status, main_fun, "Scheduling started.");
    /* Start scheduling. */
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Software timer callback.
 */
static void SwTimerCallback(TimerHandle_t xTimer)
{
    dac_write = 1;
    adc_read = 1;
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
	xSemaphoreTake(xMutex,portMAX_DELAY);	// Implementation of Mutex for shared LED resource
	LED_BLUE_OFF();
	LED_GREEN_TOGGLE();
	xSemaphoreGive(xMutex);
}

static void dac_task(void *pvParameters)
{
	static uint8_t j = 0;
    TimerHandle_t SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
    							 pdMS_TO_TICKS(33), /* Timer period. */
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
    		dac_write = 0; //Resetting the DAC flag
    	}
    }
}

static void adc_task(void *pvParameters)
{
	   while(1)
	   {
		   if(adc_read)
		   {
			    ADC16_SetChannelConfig(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP, &adc16ChannelConfigStruct);
		        while (0U == (kADC16_ChannelConversionDoneFlag &
		                      ADC16_GetChannelStatusFlags(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP)))
		        {
		        }
		        adc_read = 0;  //Resetting the ADC flag
		        adc_val = ADC16_GetChannelConversionValue(DEMO_ADC16_BASE, DEMO_ADC16_CHANNEL_GROUP);  //Function to obtain ADC value

		        buf_write(adc_buf,adc_val); //Updating the ADC value to the ADC buffer
		        if(buf_full(adc_buf) == true)  //Check for buffer full
		        {
		        	log_Str(Status, ADC_task, "Buffer Full!");
		        	while(!buf_empty(adc_buf))
		           	{
     	        		status = buf_read(adc_buf,&buf_data);
		        	    adc_arr[i] = buf_data;
		        	    i++;
		        	}
		        	i = 0;
		        	log_Str(Status, ADC_task, "Initiating DMA Transfer...");
		        	xSemaphoreTake(xMutex,portMAX_DELAY);	// Implementation of Mutex for shared LED resource
		        	LED_GREEN_OFF();
		        	LED_BLUE_TOGGLE();
		        	vTaskDelay( xDelay ); //To generate a delay of 500ms
		        	xSemaphoreGive(xMutex);
		        	DMA_PrepareTransfer(&transferConfig, adc_arr, sizeof(adc_arr[0]), dsp_arr, sizeof(dsp_arr[0]), sizeof(adc_arr),
		        		                                kDMA_MemoryToMemory);
		        	DMA_SubmitTransfer(&g_DMA_Handle, &transferConfig, kDMA_EnableInterrupt);
		            DMA_StartTransfer(&g_DMA_Handle);
		            /* Wait for DMA transfer finish */
		            while (g_Transfer_Done != true)
		            {
		            }
		            transfer_complete = true; //Flag for DMA transfer complete
		            log_Str(Status, ADC_task, "DMA Transfer Complete!");
		        }
		   }
	   }
}

static void dsp_task(void *pvParameters)
{
	static uint8_t run = 0;
	while(run < 5)
	{
		if(transfer_complete == true)
		{
			transfer_complete = false;
			max = dsp_arr[0];
			min = dsp_arr[0];
			for(i = 0;i < BUF_SIZE;i++)
			{
				if(max < dsp_arr[i])
					max = dsp_arr[i];	//To calculate the maximum value in the DSP buffer
				if(min > dsp_arr[i])
					min = dsp_arr[i];	//To calculate the minimum value in the DSP buffer
				sum += dsp_arr[i];
			}
			avg = sum/BUF_SIZE; //To calculate the average value of the elements in the DSP buffer
			for(i = 0;i < BUF_SIZE;i++)
			{
				temp1 = dsp_arr[i] - avg;
				temp2 = temp2 + (temp1 * temp1);
			}
			var = (temp2/(BUF_SIZE-1));  //Variance of the elements in the DSP buffer
			sd = sqrt(temp2/(BUF_SIZE-1));  //Standard deviation of the elements in the DSP buffer
			log_Str(Status, DSP_task, "Report Generation");
			log_Str(Status, DSP_task, "");
			PRINTF("Max - %d \t Min - %d \t Avg - %d \t SD  - %d \t Var - %d",max, min, avg, sd, var);
			sum = 0;
			temp2 = 0;
			run++;
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

/****************************************** References *************************************
 * [1] FreeRTOS examples on MCUXpresso
 *******************************************************************************************/

