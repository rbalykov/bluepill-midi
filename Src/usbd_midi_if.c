#include "usbd_midi.h"
#include "usdb_midi_if.h"
#include "stm32f1xx_hal.h"
#include "queue32.h"
#include "main.h"
#include "midi_uart.h"

uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);

USBD_MIDI_ItfTypeDef hUsbClassMIDI_CB =
{
	MIDI_DataRx
};

uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length)
{
	uint16_t cnt;
	uint16_t msgs = length / 4;
	uint16_t chk = length % 4;
	if(chk == 0)
	{
		for(cnt = 0;cnt < msgs;cnt ++)
		{
			b4arrq_push(&qDataOut,((uint32_t *)msg)+cnt);
		}
	}
	return 0;
}





