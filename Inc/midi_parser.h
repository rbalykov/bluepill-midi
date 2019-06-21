
#ifndef __MIDI_PARSER_H_FILE__
#define __MIDI_PARSER_H_FILE__

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#define MIDI_UART_PORT_COUNT	(3)
#define MIDI_UART_RXBUFFER_SIZE	(1)
#define MIDI_MSG_MAX_SIZE		(2)
#define MIDI_MSG_SIZE_2BYTES	(2)
#define MIDI_MSG_SIZE_1BYTE		(1)
#define MIDI_MSG_SIZE_NULL		(0)

typedef struct _MIDI_UART_Parser_TypeDef
{
	uint8_t  data[MIDI_MSG_MAX_SIZE];
	uint8_t  rxbuffer[MIDI_UART_RXBUFFER_SIZE];
	uint16_t length;

	uint8_t  cable_id;
	uint8_t  command;
}MIDI_UART_Parser_TypeDef;

void	MIDI_UART_Parser_Init 		(void);
void 	MIDI_UART_Parser_Handle		(MIDI_UART_Parser_TypeDef *handler, uint8_t byte);


#ifdef __cplusplus
}
#endif

#endif
