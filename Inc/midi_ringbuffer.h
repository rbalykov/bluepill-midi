#ifndef __MIDI_RINGBUFFER_H_FILE__
#define __MIDI_RINGBUFFER_H_FILE__

#include <stdint.h>
#include <string.h>

//#include "stm32f1xx_hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIDI_UART_RINGBUFFER_SIZE	(64)

typedef struct _MIDI_UART_Queue_TypeDef
{
	uint8_t semaphore;
	uint8_t data[MIDI_UART_RINGBUFFER_SIZE];
	uint16_t ptrPush;
	uint16_t ptrPop;
	uint16_t length;
}MIDI_UART_Queue_TypeDef;

// -----------------------------------------------------------------------------
//    * MIDI_UART_Queue_Push/Pop - return 1 if pushed, 0 otherwise
// -----------------------------------------------------------------------------
void 	MIDI_UART_Queue_Init (MIDI_UART_Queue_TypeDef *queue);
uint8_t MIDI_UART_Queue_Push (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte);
uint8_t MIDI_UART_Queue_Pop  (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte);
uint8_t MIDI_UART_Queue_Next (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte);
uint8_t MIDI_UART_Queue_Empty(MIDI_UART_Queue_TypeDef *queue);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#define MIDI_UART_RXBUFFER_SIZE	(1)
#define MIDI_MSG_MAX_SIZE		(3)

typedef struct _MIDI_UART_RxHandler_TypeDef
{
	uint8_t  data[MIDI_MSG_MAX_SIZE];
	uint8_t  rxbuffer[MIDI_UART_RXBUFFER_SIZE];
	uint16_t length;
	uint8_t  cable_id;
}MIDI_UART_RxHandler_TypeDef;

void 	MIDI_UART_RxHandler_Init (MIDI_UART_RxHandler_TypeDef *handler);
void 	MIDI_UART_RxHandler_Push (MIDI_UART_RxHandler_TypeDef *handler, uint8_t byte);
void 	MIDI_UART_RxHandler_MsgStart (MIDI_UART_RxHandler_TypeDef *handler);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#define MIDI_USB_RINGBUFFER_SIZE	(64)
#define MIDI_USB_MSG_SIZE			(4)

typedef struct _MIDI_USB_Queue_TypeDef
{
	uint8_t semaphore;
	uint8_t data[MIDI_UART_RINGBUFFER_SIZE][MIDI_USB_MSG_SIZE];
	uint16_t ptrPush;
	uint16_t ptrPop;
	uint16_t length;
}MIDI_USB_Queue_TypeDef;

// -----------------------------------------------------------------------------
//	* MIDI_USB_Queue_Push - return 1 if pushed, 0 otherwise
//  * MIDI_USB_Queue_Dump - returns amount of data written from queue
// -----------------------------------------------------------------------------
void 	 MIDI_USB_Queue_Init 		(MIDI_USB_Queue_TypeDef *queue);
uint8_t  MIDI_USB_Queue_Push 		(MIDI_USB_Queue_TypeDef *queue, uint8_t *message);
uint8_t  MIDI_USB_Queue_Pop 		(MIDI_USB_Queue_TypeDef *queue, uint8_t *message);
uint16_t MIDI_USB_Queue_Dump 		(MIDI_USB_Queue_TypeDef *queue, uint8_t *whereTo, uint16_t limit);
uint8_t  MIDI_USB_Queue_Empty 		(MIDI_USB_Queue_TypeDef *queue);


extern MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx1;
extern MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx2;
extern MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx3;

extern MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_1;
extern MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_2;
extern MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_3;

extern MIDI_USB_Queue_TypeDef		MIDI_USB_Queue_DataIn;
extern MIDI_USB_Queue_TypeDef		MIDI_USB_Queue_DataOut;


#ifdef __cplusplus
}
#endif

#endif
