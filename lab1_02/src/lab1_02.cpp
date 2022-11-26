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

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

//void ledOff(int delay){
//	Board_LED_Set(0, false);
//	vTaskDelay(configTICK_RATE_HZ / delay);
//	Board_LED_Set(0,true);
//}

void morse_s(unsigned int dot){

	for(int i = 0; i < 3; i++){
		Board_LED_Set(0, true);
		vTaskDelay(dot);
		Board_LED_Set(0, false);
		vTaskDelay(dot);
	}
}

void morse_o(unsigned int dash){
	for(int j = 0; j < 3; j++){
		Board_LED_Set(0, true);
		vTaskDelay(dash);
		Board_LED_Set(0,false);
		vTaskDelay(dash);
	}
}



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
	int dot = configTICK_RATE_HZ / 20; // 1000 / 20 = 50
	int dash = dot * 3; // 3*50

	while(1){
		morse_s(dot); // 6d morse_s func itself has 3 rounds, 2dots for each round
		morse_o(dash); // 18d + 6d = 24d
		morse_s(dot); // 6d + 24d = 30d

		vTaskDelay(dot * 30); // 以dot为基数，这里delay的时间就是这个led发生一次SOS信号的总时间。将会给
	}						  // 后面的绿色led闪烁时的时间提供参考，因为需要在第二次发出信号时，同时闪烁绿色led。
}

/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {
	bool LedState = false;
	int dot = configTICK_RATE_HZ / 20; // 1000 / 20 = 50

	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		/* About a 7Hz on/off toggle rate */
		vTaskDelay(dot * 30);
	}
}

/* UART (or output) thread */
static void vUARTTask(void *pvParameters) {
	int min = 0;
	int sec = 0;
	while (1) {
		if(sec == 60){
			min++;
			sec = 0;
			DEBUGOUT("------------------ \r\n");
			if(min == 60){
				min = 0;
				sec = 0;
			}
		}
		DEBUGOUT("Time: %.2d : %.2d \r\n", min, sec);
		sec++;

		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ);
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

//	/* LED2 toggle thread */
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

