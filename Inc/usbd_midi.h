
#ifndef __USB_MIDI_H
#define __USB_MIDI_H

#include <stdint.h>
#include "usbd_def.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define USB_MIDI_CONFIG_DESC_SIZE	(165u)
#define MIDI_OUT_PACKET_SIZE		(0x40)
#define MIDI_IN_PACKET_SIZE			MIDI_OUT_PACKET_SIZE
#define USB_MIDI_BUFFER_MAX_SIZE	MIDI_OUT_PACKET_SIZE

#define USB_EP_IN_ADDR_1	(0x81)
#define USB_EP_OUT_ADDR_1	(0x01)

typedef struct
{
	uint8_t cmd;
	uint8_t data[USB_MAX_EP0_SIZE];
	uint8_t len;
	uint8_t unit;
}USBD_MIDI_ControlTypeDef;

typedef struct
{
	uint8_t                   	buffer[USB_MIDI_BUFFER_MAX_SIZE];
	USBD_MIDI_ControlTypeDef 	control;
}USBD_MIDI_HandleTypeDef;

typedef struct
{
	int8_t  (*Init)         (void);
	int8_t  (*DeInit)       (void);
}USBD_MIDI_ItfTypeDef;

uint8_t  USBD_MIDI_RegisterInterface (USBD_HandleTypeDef   *pdev,
								USBD_MIDI_ItfTypeDef *fops);

extern USBD_MIDI_ItfTypeDef 	hUsbClassMidi_CB;
extern USBD_HandleTypeDef 		hUsbDeviceFS;
extern USBD_MIDI_HandleTypeDef 	hUsbClassHandleMidi;
extern USBD_ClassTypeDef  		hUsbClassMIDI;

#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_H */
