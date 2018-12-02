#include <msdCopyright.h>
/*******************************************************************************
*              Copyright 2017, MARVELL SEMICONDUCTOR, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* msdPortRmon.h
*
* DESCRIPTION:
*       API definitions for MIBs.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __msdPortRmon_h
#define __msdPortRmon_h

#include <msdApiTypes.h>
#include <msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdStatsFlushAll
*
* DESCRIPTION:
*       Flushs all RMON counters for all ports.
*
* INPUTS:
*       devNum  - physical devie number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdStatsFlushAll
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdStatsFlushPort
*
* DESCRIPTION:
*       Flush all RMON counters for a given port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdStatsFlushPort
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);

/*******************************************************************************
* msdStatsPortCounterGet
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*       counter - the counter which will be read
*
* OUTPUTS:
*       statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdStatsPortCounterGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_STATS_COUNTERS  counter,
    OUT MSD_U32  *statsData
);

/*******************************************************************************
* msdStatsPortAllCountersGet
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to MSD_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdStatsPortAllCountersGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_STATS_COUNTER_SET  *statsCounterSet
);

/*******************************************************************************
* msdStatsHistogramModeGet
*
* DESCRIPTION:
*       This routine gets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical devie number
*       None.
*
* OUTPUTS:
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdStatsHistogramModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_HISTOGRAM_MODE  *mode
);

/*******************************************************************************
* msdStatsHistogramModeSet
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical devie number
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS msdStatsHistogramModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_HISTOGRAM_MODE  mode
);

/*******************************************************************************
* msdPortMibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPortMibDump
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);
#ifdef __cplusplus
}
#endif

#endif /* __msdPortRmon_h */