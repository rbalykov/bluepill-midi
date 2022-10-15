
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_midi.h"

#include "stm32f1xx_hal_pcd_ex.h"

USBD_HandleTypeDef 		hUsbDeviceFS;
USBD_MIDI_ItfTypeDef 	hUsbClassMIDI_CB;

void MX_USB_DEVICE_Init(void)
{
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
  USBD_RegisterClass(&hUsbDeviceFS, &hUsbClassMIDI);
  USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &hUsbClassMIDI_CB);
  USBD_Start(&hUsbDeviceFS);
}
