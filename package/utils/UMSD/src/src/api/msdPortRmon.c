#include <msdCopyright.h>

/********************************************************************************
* msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for MIBs
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msdPortRmon.h>
#include <msdApiTypes.h>
#include <msdUtils.h>

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll(dev);
	else
		return MSD_NOT_SUPPORTED;
}

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort(dev, portNum);
	else
		return MSD_NOT_SUPPORTED;
}

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter(dev, portNum, counter, statsData);
	else
		return MSD_NOT_SUPPORTED;
}

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters(dev, portNum, statsCounterSet);
	else
		return MSD_NOT_SUPPORTED;
}

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode(dev, mode);
	else
		return MSD_NOT_SUPPORTED;
}

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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode(dev, mode);
	else
		return MSD_NOT_SUPPORTED;
}
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
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	if (dev->SwitchDevObj.PORTRMONObj.gmibDump != NULL)
		return dev->SwitchDevObj.PORTRMONObj.gmibDump(dev, portNum);
	else
		return MSD_NOT_SUPPORTED;
}
