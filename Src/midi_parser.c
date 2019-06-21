#include "midi_parser.h"
#include "midi_uart.h"
#include "main.h"

MIDI_UART_Parser_TypeDef 	MIDI_UART_Rx_1;
MIDI_UART_Parser_TypeDef 	MIDI_UART_Rx_2;
MIDI_UART_Parser_TypeDef 	MIDI_UART_Rx_3;

static void Init 		(MIDI_UART_Parser_TypeDef *handler, uint8_t cable_id);
static void MsgStart 	(MIDI_UART_Parser_TypeDef *handler, uint8_t command);
static void MsgComplete (MIDI_UART_Parser_TypeDef *handler);

static uint8_t CmdSupported 		(uint8_t command);
static uint8_t Guess_Code_Index 	(uint8_t command);
static uint8_t Guess_Msg_Length 	(uint8_t command);

// -----------------------------------------------------------------------------
// --- Exported Functions ------------------------------------------------------
// -----------------------------------------------------------------------------
void	MIDI_UART_Parser_Init (void)
{
	Init (&MIDI_UART_Rx_1, MIDI_CABLE_0);
	Init (&MIDI_UART_Rx_2, MIDI_CABLE_1);
	Init (&MIDI_UART_Rx_3, MIDI_CABLE_2);
}

void 	MIDI_UART_Parser_Handle		(MIDI_UART_Parser_TypeDef *handler, uint8_t byte)
{
	if (MIDI_BYTE_IS_COMMAND(byte))
	{
		MsgStart(handler, byte);
	}
	else
	{
		if (handler->length >= MIDI_MSG_MAX_SIZE) return;
		handler->data[handler->length] = byte;
		handler->length++;
	}

	if (Guess_Msg_Length(handler->command) == handler->length)
	{
		MsgComplete(handler);
	}
}

// -----------------------------------------------------------------------------
// --- Static Functions --------------------------------------------------------
// -----------------------------------------------------------------------------
static void Init (MIDI_UART_Parser_TypeDef *handler, uint8_t cable_id)
{
	handler->length = 0;
	memset(handler->data, 0, MIDI_MSG_MAX_SIZE);
	handler->cable_id = cable_id;
}

static void MsgStart (MIDI_UART_Parser_TypeDef *handler, uint8_t command)
{
	handler->length = 0;
	handler->command = command;
}

static void MsgComplete (MIDI_UART_Parser_TypeDef *handler)
{
	if (CmdSupported(handler->command)) // no SysEx
	{
		uint8_t message[MIDI_USB_MSG_SIZE] = {0};

		message[0] = MIDI_USB_PREAMBLE	(handler->cable_id , Guess_Code_Index(handler->command));
		message[1] = handler->command;
		message[2] = MIDI_TRIM (handler->data[0]);
		message[3] = MIDI_TRIM (handler->data[1]);

		MIDI_USB_Queue_Push(&MIDI_USB_Queue_DataIn,  message);
	}
	// don't reset command, only data
	// this allows MIDI_RUNNING mode
	handler->length = 0;
}

static uint8_t CmdSupported (uint8_t command)
{
	if ((command & MIDI_MASK_EVENT) == MIDI_EVENT_SYSEX)
	{
		uint8_t sysex = command & MIDI_MASK_SYSEX;
		if ((sysex == MIDI_SYSEX_MSG_START) || (sysex == MIDI_SYSEX_MSG_STOP)) return 0;
	}
	return 1;
}

static uint8_t Guess_Code_Index (uint8_t command)
{
	uint8_t event = command & MIDI_MASK_EVENT;
	switch (event) // event codes 0x80 to 0xE0
	{
		case MIDI_EVENT_NOTE_OFF:		return MIDI_CIN_NOTE_OFF;	// 0x80
		case MIDI_EVENT_NOTE_ON:		return MIDI_CIN_NOTE_ON;	// 0x90
		case MIDI_EVENT_TOUCH_SINGLE:	return MIDI_CIN_NOTE_POLY;	// 0xA0
		case MIDI_EVENT_CONTROL:		return MIDI_CIN_CONTROL;	// 0xB0
		case MIDI_EVENT_PROGRAM:		return MIDI_CIN_PROGRAM;	// 0xC0
		case MIDI_EVENT_TOUCH_GROUP:	return MIDI_CIN_PRESSURE;	// 0xD0
		case MIDI_EVENT_PITCH_BEND:		return MIDI_CIN_PITCHBEND;	// 0xE0
	}
	// event code 0xF0
	uint8_t sysex = command & MIDI_MASK_SYSEX;
	switch (sysex)  // commands 0x00 to 0x0F
	{
		case MIDI_SYSEX_TC:
		case MIDI_SYSEX_SONG_SELECT:
			return MIDI_CIN_SYS_2BYTE;
		case MIDI_SYSEX_SONG_POS:
			return MIDI_CIN_SYS_3BYTE;

		case MIDI_SYSEX_TUNE:
		case MIDI_SYSEX_BEAT_CLOCK:
		case MIDI_SYSEX_PLAY_START:
		case MIDI_SYSEX_PLAY_CONT:
		case MIDI_SYSEX_PLAY_STOP:
		case MIDI_SYSEX_ALIVE:
		case MIDI_SYSEX_RESET:
			return MIDI_CIN_SYSEX_1BYTE;
	}
	return MIDI_CIN_SINGLEBYTE;
}

static uint8_t Guess_Msg_Length (uint8_t command)
{
	uint8_t event = command & MIDI_MASK_EVENT;
	switch (event) // event codes 0x80 to 0xE0
	{
		case MIDI_EVENT_NOTE_OFF:
		case MIDI_EVENT_NOTE_ON:
		case MIDI_EVENT_TOUCH_SINGLE:
		case MIDI_EVENT_CONTROL:
		case MIDI_EVENT_PITCH_BEND:
			return MIDI_MSG_SIZE_2BYTES; break;
		case MIDI_EVENT_PROGRAM:
		case MIDI_EVENT_TOUCH_GROUP:
			return MIDI_MSG_SIZE_1BYTE; break;
	}
	// event code 0xF0
	uint8_t sysex = command & MIDI_MASK_SYSEX;
	switch (sysex)  // commands 0x00 to 0x0F
	{
		case MIDI_SYSEX_MSG_START:
		case MIDI_SYSEX_SONG_POS:
			return MIDI_MSG_SIZE_2BYTES; break;
		case MIDI_SYSEX_TC:
		case MIDI_SYSEX_SONG_SELECT:
			return MIDI_MSG_SIZE_1BYTE; break;
		case MIDI_SYSEX_TUNE:
		case MIDI_SYSEX_MSG_STOP:
		case MIDI_SYSEX_BEAT_CLOCK:
		case MIDI_SYSEX_PLAY_START:
		case MIDI_SYSEX_PLAY_CONT:
		case MIDI_SYSEX_PLAY_STOP:
		case MIDI_SYSEX_ALIVE:
		case MIDI_SYSEX_RESET:
			return MIDI_MSG_SIZE_NULL; break;
	}
	return MIDI_MSG_SIZE_2BYTES;
}
