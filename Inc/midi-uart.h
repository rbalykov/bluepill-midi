#ifndef __MIDI_UART_H
#define __MIDI_UART_H

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);


#define MIDI_USB_MSG_SIZE		(4)
#define MIDI_USB_RING_SIZE		(16)

uint16_t	MIDI_Message_Ring_Push 	(uint8_t *message);
uint16_t 	MIDI_Message_Ring_Dump 	(uint8_t *whereTo);

#define MIDI_UART_PORT_COUNT	(1)
#define MIDI_UART_RING_SIZE		(16)


enum {
	MIDI_UART_A = 0,
	MIDI_UART_B = 1,
	MIDI_UART_C = 2
} MidiPort_t;

uint8_t	MIDI_TX_Push 	(uint8_t port, uint8_t *byte);
uint8_t	MIDI_TX_Pop 	(uint8_t port, uint8_t *byte);
void 	MIDI_UART_RingInit		(void);


#define MIDI_MASK_EVENT			(0xF0)
#define MIDI_MASK_CHANNEL		(0x0F)
#define MIDI_MASK_TRIM			(0x7F)

#define MIDI_EVENT_NOTE_OFF		(0x80)
#define MIDI_EVENT_NOTE_ON		(0x90)
#define MIDI_EVENT_TOUCH_SINGLE	(0xA0)
#define MIDI_EVENT_CONTROL		(0xB0)
#define MIDI_EVENT_PROGRAM		(0xC0)
#define MIDI_EVENT_TOUCH_GROUP	(0xD0)
#define MIDI_EVENT_PITCH_BEND	(0xE0)
#define MIDI_EVENT_SYSEX		(0xF0)

#define MIDI_PREAMBLE_RSHIFT		(4)
#define MIDI_PREAMBLE(byte) 				((uint8_t)((byte & MIDI_MASK_EVENT) >> MIDI_PREAMBLE_RSHIFT))
#define MIDI_EVENT_HEADER(event,channel)	((uint8_t)((event & MIDI_MASK_EVENT) | (channel & MIDI_MASK_CHANNEL)))
#define MIDI_TRIM(byte)						((uint8_t)(byte &  MIDI_MASK_TRIM))

#define MIDI_SYSEX_MASK			(0x0F)
#define MIDI_SYSEX_MSG_START	(0x00)
#define MIDI_SYSEX_TC			(0x01)
#define MIDI_SYSEX_SONG_POS 	(0x02)
#define MIDI_SYSEX_SONG_SELECT	(0x03)
#define MIDI_SYSEX_TUNE			(0x06)
#define MIDI_SYSEX_MSG_STOP		(0x07)

#define MIDI_SYSEX_BEAT_CLOCK	(0x08)
#define MIDI_SYSEX_PLAY_START	(0x0A)
#define MIDI_SYSEX_PLAY_CONT	(0x0B)
#define MIDI_SYSEX_PLAY_STOP	(0x0C)
#define MIDI_SYSEX_ALIVE		(0x0E)
#define MIDI_SYSEX_RESET		(0x0F)



#ifdef __cplusplus
}
#endif

#endif
