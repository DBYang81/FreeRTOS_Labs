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
#include "Fmutex.h"
#include "LpcUart.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();
	/* Initial LED0 state is off */
	Board_LED_Set(0, false);
}

#if 0
/* Set up the buttons */
DigitalIoPin Sw1(0, 17, DigitalIoPin::pullup, true);
DigitalIoPin Sw2(1, 11, DigitalIoPin::pullup, true);
DigitalIoPin Sw3(1, 9, DigitalIoPin::pullup, true);
#endif

LpcPinMap none = {-1, -1}; // unused pin has negative values in
LpcPinMap txpin = {.port = 0, .pin = 18 }; // transmit pin that goes to debugger's UART->USB converter
LpcPinMap rxpin = { .port = 0, .pin = 13 }; // receive pin that goes to debugger's UART->USB converter
LpcUartConfig cfg = {
		.pUART = LPC_USART0,
		.speed = 115200,
		.data = UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1,
		.rs485 = false,
		.tx = txpin,
		.rx = rxpin,
		.rts = none,
		.cts = none
};

/* Struct the button */
struct ButtonTaskInfo{
	int button_nr;
};

/* Setup the port and pin in the array */
static const int ports[] {0, 0, 1, 1}; // first element is dummy
static const int pins [] {0, 17, 11, 9};


LpcUart lpcUart(cfg);
Fmutex m;

/* vButtonTask function */
static void vButtonTask(void *pvParameters){
	ButtonTaskInfo *bti = static_cast <ButtonTaskInfo *> (pvParameters);
	DigitalIoPin button(ports[bti->button_nr], pins[bti->button_nr], DigitalIoPin::pullup, true);
	while(1){
		if(button.read()){
			char buff[50];
			snprintf(buff, 50, "Sw%d pressed\r\n", bti->button_nr);
			m.lock();
			lpcUart.write(buff);
			m.unlock();
		}
		vTaskDelay(100);
	}
}



#if 0
/* Set up the Mutex */
Fmutex m;

/* SW1 toggle thread */
static void Sw1Task1(void *pvParameters) {
	while(1){
		if(Sw1.read()){
			m.lock();
			lpcUart.write("Sw1 pressed\r\n");
			m.unlock();
		}
		vTaskDelay(100);
	}
}

/* SW2 toggle thread */
static void Sw2Task2(void *pvParameters) {
	while(1){
		if(Sw2.read()){
			m.lock();
			lpcUart.write("Sw2 pressed\r\n");
			m.unlock();
		}
		vTaskDelay(100);
	}
}
/* SW3 toggle thread */
static void Sw3Task3(void *pvParameters) {
	while(1){

		if(Sw3.read()){
			m.lock();
			lpcUart.write("Sw3 pressed\r\n");
			m.unlock();
		}
		vTaskDelay(100);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/
#endif


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

//	LpcUart *dbgu = new LpcUart(cfg);
//	Fmutex *mux = new Fmutex();

	static ButtonTaskInfo t1 {1};
	static ButtonTaskInfo t2 {2};
	static ButtonTaskInfo t3 {3};

#if 0
	/* Sw1 toggle thread */
	xTaskCreate(Sw1Task1, "vTaskSw1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Sw2 toggle thread */
	xTaskCreate(Sw2Task2, "vTaskSw2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Sw3 toggle thread */
	xTaskCreate(Sw3Task3, "vTaskSw3",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);
#endif

#if 1
	xTaskCreate(vButtonTask, "Button1",
				configMINIMAL_STACK_SIZE + 80, &t1, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	xTaskCreate(vButtonTask, "Button2",
				configMINIMAL_STACK_SIZE + 80, &t2, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	xTaskCreate(vButtonTask, "Button3",
				configMINIMAL_STACK_SIZE + 80, &t3, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);
#endif

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}


