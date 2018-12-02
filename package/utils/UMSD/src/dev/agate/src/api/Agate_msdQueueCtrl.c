#include <msdCopyright.h>

/********************************************************************************
* Agate_msdQueueCtrl.c
*
* DESCRIPTION:
*       API definitions for Switch Queue Control, including per-port setting & global setting
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdQueueCtrl.h>
#include <agate/include/api/Agate_msdSysCtrl.h>
#include <agate/include/api/Agate_msdApiInternal.h>
#include <agate/include/driver/Agate_msdHwAccess.h>
#include <agate/include/driver/Agate_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>

/*******************************************************************************
* Agate_gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 AGATE_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 8,4,2,1 weighted fair
*											scheduling
*            AGATE_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - AGATE_MSD_PORT_SCHED_MODE enum type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_gprtSetPortSched
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AGATE_MSD_PORT_SCHED_MODE        mode
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8           phyAddr;

	MSD_DBG_INFO(("Agate_gprtSetPortSched Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	switch (mode)
	{
	case AGATE_MSD_PORT_SCHED_WRR_PRI3_2_1_0:
		data = 0;
		break;
	case AGATE_MSD_PORT_SCHED_WRR_PRI2_1_0:
		data = 1;
		break;
	case AGATE_MSD_PORT_SCHED_WRR_PRI1_0:
		data = 2;
		break;
	case AGATE_MSD_PORT_SCHED_STRICT_PRI_ALL:
		data = 3;
		break;
	default:
		MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)mode));
		return MSD_BAD_PARAM;
	}

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL2, 12, 2, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write port AGATE_QD_REG_EGRESS_RATE_CTRL2 register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gprtSetPortSched Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Agate_gprtGetPortSched
*
* DESCRIPTION:
*        This routine Agate_gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 AGATE_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 8,4,2,1 weighted fair
*											scheduling
*            AGATE_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - AGATE_MSD_PORT_SCHED_MODE enum type
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_gprtGetPortSched
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AGATE_MSD_PORT_SCHED_MODE        *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;
	MSD_U8           phyAddr;

	MSD_DBG_INFO(("Agate_gprtGetPortSched Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	phyAddr = AGATE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AGATE_QD_REG_EGRESS_RATE_CTRL2, 12, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read port AGATE_QD_REG_EGRESS_RATE_CTRL2 register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	switch (data)
	{
	case 0x0:
		*mode = AGATE_MSD_PORT_SCHED_WRR_PRI3_2_1_0;
		break;
	case 0x1:
		*mode = AGATE_MSD_PORT_SCHED_WRR_PRI2_1_0;
		break;
	case 0x2:
		*mode = AGATE_MSD_PORT_SCHED_WRR_PRI1_0;
		break;
	case 0x3:
		*mode = AGATE_MSD_PORT_SCHED_STRICT_PRI_ALL;
		break;
	default:
		MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)data));
		return MSD_BAD_PARAM;
	}

	MSD_DBG_INFO(("Agate_gprtGetPortSched Exit.\n"));
	return retVal;
}

/******************************************************************************
* gsysSetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4/8 output Queues. Queue 3/7 has the highest priority
*        Queue 0 has the lowest priority. When a scheduling mode of port is
*        configured as Weighted Round Robin queuing mode, the access sequece of
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        (Queue is 7,6,5,7,1,6,7,4 by default. That is after 6390.)
*        This sequence can be configured with this API.
*
* INPUTS:
*       weight - access sequence of the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
******************************************************************************/
MSD_STATUS Agate_gsysSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  AGATE_MSD_QoS_WEIGHT    *weight
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U16       data;
	MSD_U32       len, i;
	MSD_U32 length_loc, entry_num;

	MSD_DBG_INFO(("Agate_gsysSetQoSWeight Called.\n"));

	length_loc = 0x20;
	entry_num = 4;

	if (weight->len > 64)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM\n"));
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	len = weight->len / entry_num;

	/* program QoS Weight Table, 4/2 sequences at a time */
	for (i = 0; i<len; i++)
	{
		/* Wait until the QoS Weight Table is ready. */
		data = 1;
		while (data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,
				AGATE_QD_REG_QOS_WEIGHT, 15, 1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}


		data = (MSD_U16)((1 << 15) | (i << 8) |
			(weight->queue[i * 4] & 0x3) |
			((weight->queue[i * 4 + 1] & 0x3) << 2) |
			((weight->queue[i * 4 + 2] & 0x3) << 4) |
			((weight->queue[i * 4 + 3] & 0x3) << 6));

		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}

	/* program remaining sequences if any */
	i = weight->len % entry_num;
	if (i)
	{
		/* Wait until the QoS Weight Table is ready. */
		data = 1;
		while (data == 1)
		{
			retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,
				AGATE_QD_REG_QOS_WEIGHT, 15, 1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
		}

		data = (MSD_U16)((1 << 15) | (len << 8));
		if (i == 1)
			data |= ((weight->queue[len * 4 + 0] & 0x3) << 0);
		else if (i == 2)
			data |= ((weight->queue[len * 4 + 0] & 0x3) << 0) |
			((weight->queue[len * 4 + 1] & 0x3) << 2);
		else if (i == 3)
			data |= ((weight->queue[len * 4 + 0] & 0x3) << 0) |
			((weight->queue[len * 4 + 1] & 0x3) << 2) |
			((weight->queue[len * 4 + 2] & 0x3) << 4);

		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}

	/* Write the lengh of the sequence */

	/* Wait until the QoS Weight Table is ready. */
	data = 1;
	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,
			AGATE_QD_REG_QOS_WEIGHT, 15, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}

	data = (MSD_U16)((1 << 15) | (length_loc << 8) | weight->len);

	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);

	msdSemGive(dev->devNum, dev->tblRegsSem);

	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Agate_gsysSetQoSWeight Exit.\n"));
	return MSD_OK;
}

/******************************************************************************
* gsysGetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4/8 output Queues. Queue 3/7 has the highest priority
*        Queue 0 has the lowest priority. When a scheduling mode of port is
*        configured as Weighted Round Robin queuing mode, the access sequece of
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        (Queue is 7,6,5,7,1,6,7,4 by default. That is after 6390.)
*        This routine retrieves the access sequence of the Queue.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       weight - access sequence of the queue
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
******************************************************************************/
MSD_STATUS Agate_gsysGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
OUT AGATE_MSD_QoS_WEIGHT    *weight
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U16        data, i;
	MSD_U32        len;
	MSD_U16 length_loc, entry_num;

	MSD_DBG_INFO(("Agate_gsysGetQoSWeight Called.\n"));

	length_loc = 0x20;
	entry_num = 4;

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Read the lengh of the sequence */
	/* Wait until the QoS Weight Table is ready. */
	data = 1;
	while (data == 1) {

		retVal = msdGetAnyRegField(dev->devNum, AGATE_GLOBAL2_DEV_ADDR,
			AGATE_QD_REG_QOS_WEIGHT, 15, 1, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}

	data = (length_loc << 8);

	retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	weight->len = data & 0xFF;

	len = weight->len / entry_num;

	/* read QoS Weight Table, 4/2 sequences at a time */
	for (i = 0; i<len; i++)
	{
		data = i << 8;

		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		weight->queue[i * 4] = data & 0x3;
		weight->queue[i * 4 + 1] = (data >> 2) & 0x3;
		weight->queue[i * 4 + 2] = (data >> 4) & 0x3;
		weight->queue[i * 4 + 3] = (data >> 6) & 0x3;
	}

	/* read remaining sequences if any */
	i = (MSD_U16)(weight->len % entry_num);
	if (i)
	{
		data = (MSD_U16)(len << 8);

		retVal = msdSetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = msdGetAnyReg(dev->devNum, AGATE_GLOBAL2_DEV_ADDR, AGATE_QD_REG_QOS_WEIGHT, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Failed.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		weight->queue[len * 4] = data & 0x3;
		if (i > 1)
			weight->queue[len * 4 + 1] = (data >> 2) & 0x3;
		if (i > 2)
			weight->queue[len * 4 + 2] = (data >> 4) & 0x3;

	}

	msdSemGive(dev->devNum, dev->tblRegsSem);

	MSD_DBG_INFO(("Agate_gsysGetQoSWeight Called.\n"));
	return MSD_OK;
}
