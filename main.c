/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include <stdio.h>



#define CCM_RAM __attribute__((section(".ccmram")))

// ----------------------------------------------------------------------------

#include "led.h"

#define BLINK_PORT_NUMBER         (3)
#define BLINK_PIN_NUMBER_GREEN    (12)
#define BLINK_PIN_NUMBER_ORANGE   (13)
#define BLINK_PIN_NUMBER_RED      (14)
#define BLINK_PIN_NUMBER_BLUE     (15)
#define BLINK_ACTIVE_LOW          (false)

struct led blinkLeds[4];

// ----------------------------------------------------------------------------
/*-----------------------------------------------------------*/

/*
 * The LED timer callback function.  This does nothing but switch the red LED
 * off.
 */


/*-----------------------------------------------------------*/

/* The LED software timer.  This uses vLEDTimerCallback() as its callback
 * function.
 */

/*-----------------------------------------------------------*/
// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//------------------------------------------------------------------------------





// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


//----------------------------------------------------------------------------
#define Length 20 //length of message

static void StartProgram( TimerHandle_t xTimer );
static void SCallback1( TimerHandle_t xTimer );
static void SCallback2( TimerHandle_t xTimer );
static void SCallback3( TimerHandle_t xTimer );
static void RCallback( TimerHandle_t xTimer );


QueueHandle_t MyQueue;

SemaphoreHandle_t semaphore1;
SemaphoreHandle_t semaphore2;
SemaphoreHandle_t semaphore3;
SemaphoreHandle_t semaphore4;

TimerHandle_t periodic1;
TimerHandle_t periodic2;
TimerHandle_t periodic3;
TimerHandle_t periodic4;
TimerHandle_t NonPeriodic;

int QueueLength = 3;
int TaskCounter1,TaskCounter2,TaskCounter3;
int BlockCounter1,BlockCounter2,BlockCounter3;
int RecivedCounter;
int MAX ,MIN;
int P1,P2,P3,P4;
int state = 0;
TickType_t SumOfAllTicks = 0;
int counterPeriod1,counterPeriod2,counterPeriod3;
int cp1,cp2,cp3;

int GetPeriod(int x , int y)
{
	int RandomNumber = (rand() % (y - x + 1)) + x;
    return RandomNumber;
}

void reset()
{
	if(state==0)
	{
		state++;

		MIN = 50;
		MAX = 150;

		xQueueReset(MyQueue);

		RecivedCounter = 0;
		TaskCounter1=0;TaskCounter2=0;TaskCounter3=0;
		BlockCounter1=0;BlockCounter2=0;BlockCounter3=0;
		counterPeriod1 = 0;counterPeriod2=0;counterPeriod3=0;
		cp1=0;cp2=0;cp3=0;

		P1 = GetPeriod(MIN,MAX);
		P2 = GetPeriod(MIN,MAX);
		P3 = GetPeriod(MIN,MAX);
		P4 = 100;

		printf("%s\n","Let's start the program");

	    xSemaphoreTake(semaphore1, 0);
	    xSemaphoreTake(semaphore2, 0);
		xSemaphoreTake(semaphore3, 0);
		xSemaphoreTake(semaphore4, 0);

		return;
	}
	if(RecivedCounter==1000&&state>0&&state<7)
		{
		    printf("%s %d\n","The total number of successfully sent messages is :", TaskCounter1 + TaskCounter2 + TaskCounter3);
		    printf("%s %d\n","The total number of blocked  messages is :", BlockCounter1 + BlockCounter2 + BlockCounter3);
			printf("%s %d\n","The total number of transmitted message of task 1 is :", TaskCounter1);
			printf("%s %d\n","The total number of blocked message of task 1 is :", BlockCounter1);
			printf("%s %d\n","The total number of transmitted message of task 2 is :", TaskCounter2);
			printf("%s %d\n","The total number of blocked message of task 2 is :", BlockCounter2);
			printf("%s %d\n","The total number of transmitted message of task 3 is :", TaskCounter3);
			printf("%s %d\n","The total number of blocked message of task 3 is :", BlockCounter3);
			printf("%s %d\n","The total number of receivers is :", RecivedCounter);
			printf("%s %d\n","The Lower bound is   :", MIN);
			printf("%s %d\n","The upper bound is  :", MAX);
			printf("%s %d\n","The average sender time period of task 1 is :", counterPeriod1/cp1);
			printf("%s %d\n","The average sender time period of task 2 is :", counterPeriod2/cp2);
			printf("%s %d\n","The average sender time period of task 3 is :", counterPeriod3/cp3);

			state++;
			MIN = MIN + 30;
			MAX = MAX + 50;
			xQueueReset(MyQueue);
			RecivedCounter = 0;
			TaskCounter1=0;TaskCounter2=0;TaskCounter3=0;
			BlockCounter1=0;BlockCounter2=0;BlockCounter3=0;
			counterPeriod1 = 0;counterPeriod2=0;counterPeriod3=0;
			cp1=0;cp2=0;cp3=0;
		}
	if(state==7)
	{
		printf("%s\n","Game Over");
		state = 0;
		MIN = 50;
		MAX = 150;
		xQueueReset(MyQueue);
		RecivedCounter = 0;
		TaskCounter1=0;TaskCounter2=0;TaskCounter3=0;
		BlockCounter1=0;BlockCounter2=0;BlockCounter3=0;
		counterPeriod1 = 0;counterPeriod2=0;counterPeriod3=0;
		cp1=0;cp2=0;cp3=0;
		xTimerStop(periodic1,0);xTimerStop(periodic2,0);xTimerStop(periodic3,0);xTimerStop(periodic4,0);
		xTimerDelete(periodic1,0);xTimerDelete(periodic2,0);xTimerDelete(periodic3,0);xTimerDelete(periodic4,0);
		vTaskEndScheduler();
	}
}

//-------------------------------------------------------------------------

void senderTask1(void *pvParameters)
{
  char message[Length];
  while (1)
     {
	P1 = GetPeriod(MIN,MAX);
	cp1++;
	counterPeriod1 = counterPeriod1 + P1;
	xTimerChangePeriod(periodic1, pdMS_TO_TICKS(P1), 0);
	xSemaphoreTake(semaphore1, portMAX_DELAY);
    TickType_t currentTime = xTaskGetTickCount();
    snprintf(message, Length, "Time is %u", currentTime);
    if (xQueueSend(MyQueue, message, 0) != pdPASS)
          {
    	   BlockCounter1++;
          }
    else
        {
        	TaskCounter1++;
        }

     }
}
void senderTask2(void *pvParameters)
{
  char message[Length];
  while (1)
     {
	 P2 = GetPeriod(MIN,MAX);
	 counterPeriod2 = counterPeriod2 + P2;
	 cp2++;
	 xTimerChangePeriod(periodic2, pdMS_TO_TICKS(P2), 0);
	 xSemaphoreTake(semaphore2, portMAX_DELAY);
    TickType_t currentTime = xTaskGetTickCount();
    snprintf(message, Length, "Time is %u", currentTime);
    if (xQueueSend(MyQueue, message, 0) != pdPASS)
          {
    	BlockCounter2++;

          }
    else
        {
        	TaskCounter2++;
        }

     }
}
void senderTask3(void *pvParameters)
{
  char message[Length];

  while (1)
     {
	P3 = GetPeriod(MIN,MAX);
	counterPeriod3 = counterPeriod3 + P3;
	cp3++;
	xTimerChangePeriod(periodic3, pdMS_TO_TICKS(P3), 0);
	xSemaphoreTake(semaphore3, portMAX_DELAY);
    TickType_t currentTime = xTaskGetTickCount();
    snprintf(message, Length, "Time is %u", currentTime);
    if (xQueueSend(MyQueue, message, 0) != pdPASS)
          {
    	BlockCounter3++;
          }
    else
        {
        	TaskCounter3++;
        }
     }
}

void receiverTask(void *pvParameters)
{
  char receivedMessage[Length];
  while (1)
   {
	 xSemaphoreTake(semaphore4, portMAX_DELAY);
    if (xQueueReceive(MyQueue, receivedMessage, 0) == pdPASS)
              {
    	RecivedCounter++;
 printf("The number of receivers is %u: %s\n", RecivedCounter, receivedMessage);
               }
    else
                    {
  printf("No message in the queue...\n");
                     }
    }

}



int main(void)
{
  MyQueue = xQueueCreate(3, Length);

  semaphore1 = xSemaphoreCreateBinary();
  semaphore2 = xSemaphoreCreateBinary();
  semaphore3 = xSemaphoreCreateBinary();
  semaphore4 = xSemaphoreCreateBinary();

  reset();

  xTaskCreate(senderTask1, "Sender Task1", 1000, NULL, 1, NULL);
  xTaskCreate(senderTask2, "Sender Task2", 1000, NULL, 1, NULL);
  xTaskCreate(senderTask3, "Sender Task3", 1000, NULL, 2, NULL);
  xTaskCreate(receiverTask, "Receiver Task", 1000, NULL,3,NULL);

//  	NonPeriodic = xTimerCreate("Start",pdMS_TO_TICKS(1),pdFALSE,NULL,StartProgram);
        periodic1 = xTimerCreate("ST1",pdMS_TO_TICKS(P1),pdTRUE,NULL,SCallback1);
  	    periodic2 = xTimerCreate("ST2",pdMS_TO_TICKS(P2),pdTRUE,NULL,SCallback2);
  	    periodic3 = xTimerCreate("ST3",pdMS_TO_TICKS(P3),pdTRUE,NULL,SCallback3);
  	    periodic4 = xTimerCreate("R",pdMS_TO_TICKS(P4),pdTRUE,NULL,RCallback);


     BaseType_t A =  xTimerStart(periodic1, 0 );
     BaseType_t B =  xTimerStart(periodic2, 0 );
     BaseType_t C =  xTimerStart(periodic3, 0 );
     BaseType_t D =  xTimerStart(periodic4, 0 );
//   BaseType_t E = xTimerStart(NonPeriodic, 0 );

//  	if(NonPeriodic==NULL)
//  	    printf("%s %d\n","There is an error in the timer :", 0);
//  	   else
//  	    printf("%s %d\n","successful timer :", 1);

  	    if(A!=pdPASS)
  	    	printf("%s %d\n","There is an error in the timer :", 0);
  	       else
  	    	 printf("%s %d\n","successful timer :", 1);

  	    if(B!=pdPASS)
  	    	printf("%s %d\n","There is an error in the timer :", 0);
  	       else
  	    	 printf("%s %d\n","successful timer :", 1);

  	    if(C!=pdPASS)
  	    	printf("%s %d\n","There is an error in the timer :", 0);
  	  	   else
  	  		printf("%s %d\n","successful timer :", 1);

         vTaskStartScheduler();

  return 0;
}
// ----------------------------------------------------------------------------


static void StartProgram( TimerHandle_t xTimer )
{
	  printf("%s\n","Let's start the program");
	  xSemaphoreTake(semaphore1, 0);
	  xSemaphoreTake(semaphore2, 0);
	  xSemaphoreTake(semaphore3, 0);
	  xSemaphoreTake(semaphore4, 0);
}

static void SCallback1( TimerHandle_t xTimer )
{
	xSemaphoreGive(semaphore1);
}

static void SCallback2( TimerHandle_t xTimer )
{
	xSemaphoreGive(semaphore2);
}

static void SCallback3( TimerHandle_t xTimer )
{
	xSemaphoreGive(semaphore3);
}

static void RCallback( TimerHandle_t xTimer )
{
	xSemaphoreGive(semaphore4);
	reset();
}


void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

