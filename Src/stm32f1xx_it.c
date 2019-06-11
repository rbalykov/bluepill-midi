#include "main.h"
#include "stm32f1xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_FS;

void NMI_Handler		(void) 	{}
void HardFault_Handler	(void) 	{ while (1) {} }
void MemManage_Handler	(void) 	{ while (1) {} }
void BusFault_Handler	(void) 	{ while (1) {} }
void UsageFault_Handler	(void) 	{ while (1) {} }
void SVC_Handler		(void) 	{}
void DebugMon_Handler	(void)	{}
void PendSV_Handler		(void)	{}

void SysTick_Handler			(void)
	{ HAL_IncTick();						}
void USB_LP_CAN1_RX0_IRQHandler	(void)
	{ HAL_PCD_IRQHandler(&hpcd_USB_FS); 	}
void EXTI9_5_IRQHandler			(void)
	{ HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9); }

