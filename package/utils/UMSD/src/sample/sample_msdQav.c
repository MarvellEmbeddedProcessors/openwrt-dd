#include <msdCopyright.h>
/********************************************************************************
* sample_msdQav.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Set per port and per queue rate limit.
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Configure port1 for queue1 rate limit as 200M
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdQav(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U8 queue;
	MSD_LPORT port;
	MSD_U32 rate;

	/*Set the port1 priority1 rate limit is 200M*/
	port = 1;
	queue = 1;
	rate = 200000;

	if ((status = msdQavPortQpriXRateSet(devNum, port, queue, rate)) != MSD_OK)
	{
		MSG_PRINT(("msdQavPortQpriXRateSet returned fail.\n"));
		return status;
	}

	/**************************************************************************************
	*   After this setup, the frames fowarded to port1 with the priority1 will be rate    *
	*   limited 200Mbps                                                                   *
	**************************************************************************************/

	MSG_PRINT(("sample_msdQav success.\n"));
	return MSD_OK;
}

