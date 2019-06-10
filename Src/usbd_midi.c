
#include "usbd_midi.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

#include "usbd_def.h"
#include "usbd_core.h"

static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length);
static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length);
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev);
static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev);
static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev);
static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

/*
static void MIDI_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void MIDI_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
*/

static int8_t MIDI_Init_FS   (void);
static int8_t MIDI_DeInit_FS (void);

USBD_MIDI_ItfTypeDef hUsbClassMidi_CB =
{
  MIDI_Init_FS,
  MIDI_DeInit_FS,
};

static int8_t MIDI_Init_FS   (void)
{
  return (USBD_OK);
}

static int8_t MIDI_DeInit_FS (void)
{
  return (USBD_OK);
}

USBD_ClassTypeDef  hUsbClassMIDI =
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  USBD_MIDI_EP0_TxReady,  
  USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  USBD_MIDI_SOF,
  USBD_MIDI_IsoINIncomplete,
  USBD_MIDI_IsoOutIncomplete,      
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetCfgDesc, 
  USBD_MIDI_GetCfgDesc,
  USBD_MIDI_GetDeviceQualifierDesc,
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_CfgDesc[USB_MIDI_CONFIG_DESC_SIZE] __ALIGN_END =
//		const uint8 CYCODE USBMIDI_1_DEVICE0_CONFIGURATION0_DESCR[165u] =
{
	/*  Config Descriptor Length               */ 0x09u,
	/*  DescriptorType: CONFIG                 */ 0x02u,
	/*  wTotalLength                           */ 0xA5u, 0x00u,
	/*  bNumInterfaces                         */ 0x02u,
	/*  bConfigurationValue                    */ 0x01u,
	/*  iConfiguration                         */ 0x01u,
	/*  bmAttributes                           */ 0x80u,
	/*  bMaxPower                              */ 0x32u,
	/*********************************************************************
	* AudioControl Interface Descriptor
	*********************************************************************/
	/*  Interface Descriptor Length            */ 0x09u,
	/*  DescriptorType: INTERFACE              */ 0x04u,
	/*  bInterfaceNumber                       */ 0x00u,
	/*  bAlternateSetting                      */ 0x00u,
	/*  bNumEndpoints                          */ 0x00u,
	/*  bInterfaceClass                        */ 0x01u,
	/*  bInterfaceSubClass                     */ 0x01u,
	/*  bInterfaceProtocol                     */ 0x00u,
	/*  iInterface                             */ 0x03u,
	/*********************************************************************
	* AC Header Descriptor
	*********************************************************************/
	/*  AC Header Descriptor Length            */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x01u,
	/*  bcdADC                                 */ 0x00u, 0x01u,
	/*  wTotalLength                           */ 0x09u, 0x00u,
	/*  bInCollection                          */ 0x01u,
	/*  baInterfaceNr                          */ 0x01u,
	/*********************************************************************
	* MIDIStreaming Interface Descriptor
	*********************************************************************/
	/*  Interface Descriptor Length            */ 0x09u,
	/*  DescriptorType: INTERFACE              */ 0x04u,
	/*  bInterfaceNumber                       */ 0x01u,
	/*  bAlternateSetting                      */ 0x00u,
	/*  bNumEndpoints                          */ 0x02u,
	/*  bInterfaceClass                        */ 0x01u,
	/*  bInterfaceSubClass                     */ 0x03u,
	/*  bInterfaceProtocol                     */ 0x00u,
	/*  iInterface                             */ 0x04u,
	/*********************************************************************
	* MS Header Descriptor
	*********************************************************************/
	/*  MS Header Descriptor Length            */ 0x07u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x01u,
	/*  bcdADC                                 */ 0x00u, 0x01u,
	/*  wTotalLength                           */ 0x61u, 0x00u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x01u,
	/*  iJack                                  */ 0x05u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x02u,
	/*  iJack                                  */ 0x05u,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x03u,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x02u,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x06u,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x04u,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x01u,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x06u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x05u,
	/*  iJack                                  */ 0x07u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x06u,
	/*  iJack                                  */ 0x07u,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x07u,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x06u,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x08u,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x08u,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x05u,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x08u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x09u,
	/*  iJack                                  */ 0x09u,
	/*********************************************************************
	* MIDI IN Jack Descriptor
	*********************************************************************/
	/*  MIDI IN Jack Descriptor Length         */ 0x06u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x02u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x0Au,
	/*  iJack                                  */ 0x09u,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x01u,
	/*  bJackID                                */ 0x0Bu,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x0Au,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x0Au,
	/*********************************************************************
	* MIDI OUT Jack Descriptor
	*********************************************************************/
	/*  MIDI OUT Jack Descriptor Length        */ 0x09u,
	/*  DescriptorType: MIDI                  */ 0x24u,
	/*  bDescriptorSubtype                     */ 0x03u,
	/*  bJackType                              */ 0x02u,
	/*  bJackID                                */ 0x0Cu,
	/*  bNrInputPins                           */ 0x01u,
	/*  baSourceID                             */ 0x09u,
	/*  baSourceID                             */ 0x01u,
	/*  iJack                                  */ 0x0Au,
	/*********************************************************************
	* Endpoint Descriptor
	*********************************************************************/
	/*  Endpoint Descriptor Length             */ 0x09u,
	/*  DescriptorType: ENDPOINT               */ 0x05u,
	/*  bEndpointAddress                       */ 0x81u,
	/*  bmAttributes                           */ 0x02u,
	/*  wMaxPacketSize                         */ 0x40u, 0x00u,
	/*  bInterval                              */ 0x00u,
	/*  bRefresh                               */ 0x00u,
	/*  bSynchAddress                          */ 0x00u,
	/*********************************************************************
	* MS Bulk Data Endpoint Descriptor
	*********************************************************************/
	/*  Endpoint Descriptor Length             */ 0x07u,
	/*  DescriptorType: CS_ENDPOINT            */ 0x25u,
	/*  bDescriptorSubtype                     */ 0x01u,
	/*  bNumEmbMIDIJack                        */ 0x03u,
	/*  baAssocJackID                          */ 0x03u, 0x07u, 0x0Bu,
	/*********************************************************************
	* Endpoint Descriptor
	*********************************************************************/
	/*  Endpoint Descriptor Length             */ 0x09u,
	/*  DescriptorType: ENDPOINT               */ 0x05u,
	/*  bEndpointAddress                       */ 0x01u,
	/*  bmAttributes                           */ 0x02u,
	/*  wMaxPacketSize                         */ 0x40u, 0x00u,
	/*  bInterval                              */ 0x00u,
	/*  bRefresh                               */ 0x00u,
	/*  bSynchAddress                          */ 0x00u,
	/*********************************************************************
	* MS Bulk Data Endpoint Descriptor
	*********************************************************************/
	/*  Endpoint Descriptor Length             */ 0x07u,
	/*  DescriptorType: CS_ENDPOINT            */ 0x25u,
	/*  bDescriptorSubtype                     */ 0x01u,
	/*  bNumEmbMIDIJack                        */ 0x03u,
	/*  baAssocJackID                          */ 0x01u, 0x05u, 0x09u
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END=
{
	USB_LEN_DEV_QUALIFIER_DESC,
	USB_DESC_TYPE_DEVICE_QUALIFIER,
	0x00,
	0x02,
	0x00,_
	0x00,
	0x00,
	0x40,
	0x01,
	0x00,
};


static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	USBD_LL_OpenEP(&hUsbDeviceFS, USB_EP_OUT_ADDR_1, USBD_EP_TYPE_BULK, MIDI_OUT_PACKET_SIZE);
	USBD_LL_OpenEP(&hUsbDeviceFS, USB_EP_IN_ADDR_1,  USBD_EP_TYPE_BULK, MIDI_OUT_PACKET_SIZE);

	pdev->pClassData = &hUsbClassHandleMidi;
	USBD_MIDI_HandleTypeDef * hMidi = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;

	USBD_LL_PrepareReceive(pdev, USB_EP_OUT_ADDR_1, hMidi->buffer, MIDI_OUT_PACKET_SIZE);
	return USBD_OK;
}

static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	USBD_LL_CloseEP(pdev, USB_EP_OUT_ADDR_1);
	USBD_LL_CloseEP(pdev, USB_EP_IN_ADDR_1);
	return USBD_OK;
}

static uint8_t  USBD_MIDI_Setup (USBD_HandleTypeDef *pdev, 
                                USBD_SetupReqTypedef *req)
{
	  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_GetCfgDesc
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length)
{
	*length = sizeof (USBD_MIDI_CfgDesc);
	return USBD_MIDI_CfgDesc;
}

/**
  * @brief  USBD_MIDI_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	/* Only OUT data are processed */
	return USBD_OK;
}

/**
  * @brief  USBD_MIDI_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
	// IN Data (device to host)
	return USBD_OK;
}

/**
  * @brief  USBD_MIDI_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_TxReady (USBD_HandleTypeDef *pdev)
{
  /* Only OUT control data are processed */
  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_SOF (USBD_HandleTypeDef *pdev)
{
  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
/*
void  USBD_MIDI_Sync (USBD_HandleTypeDef *pdev, MIDI_OffsetTypeDef offset)
{
}
*/
/**
  * @brief  USBD_MIDI_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	return USBD_OK;
}
/**
  * @brief  USBD_MIDI_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_MIDI_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	return USBD_OK;
}

static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, 
                              uint8_t epnum)
{
	USBD_MIDI_HandleTypeDef   *hMidi;
	hMidi = (USBD_MIDI_HandleTypeDef*) pdev->pClassData;

	if (epnum == USB_EP_OUT_ADDR_1)
	{
		/* Prepare Out endpoint to receive next audio packet */
		USBD_LL_PrepareReceive(pdev, USB_EP_OUT_ADDR_1,
							   (uint8_t *)&hMidi->buffer, USB_MIDI_BUFFER_MAX_SIZE);
	}
	return USBD_OK;
}
// -----------------------------------------------------------------------------
static uint8_t  *USBD_MIDI_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_MIDI_DeviceQualifierDesc);
  return USBD_MIDI_DeviceQualifierDesc;
}

inline uint8_t  USBD_MIDI_RegisterInterface  (USBD_HandleTypeDef   *pdev,
                                        USBD_MIDI_ItfTypeDef *fops)
{
	if(fops != NULL)
	{
		pdev->pUserData= fops;
	}
	return 0;
}
