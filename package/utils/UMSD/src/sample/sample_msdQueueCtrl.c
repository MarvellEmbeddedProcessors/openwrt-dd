#include <msdCopyright.h>
/********************************************************************************
* sample_msdQueueCtrl.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*		Sample #1: enable egress hard queue limit for port 2 queue 2, when port 2 is 
*		overloaded with Queue priority 2, the queue counter should not exceeds the limit 
*
*		Sample #2: port schedule set for port 2, frames with strict priority can fully
*		occupy the bandwidth, frames with weighted round rouin will occupy the rest
*		bandwidth based on the according ratio in datasheet
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1: enable egress hard queue limit for port 2 queue 2, when port 2 is 
* overloaded with Queue priority 2, the queue counter should not exceeds the limit   
********************************************************************************/
MSD_STATUS sample_msdQueueCtrl_HardLimit(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_LPORT port = 2;
	MSD_U8 pointer = 0x12;		/* for Hard Queue Limit2 */
	MSD_BOOL hardOnly2 = MSD_TRUE;		/* enable Hard Queue Limit */
	MSD_U8 limit = 5;		/* limit is 5 */
	MSD_U8 data = (MSD_U8)((hardOnly2 << 7) | limit);

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*
	*    Set Hard Limit using Queue Indirect Access
	*/
	if ((status = msdPortQueueCtrlSet(devNum, port, pointer, data)) != MSD_OK)
	{
		MSG_PRINT(("msdPortQueueCtrlSet returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdQueueCtrl_HardLimit success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2: port schedule set for port 2, frames with strict priority can fully
* occupy the bandwidth, frames with weighted round rouin will occupy the rest 
* bandwidth based on the according ratio in datasheet 
********************************************************************************/
MSD_STATUS sample_msdQueueCtrl_PortSchedule(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_LPORT port = 2;
	MSD_PORT_SCHED_MODE mode;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	if (dev->devName == MSD_PERIDOT)
	{
		/* use strict for priority 7 and use weighted round robin for priorities 6, 
		5, 4, 3, 2, 1, 0(default is 25, 17, 12, 6, 3, 2, 1) */
		mode = MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0;
	}

	if (dev->devName == MSD_TOPAZ || dev->devName == MSD_AGATE || dev->devName == MSD_PEARL)
	{
		/* use strict for priority 3 and use weighted round robin for priorities 2,
		1, 0(default is 4, 2, 1) */
		mode = MSD_PORT_SCHED_WRR_PRI2_1_0;
	}

	/*
	*    Set Port's Schedule Mode
	*/
	if ((status = msdPortSchedSet(devNum, port, mode)) != MSD_OK)
	{
		MSG_PRINT(("msdPortSchedSet returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdQueueCtrl_PortSchedule success.\n"));
	return MSD_OK;
}