/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#include "FreeRTOS.h"
#include "task.h"
#include "heap_lock_monitor.h"
#include "DigitalIoPin.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* initilize the button */
DigitalIoPin Sw1(0, 17, DigitalIoPin::pinMode::pullup, true); //DigitalIoPin::pullup is also correct

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

/* LED1 toggle thread */
static void vLEDTask1(void *pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(0, LedState);
		LedState = (bool) !LedState;

		/* About a 3Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 6);
	}
}

/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {
	bool LedState = false;

	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(configTICK_RATE_HZ / 14);
	}
}

/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	unsigned int tickCnt = 0;
	unsigned int delay;
	bool LedState = false;

	while (1) {
		DEBUGOUT("Tick: %d \r\n", tickCnt);
		tickCnt++;

		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		if(int i = Sw1.read()) {
			delay = configTICK_RATE_HZ / 10; // 1）利用delay来调整打印的时间速度
		}else delay = configTICK_RATE_HZ; 	// 	2) 当按键后delay仅是之前的10分之一，速度快了10倍。

		vTaskDelay(delay);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}
/* end runtime statictics collection */

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
int main(void)
{
	prvSetupHardware();
	
	heap_monitor_setup();

	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* UART output thread, simply counts seconds */
	xTaskCreate(vUARTTask, "vTaskUart",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

