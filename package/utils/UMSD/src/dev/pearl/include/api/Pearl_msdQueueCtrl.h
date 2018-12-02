#include <msdCopyright.h>
/*******************************************************************************
* Pearl_msdQueueCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Queue Control functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Pearl_msdQueueCtrl_h
#define __Pearl_msdQueueCtrl_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                        */
/****************************************************************************/

#define PEARL_MAX_QOS_WEIGHTS        128

/*
*  typedef: enum PEARL_MSD_PORT_SCHED_MODE
*
*  Description: Enumeration of port scheduling mode
*
*  Fields:
*		   PEARL_MSD_PORT_SCHED_WRR_PRI3_2_1_0 - Use an weighted round robin queuing scheme
*
*		   PEARL_MSD_PORT_SCHED_WRR_PRI2_1_0 - use a strict for priority 3 and
*										weighted round robin for priorities 2,1 and 0
*		   PEARL_MSD_PORT_SCHED_WRR_PRI1_0 - use a strict for priority 2
*										and weighted round robin for priorities 1 and 0
*		   PEARL_MSD_PORT_SCHED_STRICT_PRI_ALL - use a strict priority scheme for all priorities
*
*  Comment:
*/
typedef enum
{
    PEARL_MSD_PORT_SCHED_WRR_PRI3_2_1_0 = 0x4,
    PEARL_MSD_PORT_SCHED_WRR_PRI2_1_0,
    PEARL_MSD_PORT_SCHED_WRR_PRI1_0,
    PEARL_MSD_PORT_SCHED_STRICT_PRI_ALL
} PEARL_MSD_PORT_SCHED_MODE;

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
    MSD_U8         queue[PEARL_MAX_QOS_WEIGHTS];
}PEARL_MSD_QoS_WEIGHT;

/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 PEARL_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair
*											scheduling
*            PEARL_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - PEARL_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Pearl_gprtSetPortSched
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  PEARL_MSD_PORT_SCHED_MODE        mode
);
MSD_STATUS Pearl_gprtSetPortSchedIntf
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
*			 PEARL_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair
*											scheduling
*            PEARL_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - PEARL_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Pearl_gprtGetPortSched
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT PEARL_MSD_PORT_SCHED_MODE        *mode
);
MSD_STATUS Pearl_gprtGetPortSchedIntf
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
MSD_STATUS Pearl_gsysSetQoSWeightIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_QoS_WEIGHT    *weight
);
MSD_STATUS Pearl_gsysSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  PEARL_MSD_QoS_WEIGHT    *weight
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
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
******************************************************************************/
MSD_STATUS Pearl_gsysGetQoSWeightIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_QoS_WEIGHT    *weight
);
MSD_STATUS Pearl_gsysGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
OUT PEARL_MSD_QoS_WEIGHT    *weight
);
#ifdef __cplusplus
}
#endif

#endif /* __Pearl_msdQueueCtrl_h */
