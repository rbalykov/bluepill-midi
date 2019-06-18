
#include "midi_ringbuffer.h"

MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx1;
MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx2;
MIDI_UART_Queue_TypeDef		MIDI_UART_Queue_Tx3;

MIDI_USB_Queue_TypeDef		MIDI_USB_Queue_DataIn;
MIDI_USB_Queue_TypeDef		MIDI_USB_Queue_DataOut;

MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_1;
MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_2;
MIDI_UART_RxHandler_TypeDef 	MIDI_UART_Rx_3;

// -----------------------------------------------------------------------------
// --- UART Queue --------------------------------------------------------------
// -----------------------------------------------------------------------------
void 	MIDI_UART_Queue_Init (MIDI_UART_Queue_TypeDef *queue)
{
	queue->semaphore  = 0;
	queue->ptrPop  = 0;
	queue->ptrPush = 0;
	memset(queue->data, 0, MIDI_UART_RINGBUFFER_SIZE);
}

uint8_t MIDI_UART_Queue_Push (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte)
{
//	if ((queue->ptrPush + 1)%MIDI_USB_RINGBUFFER_SIZE == queue->ptrPop)
	if (queue->length >= MIDI_UART_RINGBUFFER_SIZE)
		return 0;

//	while (	queue->semaphore) {}
//	queue->semaphore = 1;
	queue->data[queue->ptrPush] = byte[0];
	queue->ptrPush = (queue->ptrPush + 1)%MIDI_UART_RINGBUFFER_SIZE;
	queue->length ++;
//	queue->semaphore = 0;
	return 1;

}

uint8_t MIDI_UART_Queue_Pop  (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte)
{
	if (queue->length == 0)	return 0;

//	while (	queue->semaphore) {}
//	queue->semaphore = 1;
	byte[0] = queue->data[queue->ptrPop];
	queue->ptrPop = (queue->ptrPop + 1)%MIDI_UART_RINGBUFFER_SIZE;
	queue->length--;
//	queue->semaphore = 0;
	return 1;
}

uint8_t MIDI_UART_Queue_Next  (MIDI_UART_Queue_TypeDef *queue, uint8_t *byte)
{
	if (queue->length == 0) return 0;
	byte[0] = queue->data[queue->ptrPop];
	return 1;
}

uint8_t MIDI_UART_Queue_Empty  (MIDI_UART_Queue_TypeDef *queue)
{
	return (queue->length == 0);
}

// -----------------------------------------------------------------------------
// --- RX Parser ---------------------------------------------------------------
// -----------------------------------------------------------------------------

void 	MIDI_UART_RxHandler_Init (MIDI_UART_RxHandler_TypeDef *handler)
{
	handler->length = 0;
	memset(handler->data, 0, MIDI_MSG_MAX_SIZE);
}

void 	MIDI_UART_RxHandler_Push (MIDI_UART_RxHandler_TypeDef *handler, uint8_t byte)
{
	if (handler->length >= MIDI_MSG_MAX_SIZE) return;
	handler->data[handler->length] = byte;
	handler->length++;
}

void 	MIDI_UART_RxHandler_MsgStart (MIDI_UART_RxHandler_TypeDef *handler)
{
	handler->length = 1;
	handler->data[0] = handler->rxbuffer[0];
}



// -----------------------------------------------------------------------------
// --- USB Queue ---------------------------------------------------------------
// -----------------------------------------------------------------------------
void 	 MIDI_USB_Queue_Init 		(MIDI_USB_Queue_TypeDef *queue)
{
	queue->semaphore  = 0;
	queue->ptrPop  = 0;
	queue->ptrPush = 0;
	memset(queue->data, 0, MIDI_USB_RINGBUFFER_SIZE*MIDI_USB_MSG_SIZE);
}

uint8_t  MIDI_USB_Queue_Push 		(MIDI_USB_Queue_TypeDef *queue, uint8_t *message)
{

	uint8_t push = queue->ptrPush;
//	if ((queue->ptrPush + 1)%MIDI_USB_RINGBUFFER_SIZE == queue->ptrPop)
	if (queue->length >= MIDI_USB_RINGBUFFER_SIZE)
		return 0;

	while (	queue->semaphore) {}
	queue->semaphore = 1;
	for (uint16_t i=0; i<MIDI_USB_MSG_SIZE; i++)
		{ queue->data[push][i] = message[i];}
	queue->ptrPush = (queue->ptrPush + 1)%MIDI_USB_RINGBUFFER_SIZE;
	queue->length ++;
	queue->semaphore = 0;
	return 1;
}

uint8_t  MIDI_USB_Queue_Pop 		(MIDI_USB_Queue_TypeDef *queue, uint8_t *message)
{
	if (queue->length == 0)	return 0;

	while (	queue->semaphore) {}
	queue->semaphore = 1;
	for (uint16_t i=0; i<MIDI_USB_MSG_SIZE; i++)
		 { message[i] = queue->data[queue->ptrPop][i]; }
	queue->ptrPop = (queue->ptrPop + 1)%MIDI_USB_RINGBUFFER_SIZE;
	queue->length--;
	queue->semaphore = 0;
	return 1;
}

uint16_t MIDI_USB_Queue_Dump 		(MIDI_USB_Queue_TypeDef *queue,
									uint8_t *whereTo, uint16_t limit)
{
	uint16_t iterations = limit/MIDI_USB_MSG_SIZE;
	if (!iterations) 					return 0;
	if (MIDI_USB_Queue_Empty(queue)) 	return 0;

	uint16_t delta = 0;
	for (uint16_t i=0; i<iterations; i++)
	{
		if (MIDI_USB_Queue_Pop(queue, whereTo+delta))
			delta += MIDI_USB_MSG_SIZE;
		else
			return delta;
	}
	return delta;
}

uint8_t  MIDI_USB_Queue_Empty 	(MIDI_USB_Queue_TypeDef *queue)
{
	return (queue->length == 0);
}

// -----------------------------------------------------------------------------

