
/**
 * Includes
 */
#include "rak_global.h"
#include "rak411_api.h"
#include <stdio.h>
#include <string.h>


/*===========================================================================
 *
 * @fn          uint16_t rak_Ping(rak_Ping_t *uPingFrame)
 * @brief       Sends the Ping command to the Wi-Fi module
 * @param[in]   rak_Ping_t *uPingFrame,Pointer to Ping structure
 * @param[out]  none
 * @return      errCode
 *              -1 = SPI busy / Timeout
 *              -2 = Failure
 *              0  = SUCCESS
 * @section description 
 * This API is used to Ping hostaddr.  
 */

uint16_t rak_Ping(rak_Ping_t *uPingFrame)
{
	int16_t					retval;
#ifdef RAK_DEBUG_PRINT
	DPRINTF("Ping Start \r\n");
#endif
	uPingFrame->cmd = PinG_CMD;
  	retval =  RAK_SendCMD(PinG_CMD,uPingFrame, sizeof(rak_Ping_t));
	return retval;
}
