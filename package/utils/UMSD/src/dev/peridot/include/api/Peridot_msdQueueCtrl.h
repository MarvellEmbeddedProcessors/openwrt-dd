#include <msdCopyright.h>
/*******************************************************************************
* Peridot_msdQueueCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Queue Control functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_msdQueueCtrl_h
#define __Peridot_msdQueueCtrl_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

#define PERIDOT_MAX_QOS_WEIGHTS        128

/*
*  typedef: enum PERIDOT_MSD_PORT_SCHED_MODE
*
*  Description: Enumeration of port scheduling mode
*
*  Fields:
*
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0 - use 33,25,17,12,6,3,2,1 weighted fair
*										scheduling
*	       PERIDOT_MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0 - use a strict for priority 7 and weighted
*										round robin for priorities 6,5,4,3,2,1 and 0
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0 - use a strict for priority 7,6 and weighted
*										round robin for priorities 5,4,3,2,1 and 0
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI4_3_2_1_0 - use a strict for priority 7,6,5 and weighted
*										round robin for priorities 4,3,2,1 and 0
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI3_2_1_0 - use a strict for priority 7,6,5,4 and
*										weighted round robin for priorities 3,2,1 and 0
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI2_1_0 - use a strict for priority 7,6,5,4,3 and
*										weighted round robin for priorities 2,1 and 0
*		   PERIDOT_MSD_PORT_SCHED_WRR_PRI1_0 - use a strict for priority 7,6,5,4,3,2
*										and weighted round robin for priorities 1 and 0
*		   PERIDOT_MSD_PORT_SCHED_STRICT_PRI_ALL - use a strict priority scheme for all priorities
*
*  Comment:
*/
typedef enum
{
    PERIDOT_MSD_PORT_SCHED_WRR_PRI7_6_5_4_3_2_1_0 = 0x0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI6_5_4_3_2_1_0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI5_4_3_2_1_0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI4_3_2_1_0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI3_2_1_0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI2_1_0,
    PERIDOT_MSD_PORT_SCHED_WRR_PRI1_0,
    PERIDOT_MSD_PORT_SCHED_STRICT_PRI_ALL
} PERIDOT_MSD_PORT_SCHED_MODE;

/*
* Typedef: struct MSD_QoS_WEIGHT
*
* Description: This structure is used for Programmable Round Robin Weights.
*
* Fields:
*      len    - length of the valid queue data
*      queue  - upto 128 queue data
*/
typedef struct
{
    MSD_U32        len;
    MSD_U8         queue[PERIDOT_MAX_QOS_WEIGHTS];
}PERIDOT_MSD_QoS_WEIGHT;


/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            MSD_QUEUE_CFG_PORT_SCHEDULE
*            MSD_QUEUE_CFG_FILTER_Q_EN
*            MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 MSD_QUEUE_CFG_FC_THRESHOLD_END
*            MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        port  - the logical port number
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    IN  MSD_U8             data
);
MSD_STATUS Peridot_gprtSetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
IN  MSD_U8             data
);
/*******************************************************************************
* gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            MSD_QUEUE_CFG_PORT_SCHEDULE
*            MSD_QUEUE_CFG_FILTER_Q_EN
*            MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 MSD_QUEUE_CFG_FC_THRESHOLD_END
*            MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        port  - the logical port number
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
);
MSD_STATUS Peridot_gprtGetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
OUT  MSD_U8            *data
);
/*******************************************************************************
* gprtSetPortSched
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS: 
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtSetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  PERIDOT_MSD_PORT_SCHED_MODE        mode
);
MSD_STATUS Peridot_gprtSetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_PORT_SCHED_MODE        mode
);
/*******************************************************************************
* gprtGetPortSched
*
* DESCRIPTION:
*        This routine gets Port Scheduling Mode.
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS: 
*		None.
*
*******************************************************************************/
MSD_STATUS Peridot_gprtGetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT PERIDOT_MSD_PORT_SCHED_MODE        *mode
);
MSD_STATUS Peridot_gprtGetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_SCHED_MODE        *mode
);

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
MSD_STATUS Peridot_gsysSetQoSWeightIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_QoS_WEIGHT    *weight
);
MSD_STATUS Peridot_gsysSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  PERIDOT_MSD_QoS_WEIGHT    *weight
);

/******************************************************************************
* gsysGetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 8 output Queues. Queue 7 has the highest priority
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
MSD_STATUS Peridot_gsysGetQoSWeightIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_QoS_WEIGHT    *weight
);
MSD_STATUS Peridot_gsysGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
OUT PERIDOT_MSD_QoS_WEIGHT    *weight
);

#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdQueueCtrl_h */
