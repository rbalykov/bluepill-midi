/*
 * midi-uart.c
 *
 *  Created on: 12 июн. 2019 г.
 *      Author: pi
 */

#include "main.h"
#include <string.h>
#include "midi_uart.h"
#include "queue32.h"
#include "usbd_midi.h"
#include "usbd_core.h"

stB4Arrq qDataOut;
stB4Arrq qDataIn;

static uint8_t MIDI_Ring_Buffer[MIDI_USB_RING_SIZE]		[MIDI_USB_MSG_SIZE] = {0};
static uint16_t 	MIDI_Ring_PtrNext = 0,
					MIDI_Ring_Queue	  = 0;

extern USBD_HandleTypeDef *pInstance;

// UART Hardware
void MIDI_UART_IRQHandler(UART_HandleTypeDef *huart);
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

uint16_t MIDI_Message_Ring_Push (uint8_t * message);
uint16_t MIDI_Message_Ring_Dump (uint8_t *whereTo);
uint16_t MIDI_Message_Ring_Queue 	(void);

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

uint8_t	MIDI_DataIn_NonEmpty ()
{
	return (uint8_t) (qDataIn.num != 0);
//	return MIDI_Ring_Queue;
}

inline uint16_t	MIDI_DataIn_Push 	(uint8_t *message)
{
	return b4arrq_push(&qDataIn, (uint32_t *)message);
//	return MIDI_Message_Ring_Push (message);
//	USBD_LL_Transmit (pInstance, USB_EP_IN_ADDR_1, message, 4); return 1;
}

uint16_t MIDI_DataIn_Dump 	(uint8_t *where_to)
{
//	return MIDI_Message_Ring_Dump (where_to);

	uint16_t delta = 0;
	uint32_t *pop = 0;

	while (MIDI_IN_PACKET_SIZE - delta >= MIDI_USB_MSG_SIZE)
	{
		pop = b4arrq_pop (&qDataIn);
		if (!pop)
		{
			return delta;
		}
		memcpy (where_to + delta, pop, MIDI_USB_MSG_SIZE);
		delta += MIDI_USB_MSG_SIZE;
	}
	return delta;

}

uint16_t MIDI_Message_Ring_Queue 	(void)
{
	return MIDI_Ring_Queue;
}

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

void 	MIDI_UART_Init		(void)
{
	b4arrq_init(&qDataIn);
	b4arrq_init(&qDataOut);

	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	HAL_UART_MspInit(&huart1);
	HAL_UART_MspInit(&huart2);
	HAL_UART_MspInit(&huart3);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void sendNoteOn(uint8_t ch, uint8_t note, uint8_t vel)
{
	uint8_t message[MIDI_USB_MSG_SIZE];
	message[0] = MIDI_USB_PREAMBLE	(MIDI_CABLE_0, MIDI_CIN_NOTE_ON);
	message[1] = MIDI_EVENT_HEADER	(MIDI_EVENT_NOTE_ON,ch);
	message[2] = MIDI_TRIM			(note);
	message[3] = MIDI_TRIM			(vel);
	MIDI_DataIn_Push (message);
}

void sendNoteOff(uint8_t ch, uint8_t note)
{
	uint8_t message[MIDI_USB_MSG_SIZE];
	message[0] = MIDI_USB_PREAMBLE	(MIDI_CABLE_0, MIDI_CIN_NOTE_OFF);
	message[1] = MIDI_EVENT_HEADER	(MIDI_EVENT_NOTE_OFF, ch);
	message[2] = MIDI_TRIM			(note);
	message[3] = 0;
	MIDI_DataIn_Push (message);
}

void sendCtlChange(uint8_t ch, uint8_t ctl, uint8_t value)
{
	uint8_t message[MIDI_USB_MSG_SIZE];
	message[0] = MIDI_USB_PREAMBLE	(MIDI_CABLE_0, MIDI_CIN_CONTROL);
	message[1] = MIDI_EVENT_HEADER	(MIDI_EVENT_CONTROL, ch);
	message[2] = MIDI_TRIM			(ctl);
	message[3] = MIDI_TRIM			(value);
	MIDI_DataIn_Push (message);
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


