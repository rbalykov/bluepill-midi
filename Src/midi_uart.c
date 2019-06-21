/*
 * midi-uart.c
 *
 *  Created on: 12 июн. 2019 г.
 *      Author: pi
 */

#include "main.h"
#include <string.h>
#include "usbd_midi.h"
#include "usbd_core.h"

#include "midi_uart.h"

//extern USBD_HandleTypeDef *pInstance;

// UART Hardware
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

void MIDI_Handle_USB_DataOut (uint8_t *msg, uint16_t length)
{
	if ((length % MIDI_USB_MSG_SIZE) != 0) return;
	for (uint16_t offset=0; offset<length; offset += MIDI_USB_MSG_SIZE)
		MIDI_USB_Queue_Push (&MIDI_USB_Queue_DataOut, msg+offset);
}

MIDI_UART_Queue_TypeDef* MIDI_UART_Guess_TxQueue (UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) return &MIDI_UART_Queue_Tx1;
	if (huart->Instance == USART2) return &MIDI_UART_Queue_Tx2;
	if (huart->Instance == USART3) return &MIDI_UART_Queue_Tx3;
	return NULL;
}

MIDI_UART_RxHandler_TypeDef* MIDI_UART_Guess_RxHandler (UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) return &MIDI_UART_Rx_1;
	if (huart->Instance == USART2) return &MIDI_UART_Rx_2;
	if (huart->Instance == USART3) return &MIDI_UART_Rx_3;
	return NULL;
}

uint8_t MIDI_UART_Guess_CableId (UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) return MIDI_CABLE_0;
	if (huart->Instance == USART2) return MIDI_CABLE_1;
	if (huart->Instance == USART3) return MIDI_CABLE_2;
	return MIDI_CABLE_0;
}

HAL_StatusTypeDef MIDI_UART_Receive_IT (UART_HandleTypeDef *huart)
{
	return HAL_UART_Receive_IT(huart,
			MIDI_UART_Guess_RxHandler(huart)->rxbuffer, MIDI_UART_ONE_BYTE);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	MIDI_UART_Guess_RxHandler(huart)->length = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	MIDI_UART_RxHandler_TypeDef *handler = MIDI_UART_Guess_RxHandler(huart);
	handler->cable_id = MIDI_UART_Guess_CableId(huart);
	uint8_t message[MIDI_USB_MSG_SIZE] = {0};

	message[0] = MIDI_USB_PREAMBLE	(handler->cable_id , MIDI_CIN_SINGLEBYTE);
	message[1] = handler->rxbuffer[0];

	MIDI_USB_Queue_Push(&MIDI_USB_Queue_DataIn,  message);
	HAL_UART_Receive_IT(huart, handler->rxbuffer, MIDI_UART_ONE_BYTE);
}


void MIDI_UART_TxQueue_Transmit (void)
{
	uint8_t byte_1, byte_2, byte_3, dummy;

	if (MIDI_UART_Queue_Next (&MIDI_UART_Queue_Tx1, &byte_1))
	{
		if (HAL_UART_Transmit_IT(&huart1, &byte_1, MIDI_UART_ONE_BYTE) == HAL_OK)
			MIDI_UART_Queue_Pop (&MIDI_UART_Queue_Tx1, &dummy);
	}
	if (MIDI_UART_Queue_Next (&MIDI_UART_Queue_Tx2, &byte_2))
	{
		if (HAL_UART_Transmit_IT(&huart2, &byte_2, MIDI_UART_ONE_BYTE) == HAL_OK)
			MIDI_UART_Queue_Pop (&MIDI_UART_Queue_Tx2, &dummy);
	}
	if (MIDI_UART_Queue_Next (&MIDI_UART_Queue_Tx3, &byte_3))
	{
		if (HAL_UART_Transmit_IT(&huart3, &byte_3, MIDI_UART_ONE_BYTE) == HAL_OK)
			MIDI_UART_Queue_Pop (&MIDI_UART_Queue_Tx3, &dummy);
	}
}


void 	MIDI_UART_Dispatch 			(void)
{
	if (MIDI_USB_Queue_Empty(&MIDI_USB_Queue_DataOut)) return;

	uint8_t message[MIDI_USB_MSG_SIZE];
	MIDI_USB_Queue_Pop(&MIDI_USB_Queue_DataOut, message);
	MIDI_UART_Dispatch_Msg(message);
}

void MIDI_UART_Dispatch_Msg (uint8_t *message)
{
	MIDI_UART_Queue_TypeDef * pQueue = NULL;
	uint8_t cable_id 	= message[0] & MIDI_USB_MASK_HIGH4;
	uint8_t code_index 	= message[0] & MIDI_USB_MASK_LOW4;

	switch (cable_id)
	{
		case MIDI_CABLE_0: pQueue = &MIDI_UART_Queue_Tx1; break;
		case MIDI_CABLE_1: pQueue = &MIDI_UART_Queue_Tx2; break;
		case MIDI_CABLE_2: pQueue = &MIDI_UART_Queue_Tx3; break;
		default: break;
	}
	if (pQueue)
	{
		uint8_t lenght = MIDI_Guess_USB_Msg_Length (code_index);
		for (uint8_t i = 1; i <= lenght; i++)
		{
			MIDI_UART_Queue_Push (pQueue, &message[i]);
		}
	}
}

uint8_t MIDI_Guess_UART_Msg_Length (uint8_t command)
{
	uint8_t event = command & MIDI_MASK_EVENT;
	switch (event)
	{
		case MIDI_EVENT_NOTE_OFF:
		case MIDI_EVENT_NOTE_ON:
		case MIDI_EVENT_TOUCH_SINGLE:
		case MIDI_EVENT_CONTROL:
		case MIDI_EVENT_PITCH_BEND:
			return 3; break;
		case MIDI_EVENT_PROGRAM:
		case MIDI_EVENT_TOUCH_GROUP:
			return 2; break;
	}
	uint8_t sysex = command & MIDI_MASK_SYSEX;
	switch (sysex)
	{
		case MIDI_SYSEX_MSG_START:
		case MIDI_SYSEX_SONG_POS:
			return 3; break;
		case MIDI_SYSEX_TC:
		case MIDI_SYSEX_SONG_SELECT:
			return 2; break;
		case MIDI_SYSEX_TUNE:
		case MIDI_SYSEX_MSG_STOP:
		case MIDI_SYSEX_BEAT_CLOCK:
		case MIDI_SYSEX_PLAY_START:
		case MIDI_SYSEX_PLAY_CONT:
		case MIDI_SYSEX_PLAY_STOP:
		case MIDI_SYSEX_ALIVE:
		case MIDI_SYSEX_RESET:
			return 1; break;
	}
	return 0;
}

uint8_t MIDI_Guess_USB_Msg_Length (uint8_t code_index)
{
	switch (code_index)
	{
		case MIDI_CIN_NOTE_OFF:
		case MIDI_CIN_NOTE_ON:
		case MIDI_CIN_NOTE_POLY:
		case MIDI_CIN_CONTROL:
		case MIDI_CIN_PITCHBEND:
		case MIDI_CIN_SYS_3BYTE:
		case MIDI_CIN_SYSEX_RUN:
		case MIDI_CIN_SYSEX_3BYTE:
			return 3;
		break;

		case MIDI_CIN_PROGRAM:
		case MIDI_CIN_PRESSURE:
		case MIDI_CIN_SYS_2BYTE:
		case MIDI_CIN_SYSEX_2BYTE:
			return 2;
		break;

		case MIDI_CIN_SINGLEBYTE:
		case MIDI_CIN_SYSEX_1BYTE:
			return 1;
		break;

		case MIDI_CIN_RESERVE_0:
		case MIDI_CIN_RESERVE_1:
			return 0;
		break;
	}
	return 0;
}

void 	MIDI_Init		(void)
{
	MIDI_UART_Queue_Init(&MIDI_UART_Queue_Tx1);
	MIDI_UART_Queue_Init(&MIDI_UART_Queue_Tx2);
	MIDI_UART_Queue_Init(&MIDI_UART_Queue_Tx3);
	MIDI_USB_Queue_Init (&MIDI_USB_Queue_DataIn);
	MIDI_USB_Queue_Init (&MIDI_USB_Queue_DataOut);

	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
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
	MIDI_USB_Queue_Push(&MIDI_USB_Queue_DataIn,  message);
}

void sendNoteOff(uint8_t ch, uint8_t note)
{
	uint8_t message[MIDI_USB_MSG_SIZE];
	message[0] = MIDI_USB_PREAMBLE	(MIDI_CABLE_0, MIDI_CIN_NOTE_OFF);
	message[1] = MIDI_EVENT_HEADER	(MIDI_EVENT_NOTE_OFF, ch);
	message[2] = MIDI_TRIM			(note);
	message[3] = 0;
	MIDI_USB_Queue_Push(&MIDI_USB_Queue_DataIn,  message);
}

void sendCtlChange(uint8_t ch, uint8_t ctl, uint8_t value)
{
	uint8_t message[MIDI_USB_MSG_SIZE];
	message[0] = MIDI_USB_PREAMBLE	(MIDI_CABLE_0, MIDI_CIN_CONTROL);
	message[1] = MIDI_EVENT_HEADER	(MIDI_EVENT_CONTROL, ch);
	message[2] = MIDI_TRIM			(ctl);
	message[3] = MIDI_TRIM			(value);
	MIDI_USB_Queue_Push(&MIDI_USB_Queue_DataIn,  message);
}



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

// -----------------------------------------------------------------------------
