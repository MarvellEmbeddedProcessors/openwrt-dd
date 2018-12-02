#include <msdCopyright.h>

/*******************************************************************************
* Agate_msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/


#include <agate/include/api/Agate_msdPortRmon.h>

/*******************************************************************************
* Agate_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine Agate_gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Agate_gstatsGetPortCounterIntf
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
	return Agate_gstatsGetPortCounter(dev, port, (AGATE_MSD_STATS_COUNTERS)counter, statsData);
}


/*******************************************************************************
* Agate_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine Agate_gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to AGATE_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Agate_gstatsGetPortAllCountersIntf
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
	AGATE_MSD_STATS_COUNTER_SET  statsCounter;
	MSD_STATUS    retVal;

	retVal=Agate_gstatsGetPortAllCounters(dev, port, &statsCounter);
	if (MSD_OK != retVal)
		return retVal;

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("Input param MSD_STATS_COUNTER_SET in Agate_gstatsGetPortAllCountersIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));
	statsCounterSet->InGoodOctetsLo = statsCounter.InGoodOctetsLo;
	statsCounterSet->InGoodOctetsHi = statsCounter.InGoodOctetsHi;
	statsCounterSet->InBadOctets = statsCounter.InBadOctets;
	statsCounterSet->OutFCSErr = statsCounter.OutFCSErr;
	statsCounterSet->InUnicasts = statsCounter.InUnicasts;
	statsCounterSet->Deferred = statsCounter.Deferred;
	statsCounterSet->InBroadcasts = statsCounter.InBroadcasts;
	statsCounterSet->InMulticasts = statsCounter.InMulticasts;

	statsCounterSet->Octets64 = statsCounter.Octets64;
	statsCounterSet->Octets127 = statsCounter.Octets127;
	statsCounterSet->Octets255 = statsCounter.Octets255;
	statsCounterSet->Octets511 = statsCounter.Octets511;
	statsCounterSet->Octets1023 = statsCounter.Octets1023;
	statsCounterSet->OctetsMax = statsCounter.OctetsMax;
	statsCounterSet->OutOctetsLo = statsCounter.OutOctetsLo;
	statsCounterSet->OutOctetsHi = statsCounter.OutOctetsHi;
	statsCounterSet->OutUnicasts = statsCounter.OutUnicasts;
	statsCounterSet->Excessive = statsCounter.Excessive;
	statsCounterSet->OutMulticasts = statsCounter.OutMulticasts;
	statsCounterSet->OutBroadcasts = statsCounter.OutBroadcasts;
	statsCounterSet->Single = statsCounter.Single;

	statsCounterSet->OutPause = statsCounter.OutPause;
	statsCounterSet->InPause = statsCounter.InPause;
	statsCounterSet->Multiple = statsCounter.Multiple;
	statsCounterSet->InUndersize = statsCounter.InUndersize;
	statsCounterSet->InFragments = statsCounter.InFragments;
	statsCounterSet->InOversize = statsCounter.InOversize;
	statsCounterSet->InJabber = statsCounter.InJabber;
	statsCounterSet->InRxErr = statsCounter.InRxErr;
	statsCounterSet->InFCSErr = statsCounter.InFCSErr;
	statsCounterSet->Collisions = statsCounter.Collisions;
	statsCounterSet->Late = statsCounter.Late;

	statsCounterSet->InDiscards = 0;
	statsCounterSet->InFiltered = 0;
	statsCounterSet->InAccepted = 0;
	statsCounterSet->InBadAccepted = 0;
	statsCounterSet->InGoodAvbClassA = 0;
	statsCounterSet->InGoodAvbClassB = 0;
	statsCounterSet->InBadAvbClassA = 0;
	statsCounterSet->InBadAvbClassB = 0;
	statsCounterSet->TCAMCounter0 = 0;
	statsCounterSet->TCAMCounter1 = 0;
	statsCounterSet->TCAMCounter2 = 0;
	statsCounterSet->TCAMCounter3 = 0;
	statsCounterSet->InDroppedAvbA = 0;
	statsCounterSet->InDroppedAvbB = 0;
	statsCounterSet->InDaUnknown = 0;
	statsCounterSet->InMGMT = 0;
	statsCounterSet->OutQueue0 = 0;
	statsCounterSet->OutQueue1 = 0;
	statsCounterSet->OutQueue2 = 0;
	statsCounterSet->OutQueue3 = 0;
	statsCounterSet->OutQueue4 = 0;
	statsCounterSet->OutQueue5 = 0;
	statsCounterSet->OutQueue6 = 0;
	statsCounterSet->OutQueue7 = 0;
	statsCounterSet->OutCutThrough = 0;
	statsCounterSet->reserved_19 = 0;
	statsCounterSet->OutOctetsA = 0;
	statsCounterSet->OutOctetsB = 0;
	statsCounterSet->OutYel = 0;
	statsCounterSet->OutDroppedYel = 0;
	statsCounterSet->OutDiscards = 0;
	statsCounterSet->OutMGMT = 0;



	return retVal;
}

/*******************************************************************************
* Agate_

*
* DESCRIPTION:
*        This routine Agate_gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (AGATE_MSD_COUNT_RX_ONLY, AGATE_MSD_COUNT_TX_ONLY, 
*                    and AGATE_MSD_COUNT_RX_TX)
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
MSD_STATUS Agate_gstatsGetHistogramModeIntf
(
    IN  MSD_QD_DEV   *dev,
    OUT MSD_HISTOGRAM_MODE    *mode
)
{
	return Agate_gstatsGetHistogramMode(dev, (AGATE_MSD_HISTOGRAM_MODE*)mode);
}

/*******************************************************************************
* Agate_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (AGATE_MSD_COUNT_RX_ONLY, AGATE_MSD_COUNT_TX_ONLY, 
*                    and AGATE_MSD_COUNT_RX_TX)
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
*        None.
*
*******************************************************************************/
MSD_STATUS Agate_gstatsSetHistogramModeIntf
(
    IN MSD_QD_DEV   *dev,
    IN MSD_HISTOGRAM_MODE        mode
)
{
	return Agate_gstatsSetHistogramMode(dev, (AGATE_MSD_HISTOGRAM_MODE)mode);
}
