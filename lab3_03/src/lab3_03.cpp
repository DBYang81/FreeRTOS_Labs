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
#include "ITM_write.h"
#include "LpcUart.h"
#include "DigitalIoPin.h"
#include <cstring>
#include <cctype>
#include "queue.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define QUEUE_LENGTH_1 		10
#define QUEUE_LENGTH_2 		10
#define BINARY_SEMAPHORE_LENGTH	1
#define COMBINE_LENGTH (QUEUE_LENGTH_1 + QUEUE_LENGTH_2 + BINARY_SEMAPHORE_LENGTH)
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

struct debugEvent{
	const char* format;
	char* cmd;
	uint32_t data[2];
};


struct messageTask{
	LpcUart *uart;
	int button_nr;
};

/* Declare variables of the Queue*/
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;

/* Declare a variable of type QueueSetHandle_t, queue set to add two queues */
static QueueSetHandle_t xQueueSet = NULL;

/* Declare a semaphore*/
QueueHandle_t xSemaphore;


/* Setup the port and pin in the array */
static const int ports[]= {0, 0}; // first element is dummy
static const int pins[]={0,17};



/* Debug print thread */
static void vDebugTask(void *pvParameters) {
	QueueHandle_t xQueueThatContainsData;
	char buffer[64];
	debugEvent e;

	while (1) {
		xQueueThatContainsData = (QueueHandle_t) xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		xQueueReceive(xQueueThatContainsData, &e, portMAX_DELAY);
		snprintf(buffer, 64, e.format, e.cmd, e.data[0], e.data[1]);
		ITM_write(buffer);
	}
}

/* Read serial port thread */
static void vUartReadTask(void *pvParameters) {
	messageTask *tsk = static_cast<messageTask *>(pvParameters);
	debugEvent e;

	int count = 0;
	char str[32];


	while(1){
		int bytes = tsk->uart->read(str+count, 30-count); //lpcUart class returns number of char that has been read, return 0 meaning no char
		if(bytes > 0){
			count += bytes;
			str[count] = '\0';
			tsk->uart->write(str+count-bytes, bytes);
			for(int i = 0; i < count; i++){
				if(isspace(str[i])){
					count--;
					e.format = "Received cmd: %s, %d characters at %d\r\n";
					e.cmd = str;
					e.data[0] = count;
					e.data[1] = xTaskGetTickCount();
					xQueueSendToBack(xQueue1,
									(void *)&e,
									portMAX_DELAY);
					count = 0;
				}
			}
		}
		vTaskDelay(configTICK_RATE_HZ / 10);
	}
}


/* Button pressed thread */
static void vBtnTask(void *pvParameters) {
	messageTask *tsk = static_cast<messageTask *>(pvParameters);
	DigitalIoPin Button(ports[tsk->button_nr], pins[tsk->button_nr], DigitalIoPin::pullup, true);
	debugEvent e;
	char dscp[16] = {"button pressed"};

	while (1) {
		int tickCnt = 0;
		if(Button.read()){
			while(Button.read())tickCnt ++;
			e.format = "Received %s, length: %d, at %d\r\n";
			e.cmd = dscp;
			e.data[0] = tickCnt;
			e.data[1] = xTaskGetTickCount();
			xQueueSendToBack(xQueue2,
							(void *) &e,
							portMAX_DELAY);
		}
		/* About a 1s delay here */
		vTaskDelay(configTICK_RATE_HZ / 10);
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

	ITM_init();

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


	xQueueSet = xQueueCreateSet (COMBINE_LENGTH); //2 queues multiplied by 3 items per queue
	xQueue1 = xQueueCreate(QUEUE_LENGTH_1, sizeof(struct debugEvent));
	xQueue2 = xQueueCreate(QUEUE_LENGTH_2, sizeof(struct debugEvent));
	xSemaphore = xSemaphoreCreateBinary();


	//add two queues into the set
	xQueueAddToSet(xQueue1, xQueueSet);
	xQueueAddToSet(xQueue2, xQueueSet);
	xQueueAddToSet(xSemaphore, xQueueSet);


	LpcUart *uart = new LpcUart(cfg);

	static messageTask tsk = {uart, 1};

	/* Debug print thread */
	xTaskCreate(vDebugTask, "receiver",
				configMINIMAL_STACK_SIZE + 128, NULL, (tskIDLE_PRIORITY + 1UL),
				(TaskHandle_t *) NULL);

	/* Read serial port thread */
	xTaskCreate(vUartReadTask, "sendingTask1",
				configMINIMAL_STACK_SIZE + 128, &tsk, (tskIDLE_PRIORITY + 2UL),
				(TaskHandle_t *) NULL);

	/* Button pressed thread */
	xTaskCreate(vBtnTask, "sendingTask2",
				configMINIMAL_STACK_SIZE + 128, &tsk, (tskIDLE_PRIORITY + 2UL),
				(TaskHandle_t *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

