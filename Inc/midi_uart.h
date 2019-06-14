#ifndef __MIDI_UART_H
#define __MIDI_UART_H

#ifdef __cplusplus
extern "C" {
#endif


// -----------------------------------------------------------------------------
// MIDI 32-bit message, byte 1: cable_id + code_index
// -----------------------------------------------------------------------------
#define MIDI_CABLE_0			(0x00)
#define MIDI_CABLE_1			(0x01)
#define MIDI_CABLE_2			(0x02)
#define MIDI_CABLE_3			(0x03)
#define MIDI_CABLE_4			(0x04)
#define MIDI_CABLE_5			(0x05)
#define MIDI_CABLE_6			(0x06)
#define MIDI_CABLE_7			(0x07)
#define MIDI_CABLE_8			(0x08)
#define MIDI_CABLE_9			(0x09)
#define MIDI_CABLE_A			(0x0A)
#define MIDI_CABLE_B			(0x0B)
#define MIDI_CABLE_C			(0x0C)
#define MIDI_CABLE_D			(0x0D)
#define MIDI_CABLE_E			(0x0E)
#define MIDI_CABLE_F			(0x0F)

#define MIDI_CIN_RESERVE_0		(0x00)
#define MIDI_CIN_RESERVE_1		(0x01)
#define MIDI_CIN_SYS_2BYTE		(0x02)
#define MIDI_CIN_SYS_3BYTE		(0x03)
#define MIDI_CIN_SYSEX_RUN		(0x04)
#define MIDI_CIN_SYSEX_1BYTE	(0x05)
#define MIDI_CIN_SYSEX_2BYTE	(0x06)
#define MIDI_CIN_SYSEX_3BYTE	(0x07)
#define MIDI_CIN_NOTE_OFF		(0x08)
#define MIDI_CIN_NOTE_ON		(0x09)
#define MIDI_CIN_NOTE_POLY		(0x0A)
#define MIDI_CIN_CONTROL		(0x0B)
#define MIDI_CIN_PROGRAM		(0x0C)
#define MIDI_CIN_PRESSURE		(0x0D)
#define MIDI_CIN_PITCHBEND		(0x0E)
#define MIDI_CIN_SINGLEBYTE		(0x0F)

#define MIDI_USB_PREAMBLE_SHIFT4	(4)
#define MIDI_USB_MASK_LOW4			(0x0F)

#define MIDI_USB_PREAMBLE(cable,index) 			((uint8_t)( ((cable & MIDI_USB_MASK_LOW4)<<MIDI_USB_PREAMBLE_SHIFT4) | (index & MIDI_USB_MASK_LOW4)))


// -----------------------------------------------------------------------------
// MIDI 32-bit message, byte 2: event + channel
// -----------------------------------------------------------------------------
#define MIDI_EVENT_NOTE_OFF		(0x80)
#define MIDI_EVENT_NOTE_ON		(0x90)
#define MIDI_EVENT_TOUCH_SINGLE	(0xA0)
#define MIDI_EVENT_CONTROL		(0xB0)
#define MIDI_EVENT_PROGRAM		(0xC0)
#define MIDI_EVENT_TOUCH_GROUP	(0xD0)
#define MIDI_EVENT_PITCH_BEND	(0xE0)
#define MIDI_EVENT_SYSEX		(0xF0)

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

#define MIDI_MASK_EVENT			(0xF0)
#define MIDI_MASK_CHANNEL		MIDI_USB_MASK_LOW4
#define MIDI_EVENT_HEADER(event,channel)	((uint8_t)((event & MIDI_MASK_EVENT) | (channel & MIDI_MASK_CHANNEL)))

// -----------------------------------------------------------------------------
// MIDI 32-bit message, byte 3+4 go with high bit off
// -----------------------------------------------------------------------------
#define MIDI_MASK_TRIM			(0x7F)
#define MIDI_TRIM(byte)			((uint8_t)(byte &  MIDI_MASK_TRIM))


// -----------------------------------------------------------------------------
// UART handling
// -----------------------------------------------------------------------------

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void HAL_UART_MspInit(UART_HandleTypeDef* huart);

// -----------------------------------------------------------------------------
// MIDI USB Message Ring Buffer
// -----------------------------------------------------------------------------
#define MIDI_USB_MSG_SIZE		(4)
#define MIDI_USB_RING_SIZE		(16)

uint8_t		MIDI_DataIn_NonEmpty 	(void);
uint16_t	MIDI_DataIn_Push 	(uint8_t *message);
uint16_t 	MIDI_DataIn_Dump 	(uint8_t *whereTo);


#define MIDI_UART_PORT_COUNT	(3)
#define MIDI_UART_RING_SIZE		(16)


enum {
	MIDI_UART_A = 0,
	MIDI_UART_B = 1,
	MIDI_UART_C = 2
} MidiPort_t;

uint8_t	MIDI_TX_Push 	(uint8_t port, uint8_t *byte);
uint8_t	MIDI_TX_Pop 	(uint8_t port, uint8_t *byte);
void 	MIDI_UART_Init		(void);

extern void sendNoteOn(uint8_t ch, uint8_t note, uint8_t vel);
extern void sendNoteOff(uint8_t ch, uint8_t note);
extern void sendCtlChange(uint8_t ch, uint8_t num, uint8_t value);


#ifdef __cplusplus
}
#endif

#endif
