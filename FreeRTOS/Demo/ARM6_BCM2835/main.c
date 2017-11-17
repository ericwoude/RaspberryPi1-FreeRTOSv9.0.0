#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "bcm2835.h"
#include "raspberrypi1.h"

SemaphoreHandle_t led1Semaphore;
SemaphoreHandle_t led2Semaphore;

void task1(void *pParam) {

	int i = 0;
	while(1) {
		i++;
		xSemaphoreTake(led1Semaphore, portMAX_DELAY);
		bcm2835_gpio_set(_RPI1_RDY_LED_PIN);
		// 1 sec
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		xSemaphoreGive(led2Semaphore);
	}
}

void task2(void *pParam) {

	int i = 0;
	while(1) {
		i++;
		xSemaphoreTake(led2Semaphore, portMAX_DELAY);
		bcm2835_gpio_clr(_RPI1_RDY_LED_PIN);
		// 1 sec
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		xSemaphoreGive(led1Semaphore);
	}
}

int main (void) {
	bcm2835_init();

	/* Rdy Led as output */
	bcm2835_gpio_fsel(_RPI1_RDY_LED_PIN, BCM2835_GPIO_FSEL_OUTP);

    /* Attempt to create a semaphore. */
    led1Semaphore = xSemaphoreCreateBinary();
    led2Semaphore = xSemaphoreCreateBinary();

    if( led1Semaphore == NULL || led2Semaphore == NULL ) {
        /* There was insufficient FreeRTOS heap available for the semaphore to
        be created successfully. */
    		while(1);
    }

    xSemaphoreGive(led1Semaphore);

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1);
}
