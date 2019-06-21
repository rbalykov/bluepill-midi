
/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

#define USBD_LANGID_STRING     			1033
#define USBD_MANUFACTURER_STRING    	"Neutrino"
#define USBD_PRODUCT_STRING_FS     		"Neutrino USB-MIDI"
#define USBD_CONFIGURATION_STRING_FS    "USB-MIDI 3x3 adapter"
#define USBD_INTERFACE_STRING_FS		"Jack i/o"

#define USBD_STR_4_INTERFACE	"USB-MIDI 3in 3out"
#define USBD_STR_5_INPUT1		"Input _A_"
#define USBD_STR_6_OUTPUT1		"Output _A_"
#define USBD_STR_7_UNPUT2		"Input _B_"
#define USBD_STR_8_OUTPUT2		"Output _B_"
#define USBD_STR_9_INPUT3		"Input _C_"
#define USBD_STR_A_OUTPUT3		"Output _C_"
#define USBD_STR_UNKNOWN		"----------"

uint8_t USBD_SERIALNUMBER_STRING_FS[]   =  "000_000_000_000_000_000_0";


uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

/* Internal string descriptor. */
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

USBD_DescriptorsTypeDef FS_Desc =
{
  USBD_FS_DeviceDescriptor				// +
, USBD_FS_LangIDStrDescriptor			// +
, USBD_FS_ManufacturerStrDescriptor		// +
, USBD_FS_ProductStrDescriptor			// +
, USBD_FS_SerialStrDescriptor			// +
, USBD_FS_ConfigStrDescriptor			// +
, USBD_FS_InterfaceStrDescriptor		// +
};

uint8_t  * USBD_GetUserDectriptor (struct _USBD_HandleTypeDef *pdev ,uint8_t index,  uint16_t *length)
{
	switch (index)
	{
	case 4:
		USBD_GetString((uint8_t *) USBD_STR_4_INTERFACE, 	USBD_StrDesc, length);
		break;
	case 5:
		USBD_GetString((uint8_t *) USBD_STR_5_INPUT1, 		USBD_StrDesc, length);
		break;
	case 6:
		USBD_GetString((uint8_t *) USBD_STR_6_OUTPUT1, 		USBD_StrDesc, length);
		break;
	case 7:
		USBD_GetString((uint8_t *) USBD_STR_7_UNPUT2, 		USBD_StrDesc, length);
		break;
	case 8:
		USBD_GetString((uint8_t *) USBD_STR_8_OUTPUT2, 		USBD_StrDesc, length);
		break;
	case 9:
		USBD_GetString((uint8_t *) USBD_STR_9_INPUT3, 		USBD_StrDesc, length);
		break;
	case 10:
		USBD_GetString((uint8_t *) USBD_STR_A_OUTPUT3, 		USBD_StrDesc, length);
		break;
	default:
		USBD_GetString((uint8_t *) USBD_STR_UNKNOWN, 		USBD_StrDesc, length);
		break;
	}
	return USBD_StrDesc;
}

/** USB standard device descriptor. */
__ALIGN_BEGIN uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
		{
		/* Descriptor Length                       */ 0x12u,
		/* DescriptorType: DEVICE                  */ USB_DESC_TYPE_DEVICE,
		/* bcdUSB (ver 2.0)                        */ 0x00u, 0x02u,
		/* bDeviceClass                            */ 0x00u,
		/* bDeviceSubClass                         */ 0x00u,
		/* bDeviceProtocol                         */ 0x00u,
		/* bMaxPacketSize0                         */ USB_MAX_EP0_SIZE,
		/* idVendor                                */ 0xB4u, 0x04u,
		/* idProduct                               */ 0x32u, 0xD2u,
		/* bcdDevice                               */ 0x01u, 0x00u,
		/* iManufacturer                           */ USBD_IDX_MFC_STR,
		/* iProduct                                */ USBD_IDX_PRODUCT_STR,
		/* iSerialNumber                           */ USBD_IDX_SERIAL_STR,
		/* bNumConfigurations                      */ USBD_MAX_NUM_CONFIGURATION
		};

__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
     USB_LEN_LANGID_STR_DESC,
     USB_DESC_TYPE_STRING,
     LOBYTE(USBD_LANGID_STRING),
     HIBYTE(USBD_LANGID_STRING)
};

uint8_t * USBD_FS_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	*length = sizeof(USBD_FS_DeviceDesc);
	return USBD_FS_DeviceDesc;
}

uint8_t * USBD_FS_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	*length = sizeof(USBD_LangIDDesc);
	return USBD_LangIDDesc;
}

uint8_t * USBD_FS_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	USBD_GetString((uint8_t *) USBD_PRODUCT_STRING_FS, USBD_StrDesc, length);
	return USBD_StrDesc;
}

uint8_t * USBD_FS_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	USBD_GetString((uint8_t *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
	return USBD_StrDesc;
}

uint8_t hex2char (uint8_t ch)
{
	ch &= 0x0F;
	if (ch < 0xA) return '0'+ch;
	ch -= 0xA;
	return 'A'+ch;
}

uint8_t * USBD_FS_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	uint8_t unique_id[12];
	HAL_GetUID((uint32_t *)unique_id);
	for (uint8_t i=0; i<12; i++)
	{
		USBD_SERIALNUMBER_STRING_FS[i*2]   = hex2char(unique_id[i] >> 4);
		USBD_SERIALNUMBER_STRING_FS[i*2+1] = hex2char(unique_id[i] & 0x0F);
	}

	USBD_GetString((uint8_t *) USBD_SERIALNUMBER_STRING_FS, USBD_StrDesc,
			length);
	return USBD_StrDesc;
}

uint8_t * USBD_FS_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	USBD_GetString((uint8_t *) USBD_CONFIGURATION_STRING_FS, USBD_StrDesc,
			length);
	return USBD_StrDesc;
}

uint8_t * USBD_FS_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	USBD_GetString((uint8_t *) USBD_INTERFACE_STRING_FS, USBD_StrDesc, length);
	return USBD_StrDesc;
}

