#ifndef __USBD_DESC__C__
#define __USBD_DESC__C__

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

extern USBD_DescriptorsTypeDef FS_Desc;

uint8_t  * USBD_GetUserDectriptor (struct _USBD_HandleTypeDef *pdev ,uint8_t index,  uint16_t *length);


#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__C__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
