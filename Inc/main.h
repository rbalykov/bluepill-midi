#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "queue32.h"

void Error_Handler(void);
#define BOOT1_Pin 			GPIO_PIN_2
#define BOOT1_GPIO_Port 	GPIOB

#define BUTTON_Pin 			GPIO_PIN_9
#define BUTTON_GPIO_Port 	GPIOB
#define BUTTON_EXTI_IRQn 	EXTI9_5_IRQn
#define BUTTON_PRESSED 		(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == GPIO_PIN_RESET)

#define LEDTIMEMAX 7
#define LED_Pin 			GPIO_PIN_13
#define LED_GPIO_Port 		GPIOC

void led_on		(void);
void led_off	(void);
void led_toggle	(void);

void onNoteOn		(uint8_t ch, uint8_t note, uint8_t vel);
void onNoteOff		(uint8_t ch, uint8_t note, uint8_t vel);
void onCtlChange	(uint8_t ch, uint8_t num, uint8_t value);

extern stB4Arrq rxq;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
