/*
 * midi-uart.c
 *
 *  Created on: 12 июн. 2019 г.
 *      Author: pi
 */

#include "main.h"
#include <string.h>

// UART Hardware
void MIDI_UART_IRQHandler(UART_HandleTypeDef *huart);
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

static uint8_t MIDI_Ring_Buffer[MIDI_USB_RING_SIZE]		[MIDI_USB_MSG_SIZE] = {0};
static uint16_t 	MIDI_Ring_PtrNext = 0,
					MIDI_Ring_Queue	  = 0;


typedef struct {
	uint8_t RingBuffer [MIDI_UART_RING_SIZE];
	uint8_t RingQueue;
	uint8_t OffsetPush;
	uint8_t OffsetPop;
}MIDI_UART_Handler_t;

MIDI_UART_Handler_t TX_RingBuffer[MIDI_UART_PORT_COUNT];


// -----------------------------------------------------------------------------
void MIDI_UART_RingInit(void)
{
	for (uint8_t i=0; i<MIDI_UART_PORT_COUNT; i++)
	{
		memset(TX_RingBuffer[i].RingBuffer, 0, MIDI_UART_RING_SIZE);
		TX_RingBuffer[i].OffsetPop 	= 0;
		TX_RingBuffer[i].OffsetPush = 0;
		TX_RingBuffer[i].RingQueue 	= 0;
	}
}

uint8_t	MIDI_TX_Push 	(uint8_t port, uint8_t *byte)
{
	if (port >= MIDI_UART_PORT_COUNT) return 0;

	if (TX_RingBuffer[port].RingQueue >= MIDI_UART_RING_SIZE)
	{
		TX_RingBuffer[port].RingQueue 	  = MIDI_UART_RING_SIZE;
		if (++TX_RingBuffer[port].OffsetPop >= MIDI_UART_RING_SIZE)
		   	 {TX_RingBuffer[port].OffsetPop  = 0;}
		// put overflow handler here or just lose a byte
	}

	TX_RingBuffer[port].RingBuffer [TX_RingBuffer[port].OffsetPush] = byte[0];
	if (++TX_RingBuffer[port].OffsetPush >= MIDI_UART_RING_SIZE)
	   	 {TX_RingBuffer[port].OffsetPush  = 0;}
	if (++TX_RingBuffer[port].RingQueue  >= MIDI_UART_RING_SIZE)
	   	 {TX_RingBuffer[port].RingQueue   = MIDI_UART_RING_SIZE;}

	return 1;
}

uint8_t	MIDI_TX_Pop 	(uint8_t port, uint8_t *byte)
{
	if (port >= MIDI_UART_PORT_COUNT) 		return 0;
	if (TX_RingBuffer[port].RingQueue == 0)	return 0;

	byte[0] = TX_RingBuffer[port].RingBuffer [TX_RingBuffer[port].OffsetPop];

	if (++TX_RingBuffer[port].OffsetPop >= MIDI_UART_RING_SIZE)
	   	 {TX_RingBuffer[port].OffsetPop  = 0;}

	--TX_RingBuffer[port].RingQueue;

	return 1;
}

void MIDI_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	/*	1) handle TX complete
	 * 	2) Handle RXNE
	 * */

	uint32_t isrflags   = READ_REG(huart->Instance->SR);
//	uint32_t cr1its     = READ_REG(huart->Instance->CR1);
//	uint32_t cr3its     = READ_REG(huart->Instance->CR3);
	uint32_t errorflags = 0x00U;

	/* If no error occurs */
	errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
	if(errorflags == RESET)
	{
		if((isrflags & USART_SR_RXNE) != RESET)
		{
			uint16_t tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
			UNUSED(tmp);

			__HAL_UART_CLEAR_FLAG (huart, UART_FLAG_RXNE);
		}

		if((isrflags & USART_SR_TC) != RESET)
		{
			// code here
			__HAL_UART_CLEAR_FLAG (huart, UART_FLAG_TC);
		}

	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

uint16_t MIDI_Message_Ring_Push (uint8_t * message)
{
	for (uint16_t i=0; i<MIDI_USB_MSG_SIZE; i++)
	{
		MIDI_Ring_Buffer[MIDI_Ring_PtrNext][i] = message[i];
	}
	if (++MIDI_Ring_PtrNext >= MIDI_USB_RING_SIZE)
		{ MIDI_Ring_PtrNext  = 0;}
	if (++MIDI_Ring_Queue 	>= MIDI_USB_RING_SIZE)
		{ MIDI_Ring_Queue 	 = MIDI_USB_RING_SIZE;}
	return MIDI_Ring_Queue;
}

uint16_t MIDI_Message_Ring_Dump (uint8_t *whereTo)
{
	uint16_t result = 0;
	if (!MIDI_Ring_Queue) return result;

	if (MIDI_Ring_Queue > MIDI_Ring_PtrNext)
	{
		uint16_t queue = (MIDI_Ring_Queue - MIDI_Ring_PtrNext);
		result = queue*MIDI_USB_MSG_SIZE;
		memcpy(whereTo, &(MIDI_Ring_Buffer[queue][0]), result);
		whereTo += result;
	}

	if (MIDI_Ring_PtrNext != 0)
	{
		uint16_t delta =  MIDI_Ring_PtrNext * MIDI_USB_MSG_SIZE;
		memcpy(whereTo, &(MIDI_Ring_Buffer[0][0]), delta);
		result += delta;
	}
	MIDI_Ring_PtrNext = 0;
	MIDI_Ring_Queue = 0;
	return result;

}


void USART1_IRQHandler(void)
	{ MIDI_UART_IRQHandler(&huart1); 		}
void USART2_IRQHandler(void)
	{ MIDI_UART_IRQHandler(&huart2); 		}
void USART3_IRQHandler(void)
	{ MIDI_UART_IRQHandler(&huart3); 		}


void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 31250;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 31250;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 31250;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
}


