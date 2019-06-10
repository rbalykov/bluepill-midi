
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_midi.h"


USBD_HandleTypeDef 		hUsbDeviceFS;
USBD_ClassTypeDef  		hUsbClassMIDI;
USBD_MIDI_HandleTypeDef hUsbClassHandleMidi;
USBD_MIDI_ItfTypeDef 	hUsbClassMidi_CB;

void MX_USB_DEVICE_Init(void)
{
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);		// usbd_desc:30
  USBD_RegisterClass(&hUsbDeviceFS, &hUsbClassMIDI);	// usbd_midi:44
  USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &hUsbClassMidi_CB); // usbd_midi:28
  USBD_Start(&hUsbDeviceFS);
}
