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
MSD_STATUS Agate_gprtSetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_PORT_SCHED_MODE        mode
)
{
	AGATE_MSD_PORT_SCHED_MODE agateMode;
	switch (mode)
	{
	case MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0:
	case MSD_PORT_SCHED_WRR_PRI4_3_2_1_0:
		return MSD_NOT_SUPPORTED;
	case MSD_PORT_SCHED_WRR_PRI3_2_1_0:
		agateMode = AGATE_MSD_PORT_SCHED_WRR_PRI3_2_1_0;
		break;
	case MSD_PORT_SCHED_WRR_PRI2_1_0:
		agateMode = AGATE_MSD_PORT_SCHED_WRR_PRI2_1_0;
		break;
	case MSD_PORT_SCHED_WRR_PRI1_0:
		agateMode = AGATE_MSD_PORT_SCHED_WRR_PRI1_0;
		break;
	case MSD_PORT_SCHED_STRICT_PRI_ALL:
		agateMode = AGATE_MSD_PORT_SCHED_STRICT_PRI_ALL;
		break;
	default:
		return MSD_BAD_PARAM;
	}
	return Agate_gprtSetPortSched(dev, port, agateMode);
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
MSD_STATUS Agate_gprtGetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_SCHED_MODE        *mode
)
{
	return Agate_gprtGetPortSched(dev, port, (AGATE_MSD_PORT_SCHED_MODE*)mode);
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
MSD_STATUS Agate_gsysSetQoSWeightIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    AGATE_MSD_QoS_WEIGHT agate_weight;
    if (weight->len > AGATE_MAX_QOS_WEIGHTS)
        return MSD_BAD_PARAM;

    for (i = 0; i < weight->len; i++)
    {
        agate_weight.queue[i] = weight->queue[i];
    }
    agate_weight.len = weight->len;
    return Agate_gsysSetQoSWeight(dev, &agate_weight);
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
MSD_STATUS Agate_gsysGetQoSWeightIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    MSD_STATUS status;
    AGATE_MSD_QoS_WEIGHT agate_weight;
    if ((status = Agate_gsysGetQoSWeight(dev, &agate_weight)) != MSD_OK)
        return status;

    if (agate_weight.len > AGATE_MAX_QOS_WEIGHTS)
        return MSD_BAD_PARAM;

    for (i = 0; i < agate_weight.len; i++)
    {
        weight->queue[i] = agate_weight.queue[i];
    }
    weight->len = agate_weight.len;

    return status;
}
