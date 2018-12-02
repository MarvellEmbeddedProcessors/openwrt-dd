#include <msdCopyright.h>

/********************************************************************************
* Topaz_msdQueueCtrl.c
*
* DESCRIPTION:
*       API definitions for Switch Queue Control, including per-port setting & global setting
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <topaz/include/api/Topaz_msdQueueCtrl.h>


/*******************************************************************************
* Topaz_gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
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
MSD_STATUS Topaz_gprtSetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
IN  MSD_U8             data
)
{
	return Topaz_gprtSetQueueCtrl(dev, port, point, data);
}
/*******************************************************************************
* Topaz_gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
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
MSD_STATUS Topaz_gprtGetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
OUT  MSD_U8            *data
)
{
	return Topaz_gprtGetQueueCtrl(dev, port, point, data);
}

/*******************************************************************************
* Topaz_gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 TOPAZ_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair
*											scheduling
*            TOPAZ_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - TOPAZ_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Topaz_gprtSetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_PORT_SCHED_MODE        mode
)
{
	TOPAZ_MSD_PORT_SCHED_MODE topazMode;
	switch (mode)
	{
	case MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI4_3_2_1_0:
		return MSD_NOT_SUPPORTED;
	case MSD_PORT_SCHED_WRR_PRI3_2_1_0:
		topazMode = TOPAZ_MSD_PORT_SCHED_WRR_PRI3_2_1_0;
		break;
	case MSD_PORT_SCHED_WRR_PRI2_1_0:
		topazMode = TOPAZ_MSD_PORT_SCHED_WRR_PRI2_1_0;
		break;
	case MSD_PORT_SCHED_WRR_PRI1_0:
		topazMode = TOPAZ_MSD_PORT_SCHED_WRR_PRI1_0;
		break;
	case MSD_PORT_SCHED_STRICT_PRI_ALL:
		topazMode = TOPAZ_MSD_PORT_SCHED_STRICT_PRI_ALL;
		break;
	default:
		return MSD_BAD_PARAM;
	}
    return Topaz_gprtSetPortSched(dev, port, topazMode);
}
/*******************************************************************************
* Topaz_gprtGetPortSched
*
* DESCRIPTION:
*        This routine Topaz_gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 TOPAZ_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair
*											scheduling
*            TOPAZ_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - TOPAZ_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Topaz_gprtGetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_SCHED_MODE        *mode
)
{
    return Topaz_gprtGetPortSched(dev, port, (TOPAZ_MSD_PORT_SCHED_MODE*)mode);
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
MSD_STATUS Topaz_gsysSetQoSWeightIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    TOPAZ_MSD_QoS_WEIGHT topaz_weight;
    if (weight->len > TOPAZ_MAX_QOS_WEIGHTS)
        return MSD_BAD_PARAM;

    for (i = 0; i < weight->len; i++)
    {
        topaz_weight.queue[i] = weight->queue[i];
    }
    topaz_weight.len = weight->len;
    return Topaz_gsysSetQoSWeight(dev, &topaz_weight);
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
MSD_STATUS Topaz_gsysGetQoSWeightIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    MSD_STATUS status;
    TOPAZ_MSD_QoS_WEIGHT topaz_weight;
    if ((status = Topaz_gsysGetQoSWeight(dev, &topaz_weight)) != MSD_OK)
        return status;

    if (topaz_weight.len > TOPAZ_MAX_QOS_WEIGHTS)
        return MSD_BAD_PARAM;

    for (i = 0; i < topaz_weight.len; i++)
    {
        weight->queue[i] = topaz_weight.queue[i];
    }
    weight->len = topaz_weight.len;

    return status;
}