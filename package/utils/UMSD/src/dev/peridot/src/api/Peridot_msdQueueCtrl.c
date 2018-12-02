#include <msdCopyright.h>

/********************************************************************************
* Peridot_msdQueueCtrl.c
*
* DESCRIPTION:
*       API definitions for Switch Queue Control, including per-port setting & global setting
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <peridot/include/api/Peridot_msdQueueCtrl.h>
#include <peridot/include/api/Peridot_msdSysCtrl.h>
#include <peridot/include/api/Peridot_msdApiInternal.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#include <peridot/include/driver/Peridot_msdDrvSwRegs.h>
#include <msdUtils.h>
#include <msdSem.h>
#include <msdHwAccess.h>


/*******************************************************************************
* Peridot_gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            PERIDOT_MSD_QUEUE_CFG_PORT_SCHEDULE
*            PERIDOT_MSD_QUEUE_CFG_FILTER_Q_EN
*            PERIDOT_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            PERIDOT_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            PERIDOT_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 PERIDOT_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            PERIDOT_MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
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
MSD_STATUS Peridot_gprtSetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    IN  MSD_U8             data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16            tmpData;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gprtSetQueueCtrl Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    do {
        retVal = msdGetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_Q_CONTROL, &tmpData);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read port Queue Control register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait port Queue Control register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (tmpData&0x8000);

	tmpData =  (MSD_U16)((1 << 15) | ((point&0x7f) << 8) | data);

	retVal = msdSetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_Q_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
		 MSD_DBG_ERROR(("Set port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		 msdSemGive(dev->devNum, dev->tblRegsSem);
		 return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);

	MSD_DBG_INFO(("Peridot_gprtSetQueueCtrl Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            PERIDOT_MSD_QUEUE_CFG_PORT_SCHEDULE
*            PERIDOT_MSD_QUEUE_CFG_FILTER_Q_EN
*            PERIDOT_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            PERIDOT_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            PERIDOT_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            PERIDOT_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 PERIDOT_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            PERIDOT_MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
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
MSD_STATUS Peridot_gprtGetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16            tmpData;
    int count=0x10;

    MSD_DBG_INFO(("Peridot_gprtSetQueueCtrl Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = PERIDOT_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    do {
        retVal = msdGetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_Q_CONTROL, &tmpData);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read port Queue Control register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) 
		{
			MSD_DBG_ERROR(("Wait port Queue Control register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (tmpData&0x8000);

	tmpData =  (MSD_U16)((point&0x7f) << 8);
	retVal = msdSetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_Q_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
		 MSD_DBG_ERROR(("Set port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		 msdSemGive(dev->devNum, dev->tblRegsSem);
		 return retVal;
	}

    retVal = msdGetAnyReg(dev->devNum,  phyAddr, PERIDOT_QD_REG_Q_CONTROL, &tmpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }
    *data = (MSD_U8)(tmpData&0xff);

    msdSemGive(dev->devNum, dev->tblRegsSem);

	MSD_DBG_INFO(("Peridot_gprtSetQueueCtrl Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Peridot_gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 PERIDOT_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            PERIDOT_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - PERIDOT_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Peridot_gprtSetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  PERIDOT_MSD_PORT_SCHED_MODE        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Peridot_gprtSetPortSched Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
   
    switch(mode)
    {
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0:
            data = 0;
            break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0:
			data = 1;
			break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0:
			data = 2;
			break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI4_3_2_1_0:
			data = 3;
			break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI3_2_1_0:
			data = 4;
			break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI2_1_0:
			data = 5;
			break;
        case PERIDOT_MSD_PORT_SCHED_WRR_PRI1_0:
			data = 6;
			break;
        case PERIDOT_MSD_PORT_SCHED_STRICT_PRI_ALL:
            data = 7;
            break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)mode));
            return MSD_BAD_PARAM;
    }

    retVal = Peridot_gprtSetQueueCtrl(dev, port,0,(MSD_U8)data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_INFO(("Peridot_gprtSetQueueCtrl returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Peridot_gprtSetPortSched Exit.\n"));

    return retVal;
}

/*******************************************************************************
* Peridot_gprtGetPortSched
*
* DESCRIPTION:
*        This routine Peridot_gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 PERIDOT_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            PERIDOT_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - PERIDOT_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Peridot_gprtGetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT PERIDOT_MSD_PORT_SCHED_MODE        *mode
)
{      
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8 datac;

    MSD_DBG_INFO(("Peridot_gprtGetPortSched Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    retVal = Peridot_gprtGetQueueCtrl(dev,port,0, (MSD_U8 *)&datac);
    if(retVal != MSD_OK)
    {
		MSD_DBG_INFO(("Peridot_gprtGetQueueCtrl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    switch(datac)
    {
		case 0x0:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0;
            break;
        case 0x1:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0;
            break;
        case 0x2:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0;
            break;
        case 0x3:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI4_3_2_1_0;
            break;
		case 0x4:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI3_2_1_0;
            break;
        case 0x5:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI2_1_0;
            break;
        case 0x6:
            *mode = PERIDOT_MSD_PORT_SCHED_WRR_PRI1_0;
            break;
		case 0x7:
            *mode = PERIDOT_MSD_PORT_SCHED_STRICT_PRI_ALL;
			break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)datac));
            return MSD_BAD_PARAM;
    }
	
	MSD_DBG_INFO(("Peridot_gprtGetPortSched Exit.\n"));
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
MSD_STATUS Peridot_gsysSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  PERIDOT_MSD_QoS_WEIGHT    *weight
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16       data;
    MSD_U32       len, i;
    MSD_U32 length_loc, entry_num;

    MSD_DBG_INFO(("gsysSetQoSWeight Called.\n"));

    length_loc = 0x40;
    entry_num = 2;

    if (weight->len > 128)
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
            retVal = msdGetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,
                PERIDOT_QD_REG_QOS_WEIGHT, 15, 1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->tblRegsSem);
                return retVal;
            }
        }


        data = (MSD_U16)((1 << 15) | (i << 8) |
            (weight->queue[i * 2] & 0x7) |
            ((weight->queue[i * 2 + 1] & 0x7) << 4));

        retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);
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
            retVal = msdGetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,
                PERIDOT_QD_REG_QOS_WEIGHT, 15, 1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->tblRegsSem);
                return retVal;
            }
        }

        data = (MSD_U16)((1 << 15) | (len << 8));

        data |= ((weight->queue[len * 2 + 0] & 0x7) << 0);

        retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);
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
        retVal = msdGetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,
            PERIDOT_QD_REG_QOS_WEIGHT, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

    data = (MSD_U16)((1 << 15) | (length_loc << 8) | weight->len);

    retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);

    msdSemGive(dev->devNum, dev->tblRegsSem);

    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed.\n"));
        return retVal;
    }

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
MSD_STATUS Peridot_gsysGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
OUT PERIDOT_MSD_QoS_WEIGHT    *weight
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data, i;
    MSD_U32        len;
    MSD_U16 length_loc, entry_num;

    MSD_DBG_INFO(("gsysGetQoSWeight Called.\n"));

    length_loc = 0x40;
    entry_num = 2;

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    /* Read the lengh of the sequence */
    /* Wait until the QoS Weight Table is ready. */
    data = 1;
    while (data == 1) {

        retVal = msdGetAnyRegField(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR,
            PERIDOT_QD_REG_QOS_WEIGHT, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

    data = (length_loc << 8);

    retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed.\n"));
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

    retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, &data);
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

        retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

        retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, &data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

        weight->queue[i * 2] = data & 0x7;
        weight->queue[i * 2 + 1] = (data >> 4) & 0x7;
    }

    /* read remaining sequences if any */
    i = (MSD_U16)(weight->len % entry_num);
    if (i)
    {
        data = (MSD_U16)(len << 8);

        retVal = msdSetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

        retVal = msdGetAnyReg(dev->devNum, PERIDOT_GLOBAL2_DEV_ADDR, PERIDOT_QD_REG_QOS_WEIGHT, &data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

        weight->queue[len * 2] = data & 0x7;
    }

    msdSemGive(dev->devNum, dev->tblRegsSem);
    return MSD_OK;
}
